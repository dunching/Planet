// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelFutureValue.h"
#include "VoxelObjectPinType.h"
#include "VoxelObjectWithGuid.h"
#include "VoxelDetailTexture.generated.h"

class FVoxelQuery;
class FVoxelDependency;
class FVoxelDetailTexturePool;
class FVoxelDetailTextureManager;
class FVoxelDetailTextureAllocation;

UCLASS(Abstract, meta = (VoxelAssetType))
class VOXELGRAPHCORE_API UVoxelDetailTexture : public UVoxelObjectWithGuid
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Config", DisplayName = "LOD to Texture Size")
	TArray<int32> LODToTextureSize =
	{
		4,
		8
	};

	//~ Begin UObject Interface
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	//~ End UObject Interface

	virtual EPixelFormat GetPixelFormat() const VOXEL_PURE_VIRTUAL({});
	virtual int32 GetNumTextures() const { return 1; }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

UENUM()
enum class EVoxelFloatDetailTextureType : uint8
{
	Float32 UMETA(DisplayName = "32 bit float"),
	Float16 UMETA(DisplayName = "16 bit float (between 0 and 1)"),
	Float8 UMETA(DisplayName = "8 bit float (between 0 and 1)")
};

UCLASS()
class VOXELGRAPHCORE_API UVoxelFloatDetailTexture : public UVoxelDetailTexture
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Config");
	EVoxelFloatDetailTextureType Type = EVoxelFloatDetailTextureType::Float32;

	//~ Begin UVoxelDetailTexture Interface
	virtual EPixelFormat GetPixelFormat() const override
	{
		switch (Type)
		{
		default: ensure(false);
		case EVoxelFloatDetailTextureType::Float32: return PF_R32_FLOAT;
		case EVoxelFloatDetailTextureType::Float16: return PF_G16;
		case EVoxelFloatDetailTextureType::Float8: return PF_G8;
		}
	}
	//~ End UVoxelDetailTexture Interface
};

UCLASS()
class VOXELGRAPHCORE_API UVoxelColorDetailTexture : public UVoxelDetailTexture
{
	GENERATED_BODY()

public:
	//~ Begin UVoxelDetailTexture Interface
	virtual EPixelFormat GetPixelFormat() const override
	{
		return PF_R8G8B8A8;
	}
	//~ End UVoxelDetailTexture Interface
};

UCLASS()
class VOXELGRAPHCORE_API UVoxelMaterialIdDetailTexture : public UVoxelDetailTexture
{
	GENERATED_BODY()

public:
	//~ Begin UVoxelDetailTexture Interface
	virtual EPixelFormat GetPixelFormat() const override
	{
		return PF_R32_UINT;
	}
	virtual int32 GetNumTextures() const override
	{
		return 2;
	}
	//~ End UVoxelDetailTexture Interface
};

UCLASS()
class VOXELGRAPHCORE_API UVoxelNormalDetailTexture : public UVoxelDetailTexture
{
	GENERATED_BODY()

public:
	//~ Begin UVoxelDetailTexture Interface
	virtual EPixelFormat GetPixelFormat() const override
	{
		// R16 so we can use GatherRed
		return PF_R16_UINT;
	}
	//~ End UVoxelDetailTexture Interface
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(DisplayName = "Float Detail Texture")
struct FVoxelFloatDetailTextureRef
{
	GENERATED_BODY()

	TWeakPtr<FVoxelDetailTexturePool> WeakPool;
};

DECLARE_VOXEL_OBJECT_PIN_TYPE(FVoxelFloatDetailTextureRef);

USTRUCT()
struct FVoxelFloatDetailTextureRefPinType : public FVoxelObjectPinType
{
	GENERATED_BODY()

	DEFINE_VOXEL_OBJECT_PIN_TYPE(FVoxelFloatDetailTextureRef, UVoxelFloatDetailTexture);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(DisplayName = "Color Detail Texture")
struct FVoxelColorDetailTextureRef
{
	GENERATED_BODY()

	TWeakPtr<FVoxelDetailTexturePool> WeakPool;
};

DECLARE_VOXEL_OBJECT_PIN_TYPE(FVoxelColorDetailTextureRef);

USTRUCT()
struct FVoxelColorDetailTextureRefPinType : public FVoxelObjectPinType
{
	GENERATED_BODY()

	DEFINE_VOXEL_OBJECT_PIN_TYPE(FVoxelColorDetailTextureRef, UVoxelColorDetailTexture);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(DisplayName = "Material Id Detail Texture")
struct FVoxelMaterialIdDetailTextureRef
{
	GENERATED_BODY()

	TWeakPtr<FVoxelDetailTexturePool> WeakPool;
};

DECLARE_VOXEL_OBJECT_PIN_TYPE(FVoxelMaterialIdDetailTextureRef);

USTRUCT()
struct FVoxelMaterialIdDetailTextureRefPinType : public FVoxelObjectPinType
{
	GENERATED_BODY()

	DEFINE_VOXEL_OBJECT_PIN_TYPE(FVoxelMaterialIdDetailTextureRef, UVoxelMaterialIdDetailTexture);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(DisplayName = "Normal Detail Texture")
struct FVoxelNormalDetailTextureRef
{
	GENERATED_BODY()

	TWeakPtr<FVoxelDetailTexturePool> WeakPool;
};

DECLARE_VOXEL_OBJECT_PIN_TYPE(FVoxelNormalDetailTextureRef);

USTRUCT()
struct FVoxelNormalDetailTextureRefPinType : public FVoxelObjectPinType
{
	GENERATED_BODY()

	DEFINE_VOXEL_OBJECT_PIN_TYPE(FVoxelNormalDetailTextureRef, UVoxelNormalDetailTexture);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DECLARE_VOXEL_MEMORY_STAT(VOXELGRAPHCORE_API, STAT_VoxelDetailTextureMemory, "Voxel Detail Texture Memory (GPU)");

struct FVoxelDetailTextureAllocationRange
{
	int32 X : 16;
	int32 Y : 16;
	int32 Num;
};
checkStatic(sizeof(FVoxelDetailTextureAllocationRange) == 8);

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelDetailTextureDynamicMaterialParameter : public FVoxelDynamicMaterialParameter
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

	TArray<TWeakObjectPtr<UTexture2D>> Textures;
	FSimpleMulticastDelegate OnChangedMulticast;

	virtual void Apply(FName Name, UMaterialInstanceDynamic& Instance) const override;
	virtual void AddOnChanged(const FSimpleDelegate& OnChanged) override;
};

class VOXELGRAPHCORE_API FVoxelDetailTextureAllocator : public TSharedFromThis<FVoxelDetailTextureAllocator>
{
public:
	const int32 TextureSize;
	const EPixelFormat PixelFormat;
	const int32 NumTextures;
	const FName Name;

	FVoxelDetailTextureAllocator(
		int32 TextureSize,
		EPixelFormat PixelFormat,
		int32 NumTextures,
		const FVoxelDetailTexturePool& Pool);
	~FVoxelDetailTextureAllocator();

	void AddReferencedObjects(FReferenceCollector& Collector);

	TSharedRef<FVoxelDetailTextureAllocation> Allocate_AnyThread(int32 Num);
	void Update_GameThread();

private:
	TArray<UTexture2D*> Textures_GameThread;
	const TSharedRef<FVoxelDetailTextureDynamicMaterialParameter> DynamicParameter = MakeVoxelShared<FVoxelDetailTextureDynamicMaterialParameter>();

	VOXEL_ALLOCATED_SIZE_TRACKER_CUSTOM(STAT_VoxelDetailTextureMemory, DetailTextureMemory);

private:
	FVoxelFastCriticalSection CriticalSection;

	int32 SizeInBlocks_RequiresLock = 0;
	TVoxelArray<FVoxelDetailTextureAllocationRange> FreeRanges_RequiresLock;

	FORCEINLINE void CheckRange(const FVoxelDetailTextureAllocationRange& Range) const
	{
		checkVoxelSlow(CriticalSection.IsLocked());
		checkVoxelSlow(0 <= Range.X && Range.X < SizeInBlocks_RequiresLock);
		checkVoxelSlow(0 <= Range.Y && Range.Y < SizeInBlocks_RequiresLock);
		checkVoxelSlow(0 < Range.Num && Range.X + Range.Num <= SizeInBlocks_RequiresLock);
	}

	friend class FVoxelDetailTextureUpload;
	friend class FVoxelDetailTextureManager;
	friend class FVoxelDetailTextureAllocation;
};

class VOXELGRAPHCORE_API FVoxelDetailTextureAllocation
	: public FVirtualDestructor
	, public TSharedFromThis<FVoxelDetailTextureAllocation>
{
public:
	const int32 TextureSize;
	const FName Name;
	const EPixelFormat PixelFormat;
	const int32 NumTextures;
	const int32 Num;

	virtual ~FVoxelDetailTextureAllocation() override;

	TSharedRef<FVoxelDetailTextureDynamicMaterialParameter> GetTexture() const;

private:
	const TWeakPtr<FVoxelDetailTextureAllocator> WeakAllocator;
	TVoxelArray<FVoxelDetailTextureAllocationRange> Ranges;

	FVoxelDetailTextureAllocation(FVoxelDetailTextureAllocator& Allocator, int32 Num);

	friend class FVoxelDetailTextureUpload;
	friend class FVoxelDetailTextureManager;
	friend class FVoxelDetailTextureAllocator;
};

struct FVoxelDetailTextureCoordinate
{
	uint16 X = 0;
	uint16 Y = 0;
};
checkStatic(sizeof(FVoxelDetailTextureCoordinate) == sizeof(uint32));

class VOXELGRAPHCORE_API FVoxelDetailTextureUpload : public TSharedFromThis<FVoxelDetailTextureUpload>
{
public:
	FVoxelDetailTextureUpload(
		const FVoxelDetailTextureAllocation& Allocation,
		const int32 TextureIndex);

	bool GetUploadInfo(
		FVoxelDetailTextureCoordinate& OutCoordinate,
		TVoxelArrayView<uint8>& OutData,
		int32& OutPitch);

	void Upload(TFunction<void()> OnComplete);
	FVoxelDummyFutureValue Upload();

private:
	const TSharedRef<const FVoxelDetailTextureAllocation> Allocation;
	const int32 TextureIndex;
	const int32 TextureSize;
	const int32 BytesPerPixel;

	int32 UploadIndex = 0;
	int32 UploadRangeIndex = 0;
	int32 UploadIndexInRange = 0;
	TVoxelArray<uint8> UploadData;

	TFunction<void()> OnUploadComplete;

	void BeginUpload_GameThread();
	void Upload_RenderThread(const FTextureResource& Resource);

	friend class FVoxelDetailTextureManager;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class VOXELGRAPHCORE_API FVoxelDetailTexturePool : public TSharedFromThis<FVoxelDetailTexturePool>
{
public:
	const TWeakObjectPtr<UVoxelDetailTexture> WeakDetailTexture;
	const TSubclassOf<UVoxelDetailTexture> Class;
	const FName Name;
	const FName Guid;

	explicit FVoxelDetailTexturePool(UVoxelDetailTexture& DetailTexture);

	void AddReferencedObjects(FReferenceCollector& Collector);

	TSharedRef<FVoxelDetailTextureAllocation> Allocate_AnyThread(
		int32 TextureSize,
		int32 Num,
		const FVoxelQuery& Query);

	void UpdateTextureSize_GameThread();
	void UpdatePixelFormat_GameThread();
	int32 GetTextureSize_AnyThread(int32 LOD, const FVoxelQuery& Query);

private:
	const TSharedRef<FVoxelDependency> PixelFormatDependency;

	FVoxelFastCriticalSection CriticalSection;
	EPixelFormat PixelFormat_RequiresLock = {};
	int32 NumTextures_RequiresLock = 0;
	TVoxelArray<int32> LODToTextureSize_RequiresLock;
	TVoxelArray<TSharedPtr<FVoxelDependency>> LODToDependency_RequiresLock;
	TVoxelMap<int32, TSharedPtr<FVoxelDetailTextureAllocator>> TextureSizeToTextureAllocator_RequiresLock;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class VOXELGRAPHCORE_API FVoxelDetailTextureManager : public FVoxelSingleton
{
public:
	TSharedRef<FVoxelDetailTexturePool> FindOrAddPool_GameThread(UVoxelDetailTexture& Texture);
	void UpdatePool_GameThread(const UVoxelDetailTexture& Texture) const;

	//~ Begin FVoxelSingleton Interface
	virtual void Tick() override;
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	//~ End FVoxelSingleton Interface

private:
	TMap<UVoxelDetailTexture*, TSharedPtr<FVoxelDetailTexturePool>> TextureToPool;
	TQueue<TSharedPtr<FVoxelDetailTextureUpload>, EQueueMode::Mpsc> PendingUploads;
	TQueue<TWeakPtr<FVoxelDetailTextureAllocator>, EQueueMode::Mpsc> AllocatorsToUpdate;

	friend FVoxelDetailTextureUpload;
	friend FVoxelDetailTextureAllocator;
};

extern VOXELGRAPHCORE_API FVoxelDetailTextureManager* GVoxelDetailTextureManager;