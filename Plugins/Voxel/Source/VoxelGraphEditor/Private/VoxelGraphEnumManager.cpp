#include "VoxelGraphNodeBase.h"
#include "VoxelGraph.h"
#include "Kismet2/EnumEditorUtils.h"
#include "NodeDependingOnEnumInterface.h"

class FVoxelGraphEnumManager : public FEnumEditorUtils::INotifyOnEnumChanged
{
	virtual void PreChange(const UUserDefinedEnum* Changed, FEnumEditorUtils::EEnumEditorChangeInfo ChangedType) override
	{
	}

	virtual void PostChange(const UUserDefinedEnum* Enum, FEnumEditorUtils::EEnumEditorChangeInfo ChangedType) override
	{
		ForEachObjectOfClass<UVoxelGraph>([&](UVoxelGraph* Graph)
		{
			bool bFixup = false;
			for (const FVoxelGraphParameter& Parameter : Graph->Parameters)
			{
				if (!Parameter.Type.Is<uint8>() ||
					Parameter.Type.GetEnum() != Enum)
				{
					continue;
				}

				bFixup = true;
				break;
			}

			if (bFixup)
			{
				Graph->FixupParameters();
			}
		}, true, RF_ClassDefaultObject | RF_Transient);

		ForEachObjectOfClass<UVoxelGraphNodeBase>([&](UVoxelGraphNodeBase* Node)
		{
			const INodeDependingOnEnumInterface* NodeDependingOnEnum = Cast<INodeDependingOnEnumInterface>(Node);

			if (!Node->HasAnyFlags(RF_ClassDefaultObject | RF_Transient) &&
				NodeDependingOnEnum &&
				Enum == NodeDependingOnEnum->GetEnum() &&
				NodeDependingOnEnum->ShouldBeReconstructedAfterEnumChanged())
			{
				Node->ReconstructNode();
			}
		}, true, RF_ClassDefaultObject | RF_Transient);
	}
};

VOXEL_RUN_ON_STARTUP_EDITOR(RegisterEnumManager)
{
	new FVoxelGraphEnumManager();
}