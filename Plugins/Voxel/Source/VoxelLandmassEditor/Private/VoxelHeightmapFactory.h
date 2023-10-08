// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelHeightmap.h"
#include "VoxelHeightmapFactory.generated.h"

class FVoxelHeightmap;

UCLASS()
class UVoxelHeightmapFactory : public UFactory, public FReimportHandler
{
	GENERATED_BODY()

public:
	UVoxelHeightmapFactory();

	// In unreal units (cm)
	UPROPERTY(EditAnywhere, Category = "Import configuration")
	float ScaleZ = 25600;

	// In unreal units (cm)
	UPROPERTY(EditAnywhere, Category = "Import configuration", meta = (DisplayName = "Scale XY"))
	float ScaleXY = 100;

	UPROPERTY(EditAnywhere, Category = "Import", meta = (FilePathFilter = "Heightmap file (*.png; *.raw; *.r16)|*.png;*.raw;*.r16"))
	FFilePath ImportPath;

	TSharedPtr<FVoxelHeightmap> CachedHeightmap;

	//~ Begin UFactory Interface
	virtual bool ConfigureProperties() override;
	virtual FString GetDefaultNewAssetName() const override;
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	//~ End UFactory Interface

	//~ Begin FReimportHandler Interface
	virtual bool CanReimport(UObject* Obj, TArray<FString>& OutFilenames) override;
	virtual void SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths) override;
	virtual EReimportResult::Type Reimport(UObject* Obj) override;
	virtual int32 GetPriority() const override;
	//~ End FReimportHandler Interface

	static TSharedPtr<FVoxelHeightmap> Import(const FString& Path);
};

class SVoxelHeightmapFactoryDetails : public SWindow
{
public:
	DECLARE_DELEGATE_RetVal(bool, FOnCreate);

	VOXEL_SLATE_ARGS()
	{
		SLATE_ARGUMENT(UObject*, DetailsObject)
		SLATE_ATTRIBUTE(bool, CanCreate)
		SLATE_EVENT(FOnCreate, OnCreate)
	};

	SVoxelHeightmapFactoryDetails() = default;

	void Construct(const FArguments& Args);
};