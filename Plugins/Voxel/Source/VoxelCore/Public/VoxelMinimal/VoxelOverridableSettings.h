// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelCoreMinimal.h"
#include "VoxelOverridableSettings.generated.h"

USTRUCT()
struct VOXELCORE_API FVoxelOverridableSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Voxel")
	TSet<FName> OverridableParameters;

	template<typename T>
	void CopyOverridenParameters(const T& OverridenSettings)
	{
		const UStruct* Struct = T::StaticStruct();
		for (const FName OverridenParameter : OverridenSettings.OverridableParameters)
		{
			if (OverridenParameter == "")
			{
				continue;
			}

			const FProperty* TargetProperty = Struct->FindPropertyByName(OverridenParameter);
			if (!ensure(TargetProperty))
			{
				continue;
			}

			TargetProperty->CopyCompleteValue_InContainer(this, &OverridenSettings);
		}
	}
};