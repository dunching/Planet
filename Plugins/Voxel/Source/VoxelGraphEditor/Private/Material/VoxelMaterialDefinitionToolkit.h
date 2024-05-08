// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "Material/VoxelMaterialDefinition.h"
#include "Toolkits/VoxelSimpleAssetToolkit.h"
#include "VoxelMaterialDefinitionToolkit.generated.h"

class SVoxelMaterialDefinitionParameters;

USTRUCT()
struct FVoxelMaterialDefinitionToolkit : public FVoxelSimpleAssetToolkit
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

	UPROPERTY()
	TObjectPtr<UVoxelMaterialDefinitionInterface> Asset;

public:
	//~ Begin FVoxelSimpleAssetToolkit Interface
	virtual void Initialize() override;
	virtual TSharedPtr<FTabManager::FLayout> GetLayout() const override;
	virtual void RegisterTabs(FRegisterTab RegisterTab) override;
	virtual void PostEditChange(const FPropertyChangedEvent& PropertyChangedEvent) override;

	virtual bool ShowFloor() const override { return false; }
	virtual void SetupPreview() override;
	virtual void UpdatePreview() override;
	virtual FRotator GetInitialViewRotation() const override;
	virtual TOptional<float> GetInitialViewDistance() const override { return 500; }
	//~ End FVoxelSimpleAssetToolkit Interface

	void SelectParameter(const FGuid& Guid, bool bRequestRename, bool bRefresh);

private:
	static constexpr const TCHAR* ParametersTabId = TEXT("FVoxelMaterialLayerTemplateToolkit_Parameters");

	FGuid SelectedGuid;
	TSharedPtr<SVoxelMaterialDefinitionParameters> MaterialLayerParameters;
	TWeakObjectPtr<UStaticMeshComponent> StaticMeshComponent;
};