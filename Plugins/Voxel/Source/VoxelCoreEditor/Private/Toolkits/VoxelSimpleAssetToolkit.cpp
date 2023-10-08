// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Toolkits/VoxelSimpleAssetToolkit.h"
#include "Toolkits/SVoxelSimpleAssetEditorViewport.h"
#include "ImageUtils.h"
#include "Engine/Texture2D.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Components/StaticMeshComponent.h"
#include "Subsystems/AssetEditorSubsystem.h"

const FName FVoxelSimpleAssetToolkit::DetailsTabId = "VoxelSimpleAssetEditor_Details";
const FName FVoxelSimpleAssetToolkit::ViewportTabId = "VoxelSimpleAssetEditor_Viewport";

void FVoxelSimpleAssetToolkit::Initialize()
{
	Super::Initialize();

	{
		FDetailsViewArgs Args;
		Args.bHideSelectionTip = true;
		Args.NotifyHook = GetNotifyHook();
		Args.DefaultsOnlyVisibility = EEditDefaultsOnlyNodeVisibility::Hide;

		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PrivateDetailsView = PropertyModule.CreateDetailView(Args);
		PrivateDetailsView->SetObject(GetAsset());
	}

	PrivatePreviewScene = MakeVoxelShared<FAdvancedPreviewScene>(FPreviewScene::ConstructionValues());
	PrivatePreviewScene->SetFloorVisibility(ShowFloor(), true);

	SetupPreview();
	UpdatePreview();

	// Make sure to make the viewport after UpdatePreview so that the component bounds are correct
	Viewport = SNew(SVoxelSimpleAssetEditorViewport)
		.PreviewScene(PrivatePreviewScene)
		.InitialViewRotation(GetInitialViewRotation())
		.InitialViewDistance(GetInitialViewDistance())
		.Toolkit(SharedThis(this));

	if (!QueuedStatsText.IsEmpty())
	{
		Viewport->UpdateStatsText(QueuedStatsText);
		QueuedStatsText = {};
	}

	if (const FObjectProperty* TextureProperty = GetTextureProperty())
	{
		if (!TextureProperty->GetObjectPropertyValue_InContainer(GetAsset()))
		{
			bCaptureThumbnail = true;
		}
	}
}

TSharedPtr<FTabManager::FLayout> FVoxelSimpleAssetToolkit::GetLayout() const
{
	return FTabManager::NewLayout("Standalone_VoxelSimpleAssetEditor_Layout_v0")
		->AddArea
		(
			FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewSplitter()
				->SetOrientation(Orient_Horizontal)
				->SetSizeCoefficient(0.9f)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.3f)
					->AddTab(DetailsTabId, ETabState::OpenedTab)
				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.7f)
					->AddTab(ViewportTabId, ETabState::OpenedTab)
				)
			)
		);
}

TSharedPtr<SWidget> FVoxelSimpleAssetToolkit::GetMenuOverlay() const
{
	if (GetAsset()->GetClass() == GetObjectProperty()->PropertyClass)
	{
		return nullptr;
	}

	return
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.ColorAndOpacity(FSlateColor::UseSubduedForeground())
			.ShadowOffset(FVector2D::UnitVector)
			.Text(INVTEXT("Class: "))
		]
		+SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SNew(SSpacer)
			.Size(FVector2D(2.0f,1.0f))
		]
		+SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.ShadowOffset(FVector2D::UnitVector)
			.Text(GetAsset()->GetClass()->GetDisplayNameText())
			.TextStyle(FAppStyle::Get(), "Common.InheritedFromBlueprintTextStyle")
			.ToolTipText(INVTEXT("The class that the current asset is based on"))
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.VAlign(VAlign_Center)
			.ButtonStyle(FAppStyle::Get(), "HoverHintOnly")
			.OnClicked_Lambda([this]
			{
				if (const UBlueprintGeneratedClass* ParentBlueprintGeneratedClass = Cast<UBlueprintGeneratedClass>(GetAsset()->GetClass()))
				{
					if (ParentBlueprintGeneratedClass->ClassGeneratedBy)
					{
						TArray<UObject*> Objects;
						Objects.Add(ParentBlueprintGeneratedClass->ClassGeneratedBy);
						GEditor->SyncBrowserToObjects(Objects);
					}
				}

				return FReply::Handled();
			})
			.Visibility_Lambda([this]
			{
				return Cast<UBlueprintGeneratedClass>(GetAsset()->GetClass()) ? EVisibility::Visible : EVisibility::Collapsed;
			})
			.ToolTipText(INVTEXT("Find parent in Content Browser"))
			.ContentPadding(4.0f)
			.ForegroundColor(FSlateColor::UseForeground())
			[
				SNew(SImage)
				.Image(FAppStyle::GetBrush("Icons.Search"))
			]
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.VAlign(VAlign_Center)
			.ButtonStyle(FAppStyle::Get(), "HoverHintOnly")
			.OnClicked_Lambda([this]
			{
				if (const UBlueprintGeneratedClass* ParentBlueprintGeneratedClass = Cast<UBlueprintGeneratedClass>(GetAsset()->GetClass()))
				{
					if (ParentBlueprintGeneratedClass->ClassGeneratedBy)
					{
						GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(ParentBlueprintGeneratedClass->ClassGeneratedBy);
					}
				}

				return FReply::Handled();
			})
			.Visibility_Lambda([this]
			{
				return Cast<UBlueprintGeneratedClass>(GetAsset()->GetClass()) ? EVisibility::Visible : EVisibility::Collapsed;
			})
			.ToolTipText( INVTEXT("Open parent in editor") )
			.ContentPadding(4.0f)
			.ForegroundColor( FSlateColor::UseForeground() )
			[
				SNew(SImage)
				.Image(FAppStyle::GetBrush("Icons.Edit"))
			]
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SNew(SSpacer)
			.Size(FVector2D(8.0f, 1.0f))
		];
}

void FVoxelSimpleAssetToolkit::RegisterTabs(FRegisterTab RegisterTab)
{
	Super::RegisterTabs(RegisterTab);

	RegisterTab(DetailsTabId, INVTEXT("Details"), "LevelEditor.Tabs.Details", PrivateDetailsView);
	RegisterTab(ViewportTabId, INVTEXT("Viewport"), "LevelEditor.Tabs.Viewports", Viewport);
}

void FVoxelSimpleAssetToolkit::Tick()
{
	Super::Tick();

	if (bPreviewQueued)
	{
		UpdatePreview();
		bPreviewQueued = false;
	}
}

void FVoxelSimpleAssetToolkit::SaveDocuments()
{
	Super::SaveDocuments();

	if (!SaveCameraPosition())
	{
		return;
	}

	if (!ensure(Viewport))
	{
		return;
	}

	const TSharedPtr<FEditorViewportClient> ViewportClient = Viewport->GetViewportClient();
	if (!ensure(ViewportClient))
	{
		return;
	}

	const UObject* Asset = GetAsset();
	if (!ensure(Asset))
	{
		return;
	}

	GConfig->SetString(
		TEXT("FVoxelSimpleAssetEditorToolkit_LastPosition"),
		*Asset->GetPathName(),
		*ViewportClient->GetViewLocation().ToString(),
		GEditorPerProjectIni);

	GConfig->SetString(
		TEXT("FVoxelSimpleAssetEditorToolkit_LastRotation"),
		*Asset->GetPathName(),
		*ViewportClient->GetViewRotation().ToString(),
		GEditorPerProjectIni);
}

void FVoxelSimpleAssetToolkit::LoadDocuments()
{
	Super::LoadDocuments();

	if (!SaveCameraPosition())
	{
		return;
	}

	if (!ensure(Viewport))
	{
		return;
	}

	const TSharedPtr<FEditorViewportClient> ViewportClient = Viewport->GetViewportClient();
	if (!ensure(ViewportClient))
	{
		return;
	}

	const UObject* Asset = GetAsset();
	if (!ensure(Asset))
	{
		return;
	}

	FString LocationString;
	if (GConfig->GetString(
		TEXT("FVoxelSimpleAssetEditorToolkit_LastPosition"),
		*Asset->GetPathName(),
		LocationString,
		GEditorPerProjectIni))
	{
		FVector Location = FVector::ZeroVector;
		Location.InitFromString(LocationString);
		ViewportClient->SetViewLocation(Location);
	}

	FString RotationString;
	if (GConfig->GetString(
		TEXT("FVoxelSimpleAssetEditorToolkit_LastRotation"),
		*Asset->GetPathName(),
		RotationString,
		GEditorPerProjectIni))
	{
		FRotator Rotation = FRotator::ZeroRotator;
		Rotation.InitFromString(RotationString);
		ViewportClient->SetViewRotation(Rotation);
	}
}

void FVoxelSimpleAssetToolkit::PostEditChange(const FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChange(PropertyChangedEvent);

	if (PropertyChangedEvent.ChangeType == EPropertyChangeType::Interactive)
	{
		return;
	}

	CaptureThumbnail();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelSimpleAssetToolkit::DrawThumbnail(FViewport& InViewport)
{
	if (!bCaptureThumbnail)
	{
		return;
	}
	bCaptureThumbnail = false;

	UObject* Asset = GetAsset();
	if (!ensure(Asset))
	{
		return;
	}

	const int32 SrcWidth = InViewport.GetSizeXY().X;
	const int32 SrcHeight = InViewport.GetSizeXY().Y;

	TArray<FColor> Colors;
	if (!ensure(InViewport.ReadPixels(Colors)) ||
		!ensure(Colors.Num() == SrcWidth * SrcHeight))
	{
		return;
	}

	TArray<FColor> ScaledColors;
	constexpr int32 ScaledWidth = 512;
	constexpr int32 ScaledHeight = 512;
	FImageUtils::CropAndScaleImage(SrcWidth, SrcHeight, ScaledWidth, ScaledHeight, Colors, ScaledColors);

	FCreateTexture2DParameters Params;
	Params.bDeferCompression = true;

	UTexture2D* ThumbnailImage = FImageUtils::CreateTexture2D(
		ScaledWidth,
		ScaledHeight,
		ScaledColors,
		Asset,
		{},
		RF_NoFlags,
		Params);

	if (!ensure(ThumbnailImage))
	{
		return;
	}

	const FObjectProperty* Property = GetTextureProperty();
	if (!ensure(Property))
	{
		return;
	}

	Property->SetObjectPropertyValue(Property->ContainerPtrToValuePtr<UObject*>(Asset), ThumbnailImage);

	// Broadcast an object property changed event to update the content browser
	FPropertyChangedEvent PropertyChangedEvent(nullptr);
	FCoreUObjectDelegates::OnObjectPropertyChanged.Broadcast(Asset, PropertyChangedEvent);
}

void FVoxelSimpleAssetToolkit::UpdateStatsText(const FString& Message)
{
	if (!Viewport)
	{
		QueuedStatsText = Message;
		return;
	}

	Viewport->UpdateStatsText(Message);
}

void FVoxelSimpleAssetToolkit::BindToggleCommand(const TSharedPtr<FUICommandInfo>& UICommandInfo, bool& bValue)
{
	GetCommands()->MapAction(
		UICommandInfo,
		MakeWeakPtrDelegate(this, [this, &bValue]
		{
			bValue = !bValue;
			UpdatePreview();
		}),
		{},
		MakeWeakPtrDelegate(this, [&bValue]
		{
			return bValue ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
		}));
}

void FVoxelSimpleAssetToolkit::SetFloorScale(const FVector& Scale) const
{
	ConstCast(GetPreviewScene().GetFloorMeshComponent())->SetWorldScale3D(Scale);
}

void FVoxelSimpleAssetToolkit::CaptureThumbnail()
{
	if (!GetTextureProperty())
	{
		return;
	}

	bCaptureThumbnail = true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FObjectProperty* FVoxelSimpleAssetToolkit::GetTextureProperty() const
{
	FProperty* Property = GetAsset()->GetClass()->FindPropertyByName(STATIC_FNAME("ThumbnailTexture"));
	if (!Property)
	{
		return nullptr;
	}

	FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property);
	if (!ensure(ObjectProperty) ||
		!ensure(ObjectProperty->PropertyClass == UTexture2D::StaticClass()))
	{
		return nullptr;
	}

	return ObjectProperty;
}