// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "SVoxelGraphObjectSelector.h"
#include "SAssetDropTarget.h"

void SVoxelGraphObjectSelector::Construct(const FArguments& InArgs)
{
	if (InArgs._AllowedClass)
	{
		AllowedClasses.Add(InArgs._AllowedClass);
		ObjectClass = InArgs._AllowedClass;
	}
	else if (InArgs._AllowedClasses.Num() > 0)
	{
		ObjectClass = InArgs._AllowedClasses[0];
	}

	AllowedClasses.Append(InArgs._AllowedClasses);
	ensure(AllowedClasses.Num() > 0);

	ObjectPath = InArgs._ObjectPath;

	OnSetObject = InArgs._OnObjectChanged;

	bIsActor = ObjectClass->IsChildOf(AActor::StaticClass());

	TSharedPtr<SHorizontalBox> ButtonBox = nullptr;

	FObjectOrAssetData Value = GetValue();
	AssetThumbnail = MakeVoxelShared<FAssetThumbnail>(Value.AssetData, InArgs._ThumbnailSize.X, InArgs._ThumbnailSize.Y, InArgs._ThumbnailPool);

	ChildSlot
	[
		SNew(SAssetDropTarget)
		.OnAreAssetsAcceptableForDropWithReason(this, &SVoxelGraphObjectSelector::OnAssetDraggedOver)
		.OnAssetsDropped(this, &SVoxelGraphObjectSelector::OnAssetDropped)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.Padding(0.f, 3.f, 5.f, 0.f)
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(SBorder)
				.Visibility(EVisibility::SelfHitTestInvisible)
				.Padding(FMargin(0.f, 0.f, 4.f, 4.f))
				.BorderImage(FAppStyle::Get().GetBrush("PropertyEditor.AssetTileItem.DropShadow"))
				[
					SNew(SOverlay)
					+ SOverlay::Slot()
					.Padding(1.f)
					[
						SAssignNew(ThumbnailBorder, SBorder)
						.Padding(0.f)
						.BorderImage(FStyleDefaults::GetNoBrush())
						.OnMouseDoubleClick(this, &SVoxelGraphObjectSelector::OnAssetThumbnailDoubleClick)
						[
							SNew(SBox)
							.WidthOverride(InArgs._ThumbnailSize.X)
							.HeightOverride(InArgs._ThumbnailSize.Y)
							[
								AssetThumbnail->MakeThumbnailWidget()
							]
						]
					]
					+ SOverlay::Slot()
					[
						SNew(SImage)
						.Image_Lambda([this]
						{
							return ThumbnailBorder->IsHovered() ? FAppStyle::Get().GetBrush(STATIC_FNAME("PropertyEditor.AssetThumbnailBorderHovered")) : FAppStyle::Get().GetBrush(STATIC_FNAME("PropertyEditor.AssetThumbnailBorder"));
						})
						.Visibility(EVisibility::SelfHitTestInvisible)
					]
				]
			]
			+ SHorizontalBox::Slot()
			.Padding(0.f)
			.VAlign(VAlign_Center)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.VAlign(VAlign_Center)
				.AutoHeight()
				[
					SAssignNew(AssetComboButton, SComboButton)
					.OnGetMenuContent(this, &SVoxelGraphObjectSelector::OnGetMenuContent)
					.OnMenuOpenChanged(this, &SVoxelGraphObjectSelector::OnMenuOpenChanged)
					.ButtonContent()
					[
						SNew(STextBlock)
						.Font(FAppStyle::GetFontStyle("PropertyWindow.NormalFont"))
						.Text(this, &SVoxelGraphObjectSelector::OnGetAssetName)
					]
				]
				+ SVerticalBox::Slot()
				.VAlign(VAlign_Center)
				.AutoHeight()
				[
					SNew(SBox)
					.Padding(FMargin(0.f, 2.f, 4.f, 2.f))
					[
						SAssignNew(ButtonBox, SHorizontalBox)
					]
				]
			]
		]
	];

	if (!bIsActor)
	{
		ButtonBox->AddSlot()
		.VAlign(VAlign_Center)
		.AutoWidth()
		.Padding(2.f, 0.f)
		[
			PropertyCustomizationHelpers::MakeUseSelectedButton(FSimpleDelegate::CreateSP(this, &SVoxelGraphObjectSelector::OnUseClicked))
		];
	}

	ButtonBox->AddSlot()
	.Padding(2.f, 0.f)
	.AutoWidth()
	.VAlign(VAlign_Center)
	[
		PropertyCustomizationHelpers::MakeBrowseButton(
			FSimpleDelegate::CreateSP(this, &SVoxelGraphObjectSelector::OnBrowseClicked),
			TAttribute<FText>(/*this, &SPropertyEditorAsset::GetOnBrowseToolTip*/),
			true,
			bIsActor
		)
	];

	if (bIsActor)
	{
		ButtonBox->AddSlot()
		.VAlign(VAlign_Center)
		.AutoWidth()
		.Padding(2.f, 0.f)
		[
			PropertyCustomizationHelpers::MakeInteractiveActorPicker(
				FOnGetAllowedClasses::CreateSP(this, &SVoxelGraphObjectSelector::OnGetAllowedClasses),
				FOnShouldFilterActor(),
				FOnActorSelected::CreateSP(this, &SVoxelGraphObjectSelector::OnActorSelected)
			)
		];
	}
}

bool SVoxelGraphObjectSelector::OnAssetDraggedOver(TArrayView<FAssetData> InAssets, FText& OutReason) const
{
	const UObject* AssetObject = InAssets[0].GetAsset();
	if (!AssetObject ||
		!AssetObject->IsA(ObjectClass))
	{
		return false;
	}

	return
		CanSetBasedOnCustomClasses(InAssets[0]) &&
		CanSetBasedOnAssetReferenceFilter(CachedAssetData, OutReason);
}

void SVoxelGraphObjectSelector::OnAssetDropped(const FDragDropEvent&, TArrayView<FAssetData> InAssets) const
{
	SetValue(InAssets[0].GetAsset());
}

TSharedRef<SWidget> SVoxelGraphObjectSelector::OnGetMenuContent() const
{
	const FObjectOrAssetData Value = GetValue();

	if (bIsActor)
	{
		return PropertyCustomizationHelpers::MakeActorPickerWithMenu(
			Cast<AActor>(Value.Object),
			true,
			FOnShouldFilterActor::CreateSP(this, &SVoxelGraphObjectSelector::IsFilteredActor),
			FOnActorSelected::CreateSP(this, &SVoxelGraphObjectSelector::OnActorSelected),
			FSimpleDelegate::CreateSP(this, &SVoxelGraphObjectSelector::CloseComboButton),
			FSimpleDelegate::CreateSP(this, &SVoxelGraphObjectSelector::OnUseClicked));
	}

	return PropertyCustomizationHelpers::MakeAssetPickerWithMenu(
		Value.AssetData,
		true,
		AllowedClasses,
		{},
		PropertyCustomizationHelpers::GetNewAssetFactoriesForClasses(AllowedClasses),
		OnShouldFilterAsset,
		MakeWeakPtrDelegate(this, [this](const FAssetData& AssetData)
		{
			SetValue(AssetData);
		}),
		FSimpleDelegate::CreateSP(this, &SVoxelGraphObjectSelector::CloseComboButton),
		TSharedPtr<IPropertyHandle>(),
		OwnerAssetDataArray);
}

void SVoxelGraphObjectSelector::OnMenuOpenChanged(bool bOpen) const
{
	if (bOpen)
	{
		return;
	}

	AssetComboButton->SetMenuContent(SNullWidget::NullWidget);
}

bool SVoxelGraphObjectSelector::IsFilteredActor(const AActor* Actor) const
{
	return
		Actor &&
		Actor->IsA(ObjectClass) &&
		!Actor->IsChildActor() &&
		IsClassAllowed(Actor->GetClass());
}

void SVoxelGraphObjectSelector::CloseComboButton() const
{
	AssetComboButton->SetIsOpen(false);
}

FText SVoxelGraphObjectSelector::OnGetAssetName() const
{
	const FObjectOrAssetData Value = GetValue();

	FText Name = INVTEXT("None");
	if (Value.Object)
	{
		if (bIsActor)
		{
			if (const AActor* Actor = Cast<AActor>(Value.Object))
			{
				return FText::AsCultureInvariant(Actor->GetActorLabel());
			}

			return FText::AsCultureInvariant(Value.Object->GetName());
		}

		if (const UField* AsField = Cast<UField>(Value.Object))
		{
			return AsField->GetDisplayNameText();
		}

		return FText::AsCultureInvariant(Value.Object->GetName());
	}

	if (Value.AssetData.IsValid())
	{
		return FText::AsCultureInvariant(Value.AssetData.AssetName.ToString());
	}
	if (Value.ObjectPath.IsValid())
	{
		return FText::AsCultureInvariant(Value.ObjectPath.ToString());
	}

	return INVTEXT("None");
}

FReply SVoxelGraphObjectSelector::OnAssetThumbnailDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) const
{
	const FObjectOrAssetData Value = GetValue();

	if (UObject* ObjectToEdit = Value.AssetData.GetAsset())
	{
		if (ObjectToEdit->IsA(UWorld::StaticClass()))
		{
			constexpr bool bPromptUserToSave = true;
			constexpr bool bSaveMapPackages = true;
			constexpr bool bSaveContentPackages = true;
			if (!FEditorFileUtils::SaveDirtyPackages(bPromptUserToSave, bSaveMapPackages, bSaveContentPackages))
			{
				return FReply::Handled();
			}
		}

		GEditor->EditObject(ObjectToEdit);
	}

	return FReply::Handled();
}

void SVoxelGraphObjectSelector::OnUseClicked() const
{
	FEditorDelegates::LoadSelectedAssetsIfNeeded.Broadcast();

	const UObject* Selection = nullptr;
	if (ObjectClass && ObjectClass->IsChildOf(AActor::StaticClass() ))
	{
		Selection = GEditor->GetSelectedActors()->GetTop(ObjectClass);
	}
	else if (ObjectClass)
	{
		Selection = GEditor->GetSelectedObjects()->GetTop(ObjectClass);
	}

	if (Selection)
	{
		SetValue(Selection);
	}
}

void SVoxelGraphObjectSelector::OnBrowseClicked() const
{
	TArray<FAssetData> AssetDataList;
	AssetDataList.Add(GetValue().AssetData);

	GEditor->SyncBrowserToObjects(AssetDataList);
}

void SVoxelGraphObjectSelector::OnGetAllowedClasses(TArray<const UClass*>& OutAllowedClasses) const
{
	OutAllowedClasses.Append(AllowedClasses);
}

void SVoxelGraphObjectSelector::OnActorSelected(AActor* Actor) const
{
	SetValue(Actor);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

SVoxelGraphObjectSelector::FObjectOrAssetData SVoxelGraphObjectSelector::GetValue() const
{
	const FSoftObjectPath SoftObjectPath = FSoftObjectPath(ObjectPath.Get());

	if (UObject* Object = SoftObjectPath.ResolveObject())
	{
		return FObjectOrAssetData(Object);
	}

	if (SoftObjectPath.IsAsset())
	{
		const FString CurrentObjectPath = SoftObjectPath.ToString();
		if (SoftObjectPath.IsValid())
		{
			if (!CachedAssetData.IsValid() ||
				CachedAssetData.GetSoftObjectPath() != CurrentObjectPath)
			{
				static FName AssetRegistryName("AssetRegistry");

				const FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(AssetRegistryName);
				CachedAssetData = AssetRegistryModule.Get().GetAssetByObjectPath(CurrentObjectPath);
			}
		}

		return FObjectOrAssetData(CachedAssetData);
	}

	if (CachedAssetData.IsValid())
	{
		CachedAssetData = FAssetData();
	}

	return FObjectOrAssetData(SoftObjectPath);
}

void SVoxelGraphObjectSelector::SetValue(const FAssetData& AssetData) const
{
	AssetComboButton->SetIsOpen(false);

	if (!CanSetBasedOnCustomClasses(AssetData))
	{
		return;
	}

	FText AssetReferenceFilterFailureReason;
	if (!CanSetBasedOnAssetReferenceFilter(AssetData, AssetReferenceFilterFailureReason))
	{
		FNotificationInfo Info(AssetReferenceFilterFailureReason);
		Info.ExpireDuration = 4.f;
		FSlateNotificationManager::Get().AddNotification(Info);
		return;
	}

	OnSetObject.ExecuteIfBound(AssetData);

	if (AssetThumbnail->GetAssetData() != AssetData)
	{
		AssetThumbnail->SetAsset(AssetData);
	}
}

bool SVoxelGraphObjectSelector::CanSetBasedOnCustomClasses(const FAssetData& AssetData) const
{
	if (AssetData.IsValid())
	{
		// Force load the asset so the BP class can be found
		AssetData.GetAsset();

		return IsClassAllowed(AssetData.GetClass());
	}

	return true;
}

bool SVoxelGraphObjectSelector::CanSetBasedOnAssetReferenceFilter(const FAssetData& InAssetData, FText& OutOptionalFailureReason) const
{
	if (!GEditor ||
		!InAssetData.IsValid())
	{
		return true;
	}

	FAssetReferenceFilterContext AssetReferenceFilterContext;
	for (const FAssetData& AssetData : OwnerAssetDataArray)
	{
		if (AssetData.IsValid())
		{
			AssetReferenceFilterContext.ReferencingAssets.AddUnique(AssetData);
		}
	}

	const TSharedPtr<IAssetReferenceFilter> AssetReferenceFilter = GEditor->MakeAssetReferenceFilter(AssetReferenceFilterContext);
	if (AssetReferenceFilter.IsValid() &&
		!AssetReferenceFilter->PassesFilter(InAssetData, &OutOptionalFailureReason))
	{
		return false;
	}

	return true;
}

bool SVoxelGraphObjectSelector::IsClassAllowed(const UClass* InClass) const
{
	for (const UClass* AllowedClass : AllowedClasses)
	{
		const bool bAllowedClassIsInterface = AllowedClass->HasAnyClassFlags(CLASS_Interface);
		if (InClass->IsChildOf(AllowedClass) ||
			(bAllowedClassIsInterface && InClass->ImplementsInterface(AllowedClass)))
		{
			return true;
		}
	}

	return false;
}