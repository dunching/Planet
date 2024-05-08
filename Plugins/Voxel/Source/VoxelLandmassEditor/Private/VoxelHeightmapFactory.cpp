// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelHeightmapFactory.h"
#include "VoxelHeightmapImporter.h"

UVoxelHeightmapFactory::UVoxelHeightmapFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	bEditorImport = true;
	SupportedClass = UVoxelHeightmap::StaticClass();
}

bool UVoxelHeightmapFactory::ConfigureProperties()
{
	// Load from default
	UVoxelHeightmapFactory* Default = GetMutableDefault<UVoxelHeightmapFactory>();
	ScaleZ = Default->ScaleZ;
	ScaleXY = Default->ScaleXY;
	ImportPath = Default->ImportPath;

	ON_SCOPE_EXIT
	{
		// Save to default
		Default->ScaleZ = ScaleZ;
		Default->ScaleXY = ScaleXY;
		Default->ImportPath = ImportPath;
	};

	const TSharedRef<SVoxelHeightmapFactoryDetails> PickerWindow =
		SNew(SVoxelHeightmapFactoryDetails)
		.DetailsObject(this)
		.CanCreate_Lambda([&]
		{
			return !ImportPath.FilePath.IsEmpty();
		})
		.OnCreate_Lambda([&]
		{
			CachedHeightmap = Import(ImportPath.FilePath);
			return CachedHeightmap.IsValid();
		});

	GEditor->EditorAddModalWindow(PickerWindow);

	return CachedHeightmap.IsValid();
}

FString UVoxelHeightmapFactory::GetDefaultNewAssetName() const
{
	return FPaths::GetBaseFilename(ImportPath.FilePath);
}

UObject* UVoxelHeightmapFactory::FactoryCreateNew(UClass* Class, UObject* InParent, const FName Name, const EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	if (!ensure(CachedHeightmap))
	{
		return nullptr;
	}

	UVoxelHeightmap* Asset = NewObject<UVoxelHeightmap>(InParent, Class, Name, Flags);
	Asset->ImportWidth = CachedHeightmap->GetSizeX();
	Asset->ImportHeight = CachedHeightmap->GetSizeY();
	Asset->ImportPath = ImportPath;
	Asset->Config.ScaleZ = ScaleZ;
	Asset->Config.ScaleXY = ScaleXY;
	Asset->SourceHeightmap = CachedHeightmap;
	Asset->UpdateHeightmap();

	CachedHeightmap.Reset();

	return Asset;
}

bool UVoxelHeightmapFactory::CanReimport(UObject* Obj, TArray<FString>& OutFilenames)
{
	const UVoxelHeightmap* Asset = Cast<UVoxelHeightmap>(Obj);
	if (!Asset)
	{
		// Can happen
		return false;
	}

	OutFilenames.Add(Asset->ImportPath.FilePath);

	return true;
}

void UVoxelHeightmapFactory::SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths)
{
	UVoxelHeightmap* Asset = Cast<UVoxelHeightmap>(Obj);
	if (!ensure(Asset))
	{
		return;
	}

	if (!ensure(NewReimportPaths.Num() == 1))
	{
		return;
	}

	Asset->ImportPath.FilePath = NewReimportPaths[0];
}

EReimportResult::Type UVoxelHeightmapFactory::Reimport(UObject* Obj)
{
	UVoxelHeightmap* Asset = Cast<UVoxelHeightmap>(Obj);
	if (!ensure(Asset))
	{
		return EReimportResult::Failed;
	}

	const TSharedPtr<FVoxelHeightmap> Heightmap = Import(Asset->ImportPath.FilePath);
	if (!Heightmap)
	{
		return EReimportResult::Failed;
	}

	Asset->ImportWidth = Heightmap->GetSizeX();
	Asset->ImportHeight = Heightmap->GetSizeY();
	Asset->SourceHeightmap = Heightmap;
	Asset->UpdateHeightmap();
	Asset->MarkPackageDirty();

	return EReimportResult::Succeeded;
}

int32 UVoxelHeightmapFactory::GetPriority() const
{
	return ImportPriority;
}

TSharedPtr<FVoxelHeightmap> UVoxelHeightmapFactory::Import(const FString& Path)
{
	VOXEL_FUNCTION_COUNTER();

	FString Error;
	FIntPoint Size;
	int32 BitDepth;
	TArray64<uint8> HeightmapData;
	if (!FVoxelHeightmapImporter::Import(Path, Error, Size, BitDepth, HeightmapData))
	{
		VOXEL_MESSAGE(Error, "Heightmap failed to import: {0}", Error);
		return nullptr;
	}

	TVoxelArray<uint16> Heights;
	FVoxelUtilities::SetNumFast(Heights, Size.X * Size.Y);

	if (BitDepth == 8)
	{
		for (int32 Index = 0; Index < Size.X * Size.Y; Index++)
		{
			Heights[Index] = HeightmapData[Index] << 8;
		}
	}
	else if (BitDepth == 16)
	{
		Heights = TVoxelArray<uint16>(ReinterpretCastVoxelArrayView<uint16>(HeightmapData));
	}
	else if (BitDepth == 32)
	{
		const TVoxelArrayView64<float> FloatHeights = ReinterpretCastVoxelArrayView<float>(HeightmapData);

		float Min = FloatHeights[0];
		float Max = FloatHeights[0];
		for (const float Height : FloatHeights)
		{
			Min = FMath::Min(Min, Height);
			Max = FMath::Max(Max, Height);
		}

		for (int32 Index = 0; Index < Size.X * Size.Y; Index++)
		{
			const float Value = (FloatHeights[Index] - Min) / (Max - Min);
			Heights[Index] = FVoxelUtilities::ClampToUINT16(FMath::RoundToInt(Value));
		}
	}
	else
	{
		ensure(false);
		return nullptr;
	}

	const TSharedRef<FVoxelHeightmap> Data = MakeVoxelShared<FVoxelHeightmap>();

	Data->Initialize(
		Size.X,
		Size.Y,
		MoveTemp(Heights));

	return Data;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void SVoxelHeightmapFactoryDetails::Construct(const FArguments& Args)
{
	SWindow::Construct(
		SWindow::FArguments()
		.Title(INVTEXT("Import Heightmap"))
		.SizingRule(ESizingRule::Autosized));

	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bAllowSearch = false;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;

	const TSharedRef<IDetailsView> DetailsPanel = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	DetailsPanel->SetObject(Args._DetailsObject);

	SetContent(
		SNew(SBorder)
		.Visibility(EVisibility::Visible)
		.BorderImage(FAppStyle::GetBrush("Menu.Background"))
		[
			SNew(SBox)
			.Visibility(EVisibility::Visible)
			.WidthOverride(520.0f)
			[
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				.AutoHeight()
				.MaxHeight(500)
				[
					SNew(SScrollBox)
					+ SScrollBox::Slot()
					[
						DetailsPanel
					]
				]
				+SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Bottom)
				.Padding(8)
				[
					SNew(SUniformGridPanel)
					.SlotPadding(FAppStyle::GetMargin("StandardDialog.SlotPadding"))
					+ SUniformGridPanel::Slot(0, 0)
					[
						SNew(SButton)
						.Text(INVTEXT("Create"))
						.HAlign(HAlign_Center)
						.IsEnabled(Args._CanCreate)
						.ContentPadding(FAppStyle::GetMargin("StandardDialog.ContentPadding"))
						.OnClicked_Lambda([=]
						{
							if (Args._OnCreate.Execute())
							{
								RequestDestroyWindow();
							}
							return FReply::Handled();
						})
						.ButtonStyle(FAppStyle::Get(), "FlatButton.Success")
						.TextStyle(FAppStyle::Get(), "FlatButton.DefaultTextStyle")
					]
					+SUniformGridPanel::Slot(1,0)
					[
						SNew(SButton)
						.Text(INVTEXT("Cancel"))
						.HAlign(HAlign_Center)
						.ContentPadding(FAppStyle::GetMargin("StandardDialog.ContentPadding"))
						.OnClicked_Lambda([this]
						{
							RequestDestroyWindow();
							return FReply::Handled();
						})
						.ButtonStyle(FAppStyle::Get(), "FlatButton.Default")
						.TextStyle(FAppStyle::Get(), "FlatButton.DefaultTextStyle")
					]
				]
			]
		]);
}
