// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelObjectPinType.h"

UObject* FVoxelObjectPinType::GetObject(const FConstVoxelStructView Struct) const
{
	const TWeakObjectPtr<UObject> WeakObject = GetWeakObject(Struct);
	ensure(WeakObject.IsValid() || WeakObject.IsExplicitlyNull());
	return WeakObject.Get();
}

const TVoxelMap<const UScriptStruct*, const FVoxelObjectPinType*>& FVoxelObjectPinType::StructToPinType()
{
	static TVoxelMap<const UScriptStruct*, const FVoxelObjectPinType*> Map;
	if (Map.Num() == 0)
	{
		VOXEL_FUNCTION_COUNTER();

		for (UScriptStruct* StructIt : GetDerivedStructs<FVoxelObjectPinType>())
		{
			TVoxelInstancedStruct<FVoxelObjectPinType> Instance(StructIt);
			const UScriptStruct* InstanceStruct = Instance->GetStruct();

			ensure(!Map.Contains(InstanceStruct));
			Map.Add(InstanceStruct, Instance.Release());
		}

		GOnVoxelModuleUnloaded_DoCleanup.AddLambda([]
		{
			for (const auto& It : Map)
			{
				FVoxelMemory::Delete(It.Value);
			}
			Map.Empty();
		});
	}
	return Map;
}