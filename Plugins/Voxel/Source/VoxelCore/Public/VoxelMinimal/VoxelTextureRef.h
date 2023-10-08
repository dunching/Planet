// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelCoreMinimal.h"
#include "VoxelMinimal/Utilities/VoxelBaseUtilities.h"
#include "VoxelMinimal/Utilities/VoxelThreadingUtilities.h"

class UTexture;

enum class EVoxelTextureType
{
	Texture2D,
	GpuTexture2D
};

struct VOXELCORE_API FVoxelTextureKey
{
	const EVoxelTextureType Type;
	const int32 SizeX;
	const int32 SizeY;
	const EPixelFormat Format;

	FVoxelTextureKey(
		const EVoxelTextureType Type,
		const int32 SizeX,
		const int32 SizeY,
		const EPixelFormat Format)
		: Type(Type)
		, SizeX(SizeX)
		, SizeY(SizeY)
		, Format(Format)
	{
	}

	int64 GetSizeInBytes() const
	{
		return
			SizeX *
			SizeY *
			GPixelFormats[Format].BlockBytes;
	}

	bool operator==(const FVoxelTextureKey& Other) const
	{
		return
			Type == Other.Type &&
			SizeX == Other.SizeX &&
			SizeY == Other.SizeY &&
			Format == Other.Format;
	}
	friend uint32 GetTypeHash(const FVoxelTextureKey& Key)
	{
		return FVoxelUtilities::MurmurHashMulti(
			Key.Type,
			Key.SizeX,
			Key.SizeY,
			Key.Format);
	}
};

DECLARE_VOXEL_COUNTER(VOXELCORE_API, STAT_VoxelNumTexturesUsed, "Num Textures Used");
DECLARE_VOXEL_COUNTER(VOXELCORE_API, STAT_VoxelNumTexturesPooled, "Num Textures Pooled");

struct FVoxelTextureRefImpl;

class VOXELCORE_API FVoxelTextureRef : public FVirtualDestructor
{
public:
	static TSharedRef<FVoxelTextureRef> Make(FName DebugName, const FVoxelTextureKey& Key);
	virtual ~FVoxelTextureRef() override;

	template<typename T>
	T* Get() const
	{
		UTexture* Texture = Get();
		ensure(Texture || Texture->IsA<T>());
		return Cast<T>(Texture);
	}
	UTexture* Get() const;

private:
	const FName DynamicStatName;
	const FVoxelTextureKey Key;
	const TSharedRef<FVoxelTextureRefImpl> Impl;

	FVoxelTextureRef(
		FName DebugName,
		const FVoxelTextureKey& Key,
		const TSharedRef<FVoxelTextureRefImpl>& Impl);

	friend class FVoxelTexturePool;
	friend FVoxelUtilities::TGameThreadDeleter<FVoxelTextureRef>;
};