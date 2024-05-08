// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelChannel.h"
#include "VoxelPinValue.h"
#include "VoxelSettings.h"
#include "VoxelObjectPinType.h"
#include "Misc/ConfigCacheIni.h"
#include "VoxelChannelAsset_DEPRECATED.generated.h"

USTRUCT()
struct FVoxelDistance_DEPRECATED
{
	GENERATED_BODY()
};

UCLASS()
class VOXELGRAPHCORE_API UVoxelChannelAsset_DEPRECATED : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Config")
	FVoxelPinType Type;

	UPROPERTY(EditAnywhere, Category = "Config")
	FVoxelPinValue DefaultValue;

	FVoxelChannelName MakeChannelName() const
	{
		const FName Name = "Project." + GetFName();
#if WITH_EDITOR
		if (!GVoxelChannelManager->FindChannelDefinition(Name))
		{
			UVoxelSettings* Settings = GetMutableDefault<UVoxelSettings>();
			Settings->GlobalChannels.Add(FVoxelChannelExposedDefinition
			{
				GetFName(),
				Type,
				DefaultValue
			});
			Settings->PostEditChange();
			Settings->TryUpdateDefaultConfigFile();

			// Force save config now
			GConfig->Flush(false, GEngineIni);
		}
#else
		ensure(false);
#endif
		return FVoxelChannelName{ Name };
	}
};

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelChannelRef_DEPRECATED
{
	GENERATED_BODY()

	TWeakObjectPtr<UVoxelChannelAsset_DEPRECATED> Channel;
};

DECLARE_VOXEL_OBJECT_PIN_TYPE(FVoxelChannelRef_DEPRECATED);

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelChannelRef_DEPRECATEDPinType : public FVoxelObjectPinType
{
	GENERATED_BODY()

	DEFINE_VOXEL_OBJECT_PIN_TYPE(FVoxelChannelRef_DEPRECATED, UVoxelChannelAsset_DEPRECATED)
	{
		if (bSetObject)
		{
			Object = Struct.Channel;
		}
		else
		{
			Struct.Channel = Object;
		}
	}
};