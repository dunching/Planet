// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelEditorMinimal.h"
#include "VoxelAssetTypeActions.h"
#include "Material/VoxelMaterialDefinition.h"
#include "Material/VoxelMaterialDefinitionInstance.h"

class FVoxelMaterialDefinitionAssetTypeActions : public FVoxelInstanceAssetTypeActions
{
public:
	FVoxelMaterialDefinitionAssetTypeActions() = default;

	//~ Begin FVoxelInstanceAssetTypeActions Interface
	virtual UClass* GetInstanceClass() const override
	{
		return UVoxelMaterialDefinitionInstance::StaticClass();
	}
	virtual FSlateIcon GetInstanceActionIcon() const override
	{
		return FSlateIcon(FAppStyle::GetAppStyleSetName(), "ClassIcon.BrushComponent");
	}
	virtual void SetParent(UObject* InstanceAsset, UObject* ParentAsset) const override
	{
		CastChecked<UVoxelMaterialDefinitionInstance>(InstanceAsset)->SetParentDefinition(CastChecked<UVoxelMaterialDefinitionInterface>(ParentAsset));
	}
	//~ End FVoxelInstanceAssetTypeActions Interface
};

VOXEL_RUN_ON_STARTUP_EDITOR(RegisterMaterialDefinitionAssetTypeActions)
{
	FVoxelAssetTypeActions::Register(
		UVoxelMaterialDefinition::StaticClass(),
		MakeVoxelShared<FVoxelMaterialDefinitionAssetTypeActions>());
	FVoxelAssetTypeActions::Register(
		UVoxelMaterialDefinitionInstance::StaticClass(),
		MakeVoxelShared<FVoxelMaterialDefinitionAssetTypeActions>());
}