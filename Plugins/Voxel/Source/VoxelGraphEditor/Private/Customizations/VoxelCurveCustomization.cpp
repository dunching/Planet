// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelCurveCustomization.h"

#include "VoxelCurveNode.h"
#include "Widgets/SVoxelCurveThumbnail.h"
#include "Widgets/SVoxelCurveKeySelector.h"
#include "Widgets/SVoxelCurveTemplateBar.h"

#include "AssetToolsModule.h"
#include "EditorFontGlyphs.h"
#include "SCurveEditorPanel.h"
#include "SCurveEditorView.h"
#include "ContentBrowserModule.h"
#include "SCurveKeyDetailPanel.h"
#include "Factories/CurveFactory.h"
#include "IContentBrowserSingleton.h"
#include "Tree/SCurveEditorTreePin.h"

void FVoxelCurveEditorModel::AddKeys(const TArrayView<const FKeyPosition> InKeyPositions, const TArrayView<const FKeyAttributes> InAttributes, TArrayView<TOptional<FKeyHandle>>* OutKeyHandles)
{
	CurvePreEditChangeDelegate.Broadcast();
	FRichCurveEditorModel::AddKeys(InKeyPositions, InAttributes, OutKeyHandles);
	CurvePostEditChangeDelegate.Broadcast(*Curve);
}

void FVoxelCurveEditorModel::RemoveKeys(const TArrayView<const FKeyHandle> InKeys)
{
	CurvePreEditChangeDelegate.Broadcast();
	FRichCurveEditorModel::RemoveKeys(InKeys);
	CurvePostEditChangeDelegate.Broadcast(*Curve);
}

void FVoxelCurveEditorModel::SetKeyPositions(const TArrayView<const FKeyHandle> InKeys, const TArrayView<const FKeyPosition> InKeyPositions, const EPropertyChangeType::Type ChangeType)
{
	CurvePreEditChangeDelegate.Broadcast();
	FRichCurveEditorModel::SetKeyPositions(InKeys, InKeyPositions, ChangeType);
	CurvePostEditChangeDelegate.Broadcast(*Curve);
}

void FVoxelCurveEditorModel::SetKeyAttributes(const TArrayView<const FKeyHandle> InKeys, const TArrayView<const FKeyAttributes> InAttributes, const EPropertyChangeType::Type ChangeType)
{
	CurvePreEditChangeDelegate.Broadcast();
	FRichCurveEditorModel::SetKeyAttributes(InKeys, InAttributes, ChangeType);
	CurvePostEditChangeDelegate.Broadcast(*Curve);
}

void FVoxelCurveEditorModel::SetCurveAttributes(const FCurveAttributes& InCurveAttributes)
{
	CurvePreEditChangeDelegate.Broadcast();
	FRichCurveEditorModel::SetCurveAttributes(InCurveAttributes);
	CurvePostEditChangeDelegate.Broadcast(*Curve);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelCurveDetailsTreeItem::FVoxelCurveDetailsTreeItem(const TSharedPtr<FRichCurve>& Curve)
	: Curve(Curve)
{
}

TSharedPtr<SWidget> FVoxelCurveDetailsTreeItem::GenerateCurveEditorTreeWidget(const FName& InColumnName, TWeakPtr<FCurveEditor> InCurveEditor, FCurveEditorTreeItemID InTreeItemID, const TSharedRef<ITableRow>& TableRow)
{
	if (InColumnName == ColumnNames.Label)
	{
		return
			SNew(SBox)
			.VAlign(VAlign_Center)
			.Padding(0.f, 0.f, 5.f, 0.f)
			[
				SNew(STextBlock)
				.Font(FAppStyle::Get().GetFontStyle("FontAwesome.8"))
				.Text(FEditorFontGlyphs::Circle)
				.ColorAndOpacity(FSlateColor(FLinearColor(1.0f, 0.05f, 0.05f)))
			];
	}

	if (InColumnName == ColumnNames.PinHeader)
	{
		return SNew(SCurveEditorTreePin, InCurveEditor, InTreeItemID, TableRow);
	}

	return nullptr;
}

void FVoxelCurveDetailsTreeItem::CreateCurveModels(TArray<TUniquePtr<FCurveModel>>& OutCurveModels)
{
	TUniquePtr<FVoxelCurveEditorModel> CurveModel = MakeUnique<FVoxelCurveEditorModel>(Curve);
	CurveModel->SetColor(FLinearColor(1.0f, 0.05f, 0.05f));
	CurveModel->CurvePreEditChangeDelegate.AddSP(this, &FVoxelCurveDetailsTreeItem::PreEditChange);
	CurveModel->CurvePostEditChangeDelegate.AddSP(this, &FVoxelCurveDetailsTreeItem::PostEditChange);

	OutCurveModels.Add(MoveTemp(CurveModel));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelCurveCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	CurveHandle = PropertyHandle->GetChildHandleStatic(FVoxelCurve, Curve);

	TSharedPtr<SWidget> ValueWidget;
	if (PropertyHandle->GetNumPerObjectValues() > 1)
	{
		ValueWidget =
			SNew(SVoxelDetailText)
			.Text(INVTEXT("Multiple Values"));
	}
	else
	{
		CachedCurve = MakeShared<FRichCurve>(FVoxelEditorUtilities::GetStructPropertyValue<FRichCurve>(CurveHandle));

		const FRichCurve& Curve = FVoxelEditorUtilities::GetStructPropertyValue<FRichCurve>(CurveHandle);
		CurveThumbnailWidget = SNew(SVoxelCurveThumbnail, &Curve);

		ValueWidget = CurveThumbnailWidget;
	}

	HeaderRow
	.ShouldAutoExpand(true)
	.NameContent()
	[
		PropertyHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	[
		ValueWidget.ToSharedRef()
	];
}

void FVoxelCurveCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	if (PropertyHandle->GetNumPerObjectValues() > 1)
	{
		return;
	}

	TSharedPtr<FCurveEditor> CurveEditor = MakeShared<FCurveEditor>();
	WeakCurveEditor = CurveEditor;

	const FCurveEditorInitParams InitParams;
	CurveEditor->InitCurveEditor(InitParams);
	CurveEditor->GridLineLabelFormatXAttribute = INVTEXT("{0}");

	const TSharedRef<SCurveEditorPanel> CurveEditorPanel =
		SNew(SCurveEditorPanel, CurveEditor.ToSharedRef())
		.MinimumViewPanelHeight(400.0f)
		.TreeContent()
		[
			SNullWidget::NullWidget
		];

	const TSharedRef<FVoxelCurveDetailsTreeItem> DetailsTreeItem = MakeShared<FVoxelCurveDetailsTreeItem>(CachedCurve);
	WeakTreeItem = DetailsTreeItem;

	DetailsTreeItem->PostEditChangeDelegate.AddSP(this, &FVoxelCurveCustomization::PostEditChange);

	FCurveEditorTreeItem* EditorTreeItem = CurveEditor->AddTreeItem(FCurveEditorTreeItemID::Invalid());
	EditorTreeItem->SetStrongItem(DetailsTreeItem);
	for (const FCurveModelID& CurveModelId : EditorTreeItem->GetOrCreateCurves(CurveEditor.Get()))
	{
		CurveEditor->PinCurve(CurveModelId);
	}

	ChildBuilder.AddCustomRow(INVTEXT("Curve"))
	.WholeRowContent()
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			MakeToolbar()
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.f, 3.f)
		[
			SNew(SVoxelCurveTemplateBar, CurveEditor.ToSharedRef())
		]
		+ SVerticalBox::Slot()
		.Padding(0.f, 0.f, 0.f, 5.f)
		[
			CurveEditorPanel
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.AutoWidth()
			.Padding(0.f, 0.f, 8.f, 0.f)
			[
				SNew(SVoxelCurveKeySelector, CurveEditor, EditorTreeItem->GetID())
			]
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.AutoWidth()
			.Padding(0.f, 0.f, 3.f, 0.f)
			[
				SNew(STextBlock)
				.Text(INVTEXT("Key Data"))
			]
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.FillWidth(1.f)
			[
				CurveEditorPanel->GetKeyDetailsView().ToSharedRef()
			]
		]
	];
}

void FVoxelCurveCustomization::Tick()
{
	if (CurveHandle->GetNumPerObjectValues() > 1)
	{
		return;
	}

	const FRichCurve NewCurve = FVoxelEditorUtilities::GetStructPropertyValue<FRichCurve>(CurveHandle);
	if (!(*CachedCurve == NewCurve))
	{
		*CachedCurve = NewCurve;
		CurveThumbnailWidget->UpdateCurve(&*CachedCurve);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedRef<SWidget> FVoxelCurveCustomization::MakeToolbar()
{
	FToolBarBuilder ToolBarBuilder(nullptr, FMultiBoxCustomization::None, nullptr, true);
	ToolBarBuilder.SetStyle(&FAppStyle::Get(), "SlimToolbar");

	ToolBarBuilder.AddComboButton(
		FUIAction(),
		FOnGetContent::CreateLambda([this]
		{
			FAssetPickerConfig AssetPickerConfig;
			AssetPickerConfig.OnAssetSelected = FOnAssetSelected::CreateSP(this, &FVoxelCurveCustomization::CopySelectedCurve);
			AssetPickerConfig.bAllowNullSelection = false;
			AssetPickerConfig.InitialAssetViewType = EAssetViewType::List;
			AssetPickerConfig.Filter.ClassPaths.Add(UCurveFloat::StaticClass()->GetClassPathName());

			const FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));

			return
				SNew(SBox)
				.WidthOverride(300.0f)
				.HeightOverride(480.f)
				[
					ContentBrowserModule.Get().CreateAssetPicker(AssetPickerConfig)
				];
		}),
		INVTEXT("Import"),
		INVTEXT("Import data from another Curve asset"),
		FSlateIcon(FAppStyle::Get().GetStyleSetName(), "MeshPaint.Import.Small"));

	ToolBarBuilder.AddToolBarButton(
		FUIAction(FExecuteAction::CreateSP(this, &FVoxelCurveCustomization::SaveCurveToAsset)),
		{},
		INVTEXT("Save"),
		INVTEXT("Save data to new Curve asset"),
		FSlateIcon(FAppStyle::Get().GetStyleSetName(), "MeshPaint.Save.Small"));

	return ToolBarBuilder.MakeWidget();
}

void FVoxelCurveCustomization::PostEditChange(const FRichCurve& InCurve) const
{
	FVoxelEditorUtilities::SetStructPropertyValue(CurveHandle, InCurve);
}

void FVoxelCurveCustomization::CopySelectedCurve(const FAssetData& AssetData) const
{
	ON_SCOPE_EXIT
	{
		FSlateApplication::Get().DismissAllMenus();
	};

	const UCurveFloat* CurveAsset = Cast<UCurveFloat>(AssetData.GetAsset());
	if (!ensure(CurveAsset))
	{
		return;
	}

	FVoxelEditorUtilities::SetStructPropertyValue(CurveHandle, CurveAsset->FloatCurve);
	const TSharedPtr<FVoxelCurveDetailsTreeItem> TreeItem = WeakTreeItem.Pin();
	if (!ensure(TreeItem))
	{
		return;
	}

	*TreeItem->Curve = CurveAsset->FloatCurve;

	const TSharedPtr<FCurveEditor> CurveEditor = WeakCurveEditor.Pin();
	if (!ensure(CurveEditor))
	{
		return;
	}

	CurveEditor->RemoveAllCurves();

	FCurveEditorTreeItem* EditorTreeItem = CurveEditor->AddTreeItem(FCurveEditorTreeItemID::Invalid());
	EditorTreeItem->SetStrongItem(TreeItem);
	for (const FCurveModelID& CurveModelId : EditorTreeItem->GetOrCreateCurves(CurveEditor.Get()))
	{
		CurveEditor->PinCurve(CurveModelId);
	}
}

void FVoxelCurveCustomization::SaveCurveToAsset() const
{
	const FAssetToolsModule& AssetToolsModule = FAssetToolsModule::GetModule();

	UCurveFloatFactory* Factory = NewObject<UCurveFloatFactory>();
	UCurveFloat* NewCurve = Cast<UCurveFloat>(AssetToolsModule.Get().CreateAssetWithDialog(UCurveFloat::StaticClass(), Factory));
	if (!NewCurve)
	{
		return;
	}

	NewCurve->FloatCurve = FVoxelEditorUtilities::GetStructPropertyValue<FRichCurve>(CurveHandle);
	NewCurve->PostEditChange();
}

DEFINE_VOXEL_STRUCT_LAYOUT(FVoxelCurve, FVoxelCurveCustomization);