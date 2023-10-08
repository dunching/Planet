// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelDetailTexture.h"
#include "VoxelQuery.h"
#include "VoxelDependency.h"
#include "TextureResource.h"
#include "Engine/Texture2D.h"
#include "Materials/MaterialInstanceDynamic.h"

DEFINE_VOXEL_FACTORY(UVoxelFloatDetailTexture);
DEFINE_VOXEL_FACTORY(UVoxelColorDetailTexture);
DEFINE_VOXEL_FACTORY(UVoxelMaterialIdDetailTexture);
DEFINE_VOXEL_FACTORY(UVoxelNormalDetailTexture);

DEFINE_VOXEL_MEMORY_STAT(STAT_VoxelDetailTextureMemory);

FVoxelDetailTextureManager* GVoxelDetailTextureManager = MakeVoxelSingleton(FVoxelDetailTextureManager);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#if WITH_EDITOR
void UVoxelDetailTexture::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (LODToTextureSize.Num() == 0)
	{
		LODToTextureSize.Add(4);
	}

	for (int32& TextureSize : LODToTextureSize)
	{
		TextureSize = FMath::Clamp(TextureSize, 4, 128);
	}

	if (PropertyChangedEvent.ChangeType == EPropertyChangeType::Interactive)
	{
		return;
	}

	GVoxelDetailTextureManager->UpdatePool_GameThread(*this);
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelFloatDetailTextureRefPinType::Convert(
	const bool bSetObject,
	TWeakObjectPtr<UVoxelFloatDetailTexture>& Object,
	FVoxelFloatDetailTextureRef& Struct) const
{
	if (bSetObject)
	{
		if (const TSharedPtr<FVoxelDetailTexturePool> Pool = Struct.WeakPool.Pin())
		{
			Object = ReinterpretCastRef<TWeakObjectPtr<UVoxelFloatDetailTexture>>(Pool->WeakDetailTexture);
		}
	}
	else
	{
		Struct.WeakPool = GVoxelDetailTextureManager->FindOrAddPool_GameThread(*Object);
	}
}

void FVoxelColorDetailTextureRefPinType::Convert(
	const bool bSetObject,
	TWeakObjectPtr<UVoxelColorDetailTexture>& Object,
	FVoxelColorDetailTextureRef& Struct) const
{
	if (bSetObject)
	{
		if (const TSharedPtr<FVoxelDetailTexturePool> Pool = Struct.WeakPool.Pin())
		{
			Object = ReinterpretCastRef<TWeakObjectPtr<UVoxelColorDetailTexture>>(Pool->WeakDetailTexture);
		}
	}
	else
	{
		Struct.WeakPool = GVoxelDetailTextureManager->FindOrAddPool_GameThread(*Object);
	}
}

void FVoxelMaterialIdDetailTextureRefPinType::Convert(
	const bool bSetObject,
	TWeakObjectPtr<UVoxelMaterialIdDetailTexture>& Object,
	FVoxelMaterialIdDetailTextureRef& Struct) const
{
	if (bSetObject)
	{
		if (const TSharedPtr<FVoxelDetailTexturePool> Pool = Struct.WeakPool.Pin())
		{
			Object = ReinterpretCastRef<TWeakObjectPtr<UVoxelMaterialIdDetailTexture>>(Pool->WeakDetailTexture);
		}
	}
	else
	{
		Struct.WeakPool = GVoxelDetailTextureManager->FindOrAddPool_GameThread(*Object);
	}
}

void FVoxelNormalDetailTextureRefPinType::Convert(
	const bool bSetObject,
	TWeakObjectPtr<UVoxelNormalDetailTexture>& Object,
	FVoxelNormalDetailTextureRef& Struct) const
{
	if (bSetObject)
	{
		if (const TSharedPtr<FVoxelDetailTexturePool> Pool = Struct.WeakPool.Pin())
		{
			Object = Cast<UVoxelNormalDetailTexture>(Pool->WeakDetailTexture);
		}
	}
	else
	{
		Struct.WeakPool = GVoxelDetailTextureManager->FindOrAddPool_GameThread(*Object);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelDetailTextureAllocator::FVoxelDetailTextureAllocator(
	const int32 TextureSize,
	const EPixelFormat PixelFormat,
	const int32 NumTextures,
	const FVoxelDetailTexturePool& Pool)
	: TextureSize(TextureSize)
	, PixelFormat(PixelFormat)
	, NumTextures(NumTextures)
	, Name(Pool.Name)
{
	Textures_GameThread.SetNumZeroed(NumTextures);
}

FVoxelDetailTextureAllocator::~FVoxelDetailTextureAllocator()
{
	ensure(Textures_GameThread.Num() == NumTextures);
}

void FVoxelDetailTextureAllocator::AddReferencedObjects(FReferenceCollector& Collector)
{
	ensure(Textures_GameThread.Num() == NumTextures);

	for (const UTexture2D* Texture : Textures_GameThread)
	{
		ensure(Texture || !GIsRunning);
	}

	Collector.AddReferencedObjects(Textures_GameThread);

	for (const UTexture2D* Texture : Textures_GameThread)
	{
		ensure(Texture || !GIsRunning);
	}
}

TSharedRef<FVoxelDetailTextureAllocation> FVoxelDetailTextureAllocator::Allocate_AnyThread(const int32 Num)
{
	VOXEL_FUNCTION_COUNTER();
	VOXEL_SCOPE_LOCK(CriticalSection);

	const TSharedRef<FVoxelDetailTextureAllocation> Allocation(new FVoxelDetailTextureAllocation(*this, Num));

	if (SizeInBlocks_RequiresLock == 0)
	{
		ensure(FreeRanges_RequiresLock.Num() == 0);

		SizeInBlocks_RequiresLock = 512;
		GVoxelDetailTextureManager->AllocatorsToUpdate.Enqueue(AsWeak());

		for (int32 Y = 0; Y < SizeInBlocks_RequiresLock; Y++)
		{
			FreeRanges_RequiresLock.Add(
			{
				0,
				Y,
				SizeInBlocks_RequiresLock
			});
		}
	}

	int32 NumLeft = Num;
	while (NumLeft > 0)
	{
		if (FreeRanges_RequiresLock.Num() == 0)
		{
			// Reallocate

			if (!ensure(2 * SizeInBlocks_RequiresLock * TextureSize <= 16384))
			{
				return Allocation;
			}

			for (int32 Y = 0; Y < SizeInBlocks_RequiresLock; Y++)
			{
				FreeRanges_RequiresLock.Add(
				{
					SizeInBlocks_RequiresLock,
					Y,
					SizeInBlocks_RequiresLock
				});
				FreeRanges_RequiresLock.Add(
				{
					0,
					SizeInBlocks_RequiresLock + Y,
					2 * SizeInBlocks_RequiresLock
				});
			}

			SizeInBlocks_RequiresLock *= 2;
			GVoxelDetailTextureManager->AllocatorsToUpdate.Enqueue(AsWeak());
		}

		const FVoxelDetailTextureAllocationRange Range = FreeRanges_RequiresLock.Pop(false);
		CheckRange(Range);

		const int32 NumInRange = FMath::Min(NumLeft, Range.Num);
		Allocation->Ranges.Add({ Range.X, Range.Y, NumInRange });

		NumLeft -= NumInRange;

		if (NumInRange < Range.Num)
		{
			const FVoxelDetailTextureAllocationRange NewRange
			{
				Range.X + NumInRange,
				Range.Y,
				Range.Num - NumInRange
			};
			CheckRange(NewRange);
			FreeRanges_RequiresLock.Add(NewRange);
		}
	}
	ensure(NumLeft == 0);

	return Allocation;
}

void FVoxelDetailTextureAllocator::Update_GameThread()
{
	VOXEL_FUNCTION_COUNTER();
	check(IsInGameThread());
	VOXEL_SCOPE_LOCK(CriticalSection);

	const int32 Size = SizeInBlocks_RequiresLock * TextureSize;
	{
		bool bNeedUpdate = false;
		for (const UTexture2D* Texture : Textures_GameThread)
		{
			if (!Texture ||
				Texture->GetSizeX() != Size ||
				Texture->GetSizeY() != Size)
			{
				bNeedUpdate = true;
			}
		}
		if (!bNeedUpdate)
		{
			return;
		}
	}

	const TArray<UTexture2D*> OldTextures = Textures_GameThread;

	const FName DebugName = "VoxelDetailTexture_" + Name + "_TextureSize_" + FString::FromInt(TextureSize);
	LOG_VOXEL(Log, "Reallocating detail texture %s to %dx%d", *DebugName.ToString(), Size, Size);

	for (int32 Index = 0; Index < NumTextures; Index++)
	{
		UTexture2D* Texture = FVoxelTextureUtilities::CreateTexture2D(
			DebugName + FString::Printf(TEXT("_TextureIndex_%d_"), Index),
			Size,
			Size,
			false,
			TF_Bilinear,
			PixelFormat);

		FVoxelTextureUtilities::RemoveBulkData(Texture);

		Textures_GameThread[Index] = Texture;
	}

	const FPixelFormatInfo& Format = GPixelFormats[PixelFormat];
	DetailTextureMemory = Size * Size * Format.BlockBytes * NumTextures / (Format.BlockSizeX * Format.BlockSizeY);

	DynamicParameter->Textures = TArray<TWeakObjectPtr<UTexture2D>>(Textures_GameThread);
	DynamicParameter->OnChangedMulticast.Broadcast();

	for (int32 Index = 0; Index < NumTextures; Index++)
	{
		UTexture2D* OldTexture = OldTextures[Index];
		UTexture2D* NewTexture = Textures_GameThread[Index];
		if (!OldTexture ||
			!ensure(NewTexture))
		{
			continue;
		}

		FTextureResource* OldResource = OldTexture->GetResource();
		FTextureResource* NewResource = NewTexture->GetResource();

		FRHICopyTextureInfo CopyInfo;
		CopyInfo.Size = { OldTexture->GetSizeX(), OldTexture->GetSizeY(), 1 };

		VOXEL_ENQUEUE_RENDER_COMMAND(FVoxelDetailTextureAllocator_Reallocate)([OldResource, NewResource, CopyInfo](FRHICommandListImmediate& RHICmdList)
		{
			RHICmdList.CopyTexture(
				OldResource->GetTextureRHI(),
				NewResource->GetTextureRHI(),
				CopyInfo);
		});
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelDetailTextureDynamicMaterialParameter::Apply(const FName Name, UMaterialInstanceDynamic& Instance) const
{
	if (Textures.Num() == 1)
	{
		UTexture2D* Texture = Textures[0].Get();

		Instance.SetTextureParameterValue(Name + "_Texture", Texture);
		Instance.SetScalarParameterValue(Name + "_Texture_Size", Texture ? Texture->GetSizeX() : 1);
		Instance.SetScalarParameterValue(Name + "_Texture_InvSize", Texture ? 1. / double(Texture->GetSizeX()) : 1);
	}
	else if (Textures.Num() == 2)
	{
		UTexture2D* Texture0 = Textures[0].Get();
		UTexture2D* Texture1 = Textures[1].Get();
		ensure((Texture0 != nullptr) == (Texture1 != nullptr));

		if (Texture0 &&
			Texture1)
		{
			ensure(Texture0->GetSizeX() == Texture1->GetSizeX());
		}

		Instance.SetTextureParameterValue(Name + "_TextureA", Texture0);
		Instance.SetTextureParameterValue(Name + "_TextureB", Texture1);

		Instance.SetScalarParameterValue(Name + "_Texture_Size", Texture0 ? Texture0->GetSizeX() : 1);
		Instance.SetScalarParameterValue(Name + "_Texture_InvSize", Texture0 ? 1. / double(Texture0->GetSizeX()) : 1);
	}
	else
	{
		ensure(false);
	}
}

void FVoxelDetailTextureDynamicMaterialParameter::AddOnChanged(const FSimpleDelegate& OnChanged)
{
	OnChangedMulticast.Add(OnChanged);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelDetailTextureAllocation::~FVoxelDetailTextureAllocation()
{
	VOXEL_FUNCTION_COUNTER();

	const TSharedPtr<FVoxelDetailTextureAllocator> Allocator = WeakAllocator.Pin();
	if (!Allocator)
	{
		return;
	}

	VOXEL_SCOPE_LOCK(Allocator->CriticalSection);

	for (const FVoxelDetailTextureAllocationRange& Range : Ranges)
	{
		Allocator->CheckRange(Range);
	}

	Allocator->FreeRanges_RequiresLock.Append(Ranges);
}

TSharedRef<FVoxelDetailTextureDynamicMaterialParameter> FVoxelDetailTextureAllocation::GetTexture() const
{
	const TSharedPtr<FVoxelDetailTextureAllocator> Allocator = WeakAllocator.Pin();
	if (!ensure(Allocator))
	{
		return MakeVoxelShared<FVoxelDetailTextureDynamicMaterialParameter>();
	}

	return Allocator->DynamicParameter;
}

FVoxelDetailTextureAllocation::FVoxelDetailTextureAllocation(
	FVoxelDetailTextureAllocator& Allocator,
	const int32 Num)
	: TextureSize(Allocator.TextureSize)
	, Name(Allocator.Name)
	, PixelFormat(Allocator.PixelFormat)
	, NumTextures(Allocator.NumTextures)
	, Num(Num)
	, WeakAllocator(Allocator.AsWeak())
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelDetailTextureUpload::FVoxelDetailTextureUpload(const FVoxelDetailTextureAllocation& Allocation, const int32 TextureIndex)
	: Allocation(Allocation.AsShared())
	, TextureIndex(TextureIndex)
	, TextureSize(Allocation.TextureSize)
	, BytesPerPixel(GPixelFormats[Allocation.PixelFormat].BlockBytes)
{
	VOXEL_FUNCTION_COUNTER();

	// Needed to not crash in FD3D11DynamicRHI::RHIUpdateTexture2D
	const int32 SourcePitch = Allocation.Num * TextureSize * BytesPerPixel;
	const FPixelFormatInfo& FormatInfo = GPixelFormats[Allocation.PixelFormat];
	const int32 MaxUpdateHeightInTiles = FMath::DivideAndRoundUp(TextureSize, FormatInfo.BlockSizeY);
	const int32 Padding = SourcePitch * MaxUpdateHeightInTiles;

	FVoxelUtilities::SetNumFast(
		UploadData,
		Allocation.Num * FMath::Square(Allocation.TextureSize) * BytesPerPixel + Padding);
}

bool FVoxelDetailTextureUpload::GetUploadInfo(
	FVoxelDetailTextureCoordinate& OutCoordinate,
	TVoxelArrayView<uint8>& OutData,
	int32& OutPitch)
{
	if (UploadIndex == Allocation->Num)
	{
		checkVoxelSlow(UploadIndexInRange == 0);
		return false;
	}

	if (!ensure(Allocation->Ranges.IsValidIndex(UploadRangeIndex)))
	{
		return false;
	}

	{
		const FVoxelDetailTextureAllocationRange& Range = Allocation->Ranges[UploadRangeIndex];

		OutCoordinate = FVoxelDetailTextureCoordinate
		{
			uint16(Range.X + UploadIndexInRange),
			uint16(Range.Y)
		};
	}

	{
		const int32 Index = UploadIndex * TextureSize * BytesPerPixel;
		OutData = MakeVoxelArrayView(UploadData).RightChop(Index);
	}

	OutPitch = Allocation->Num * TextureSize * BytesPerPixel;

	UploadIndexInRange++;
	UploadIndex++;

	if (UploadIndexInRange == Allocation->Ranges[UploadRangeIndex].Num)
	{
		UploadRangeIndex++;
		UploadIndexInRange = 0;
	}

	return true;
}

void FVoxelDetailTextureUpload::Upload(TFunction<void()> OnComplete)
{
	check(OnComplete);
	check(!OnUploadComplete);
	OnUploadComplete = MoveTemp(OnComplete);

	GVoxelDetailTextureManager->PendingUploads.Enqueue(AsShared());
}

FVoxelDummyFutureValue FVoxelDetailTextureUpload::Upload()
{
	const FVoxelDummyFutureValue Dummy = FVoxelFutureValue::MakeDummy();
	Upload([=]
	{
		Dummy.MarkDummyAsCompleted();
	});
	return Dummy;
}

void FVoxelDetailTextureUpload::BeginUpload_GameThread()
{
	VOXEL_FUNCTION_COUNTER();
	check(IsInGameThread());

	const TSharedPtr<FVoxelDetailTextureAllocator> Allocator = Allocation->WeakAllocator.Pin();
	if (!Allocator ||
		!ensure(Allocator->Textures_GameThread.IsValidIndex(TextureIndex)) ||
		!ensure(Allocator->Textures_GameThread[TextureIndex]))
	{
		return;
	}

	const FTextureResource* Resource = Allocator->Textures_GameThread[TextureIndex]->GetResource();
	if (!ensure(Resource))
	{
		return;
	}

	VOXEL_ENQUEUE_RENDER_COMMAND(FVoxelDetailTextureUpload_Upload)([This = AsShared(), Resource](FRHICommandListImmediate& RHICmdList)
	{
		This->Upload_RenderThread(*Resource);
	});
}

void FVoxelDetailTextureUpload::Upload_RenderThread(const FTextureResource& Resource)
{
	VOXEL_FUNCTION_COUNTER();
	check(IsInRenderingThread());

	FRHITexture* TextureRHI = Resource.GetTexture2DRHI();
	if (!ensure(TextureRHI))
	{
		return;
	}

	int32 Index = 0;
	for (const FVoxelDetailTextureAllocationRange& Range : Allocation->Ranges)
	{
		const FUpdateTextureRegion2D UpdateRegion(
			TextureSize * Range.X,
			TextureSize * Range.Y,
			// DX12 RHI is bugged, Src is not used
			0,
			0,
			TextureSize * Range.Num,
			TextureSize);

		VOXEL_SCOPE_COUNTER_FORMAT("RHIUpdateTexture2D %s Num=%d", *Allocation->Name.ToString(), TextureSize * TextureSize * Range.Num);
		RHIUpdateTexture2D_Safe(
			TextureRHI,
			0,
			UpdateRegion,
			Allocation->Num * TextureSize * BytesPerPixel,
			MakeVoxelArrayView(UploadData).RightChop(TextureSize * Index * BytesPerPixel));

		Index += Range.Num;
	}
	ensure(Index == Allocation->Num);

	OnUploadComplete();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelDetailTexturePool::FVoxelDetailTexturePool(UVoxelDetailTexture& DetailTexture)
	: WeakDetailTexture(&DetailTexture)
	, Class(DetailTexture.GetClass())
	, Name(DetailTexture.GetFName())
	, Guid(*DetailTexture.GetGuid().ToString())
	, PixelFormatDependency(FVoxelDependency::Create(
		STATIC_FNAME("DetailTexture PixelFormat"),
		DetailTexture.GetFName()))
{
}

void FVoxelDetailTexturePool::AddReferencedObjects(FReferenceCollector& Collector)
{
	VOXEL_FUNCTION_COUNTER();
	VOXEL_SCOPE_LOCK(CriticalSection);

	for (const auto& It : TextureSizeToTextureAllocator_RequiresLock)
	{
		It.Value->AddReferencedObjects(Collector);
	}
}

TSharedRef<FVoxelDetailTextureAllocation> FVoxelDetailTexturePool::Allocate_AnyThread(
	const int32 TextureSize,
	const int32 Num,
	const FVoxelQuery& Query)
{
	VOXEL_FUNCTION_COUNTER();

	Query.GetDependencyTracker().AddDependency(PixelFormatDependency);

	TSharedPtr<FVoxelDetailTextureAllocator> TextureAllocator;
	{
		VOXEL_SCOPE_LOCK(CriticalSection);

		TextureAllocator = TextureSizeToTextureAllocator_RequiresLock.FindRef(TextureSize);

		if (!TextureAllocator)
		{
			TextureAllocator = TextureSizeToTextureAllocator_RequiresLock.Add_CheckNew(
				TextureSize,
				MakeVoxelShared<FVoxelDetailTextureAllocator>(
					TextureSize,
					PixelFormat_RequiresLock,
					NumTextures_RequiresLock,
					*this));
		}
	}

	return TextureAllocator->Allocate_AnyThread(Num);
}

void FVoxelDetailTexturePool::UpdateTextureSize_GameThread()
{
	VOXEL_FUNCTION_COUNTER();

	FVoxelDependencyInvalidationScope InvalidationScope;

	VOXEL_SCOPE_LOCK(CriticalSection);

	const UVoxelDetailTexture* DetailTexture = WeakDetailTexture.Get();
	if (!ensure(DetailTexture))
	{
		return;
	}

	const TVoxelArray<int32> PreviousLODToTextureSize = LODToTextureSize_RequiresLock;

	LODToTextureSize_RequiresLock.Reset(32);
	LODToTextureSize_RequiresLock.Append(DetailTexture->LODToTextureSize);

	while (LODToTextureSize_RequiresLock.Num() < 32)
	{
		LODToTextureSize_RequiresLock.Add(MakeCopy(LODToTextureSize_RequiresLock.Last()));
	}

	for (int32& TextureSize : LODToTextureSize_RequiresLock)
	{
		TextureSize = FMath::Clamp(TextureSize, 4, 128);
	}

	for (int32 LOD = 0; LOD < LODToDependency_RequiresLock.Num(); LOD++)
	{
		const TSharedPtr<FVoxelDependency> Dependency = LODToDependency_RequiresLock[LOD];
		if (!Dependency)
		{
			continue;
		}

		if (PreviousLODToTextureSize.IsValidIndex(LOD) !=
			LODToTextureSize_RequiresLock.IsValidIndex(LOD))
		{
			Dependency->Invalidate();
			continue;
		}

		if (!PreviousLODToTextureSize.IsValidIndex(LOD) ||
			!LODToTextureSize_RequiresLock.IsValidIndex(LOD) ||
			PreviousLODToTextureSize[LOD] == LODToTextureSize_RequiresLock[LOD])
		{
			continue;
		}

		Dependency->Invalidate();
	}
}

void FVoxelDetailTexturePool::UpdatePixelFormat_GameThread()
{
	VOXEL_FUNCTION_COUNTER();

	FVoxelDependencyInvalidationScope InvalidationScope;

	VOXEL_SCOPE_LOCK(CriticalSection);

	const UVoxelDetailTexture* DetailTexture = WeakDetailTexture.Get();
	if (!ensure(DetailTexture))
	{
		return;
	}
	if (PixelFormat_RequiresLock == DetailTexture->GetPixelFormat() &&
		NumTextures_RequiresLock == DetailTexture->GetNumTextures())
	{
		return;
	}

	PixelFormat_RequiresLock = DetailTexture->GetPixelFormat();
	NumTextures_RequiresLock = DetailTexture->GetNumTextures();
	TextureSizeToTextureAllocator_RequiresLock.Reset();
	PixelFormatDependency->Invalidate();
}

int32 FVoxelDetailTexturePool::GetTextureSize_AnyThread(const int32 LOD, const FVoxelQuery& Query)
{
	TSharedPtr<FVoxelDependency> Dependency;
	const int32 TextureSize = INLINE_LAMBDA
	{
		VOXEL_SCOPE_LOCK(CriticalSection);

		if (!LODToDependency_RequiresLock.IsValidIndex(LOD))
		{
			LODToDependency_RequiresLock.SetNum(LOD + 1);
		}

		TSharedPtr<FVoxelDependency>& DependencyRef = LODToDependency_RequiresLock[LOD];
		if (!DependencyRef)
		{
			DependencyRef = FVoxelDependency::Create(
				STATIC_FNAME("DetailTexture TextureSize"),
				*FString::Printf(TEXT("%s LOD=%d"), *Name.ToString(), LOD));
		}
		Dependency = DependencyRef;

		if (!ensure(LODToTextureSize_RequiresLock.IsValidIndex(LOD)))
		{
			return 4;
		}
		return LODToTextureSize_RequiresLock[LOD];
	};

	Query.GetDependencyTracker().AddDependency(Dependency.ToSharedRef());
	return TextureSize;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedRef<FVoxelDetailTexturePool> FVoxelDetailTextureManager::FindOrAddPool_GameThread(UVoxelDetailTexture& Texture)
{
	VOXEL_FUNCTION_COUNTER();
	check(IsInGameThread());

	TSharedPtr<FVoxelDetailTexturePool>& Pool = TextureToPool.FindOrAdd(&Texture);
	if (Pool)
	{
		return Pool.ToSharedRef();
	}

	Pool = MakeVoxelShared<FVoxelDetailTexturePool>(Texture);
	Pool->UpdatePixelFormat_GameThread();
	Pool->UpdateTextureSize_GameThread();
	return Pool.ToSharedRef();
}

void FVoxelDetailTextureManager::UpdatePool_GameThread(const UVoxelDetailTexture& Texture) const
{
	VOXEL_FUNCTION_COUNTER();
	check(IsInGameThread());

	const TSharedPtr<FVoxelDetailTexturePool> Pool = TextureToPool.FindRef(&Texture);
	if (!Pool)
	{
		return;
	}

	Pool->UpdatePixelFormat_GameThread();
	Pool->UpdateTextureSize_GameThread();
}

void FVoxelDetailTextureManager::Tick()
{
	VOXEL_FUNCTION_COUNTER();
	check(IsInGameThread());

	// Allocate all the textures first
	TWeakPtr<FVoxelDetailTextureAllocator> WeakAllocator;
	while (AllocatorsToUpdate.Dequeue(WeakAllocator))
	{
		if (const TSharedPtr<FVoxelDetailTextureAllocator> Allocator = WeakAllocator.Pin())
		{
			Allocator->Update_GameThread();
		}
	}

	// Then upload data
	TSharedPtr<FVoxelDetailTextureUpload> Upload;
	while (PendingUploads.Dequeue(Upload))
	{
		Upload->BeginUpload_GameThread();
	}
}

void FVoxelDetailTextureManager::AddReferencedObjects(FReferenceCollector& Collector)
{
	VOXEL_FUNCTION_COUNTER();

	for (auto It = TextureToPool.CreateIterator(); It; ++It)
	{
		Collector.AddReferencedObject(It.Key());

		if (!ensure(It.Key()))
		{
			It.RemoveCurrent();
			continue;
		}

		It.Value()->AddReferencedObjects(Collector);
	}
}