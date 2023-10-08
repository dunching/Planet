// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelEditorMinimal.h"
#include "VoxelAssetTypeActions.h"
#include "VoxelGraph.h"
#include "VoxelGraphInstance.h"

class FVoxelGraphAssetTypeActions : public FVoxelInstanceAssetTypeActions
{
public:
	FVoxelGraphAssetTypeActions() = default;

	//~ Begin FVoxelInstanceAssetTypeActions Interface
	virtual UClass* GetInstanceClass() const override
	{
		return UVoxelGraphInstance::StaticClass();
	}
	virtual FSlateIcon GetInstanceActionIcon() const override
	{
		return FSlateIcon(FAppStyle::GetAppStyleSetName(), "ClassIcon.BrushComponent");
	}
	virtual void SetParent(UObject* InstanceAsset, UObject* ParentAsset) const override
	{
		CastChecked<UVoxelGraphInstance>(InstanceAsset)->SetParentGraph(CastChecked<UVoxelGraphInterface>(ParentAsset));
	}
	//~ End FVoxelInstanceAssetTypeActions Interface
};

VOXEL_RUN_ON_STARTUP_EDITOR(RegisterGraphAssetTypeActions)
{
	FVoxelAssetTypeActions::Register(
		UVoxelGraph::StaticClass(),
		MakeVoxelShared<FVoxelGraphAssetTypeActions>());
	FVoxelAssetTypeActions::Register(
		UVoxelGraphInstance::StaticClass(),
		MakeVoxelShared<FVoxelGraphAssetTypeActions>());
}