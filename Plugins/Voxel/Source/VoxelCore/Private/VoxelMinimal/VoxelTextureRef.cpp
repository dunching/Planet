// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelMinimal.h"
#include "VoxelGpuTexture.h"
#include "Engine/Texture2D.h"

VOXEL_CONSOLE_VARIABLE(
	VOXELCORE_API, float, GVoxelTexturePoolDuration, 30,
	"voxel.TexturePoolDuration",
	"");

DEFINE_VOXEL_COUNTER(STAT_VoxelNumTexturesUsed);
DEFINE_VOXEL_COUNTER(STAT_VoxelNumTexturesPooled);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

struct FVoxelTextureRefImpl
{
	UTexture* Texture = nullptr;
};

class FVoxelTexturePool : public FVoxelSingleton
{
public:
	struct FPooledTexture
	{
		TSharedRef<FVoxelTextureRefImpl> Ref;
		double LastUsedTime = 0;
	};
	TMap<FVoxelTextureKey, TArray<FPooledTexture>> KeyToTextures;
	TSet<TSharedPtr<FVoxelTextureRefImpl>> Refs;

	VOXEL_COUNTER_HELPER(STAT_VoxelNumTexturesPooled, NumTexturesPooledStat);

	//~ Begin FVoxelSingleton Interface
	virtual void Tick() override
	{
		VOXEL_FUNCTION_COUNTER();

		int32 NumTexturesPooled = 0;

		const double Time = FPlatformTime::Seconds();
		for (auto It = KeyToTextures.CreateIterator(); It; ++It)
		{
			TArray<FPooledTexture>& Textures = It.Value();
			for (int32 Index = 0; Index < Textures.Num(); Index++)
			{
				const FPooledTexture& Texture = Textures[Index];
				ensure(Texture.Ref->Texture);

				if (Time < Texture.LastUsedTime + GVoxelTexturePoolDuration)
				{
					NumTexturesPooled++;
					continue;
				}

				ensure(Refs.Remove(Texture.Ref));

				Textures.RemoveAtSwap(Index);
				Index--;
			}

			if (Textures.Num() == 0)
			{
				It.RemoveCurrent();
			}
		}

		NumTexturesPooledStat = NumTexturesPooled;
	}
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override
	{
		VOXEL_FUNCTION_COUNTER();

		for (const TSharedPtr<FVoxelTextureRefImpl>& Ref : Refs)
		{
			ensure(Ref->Texture);
			Collector.AddReferencedObject(Ref->Texture);
			ensure(Ref->Texture);
		}
	}
	//~ End FVoxelSingleton Interface

	TSharedRef<FVoxelTextureRef> MakeTexture(FName DebugName, const FVoxelTextureKey& Key)
	{
		VOXEL_FUNCTION_COUNTER();
		check(IsInGameThread());

		const TSharedRef<FVoxelTextureRef> TexturePoolRef = INLINE_LAMBDA
		{
			if (TArray<FPooledTexture>* Textures = KeyToTextures.Find(Key))
			{
				while (Textures->Num() > 0)
				{
					const FPooledTexture Texture = Textures->Pop(false);
					if (!ensure(Texture.Ref->Texture))
					{
						continue;
					}

					return MakeShared_GameThread<FVoxelTextureRef>(DebugName, Key, Texture.Ref);
				}
			}

			return MakeShared_GameThread<FVoxelTextureRef>(DebugName, Key, AllocateTexture(Key));
		};

		return TexturePoolRef;
	}

	TSharedRef<FVoxelTextureRefImpl> AllocateTexture(FVoxelTextureKey Key)
	{
		VOXEL_FUNCTION_COUNTER();
		check(IsInGameThread());

		FString DebugName = "VoxelPooledTexture_";
		DebugName += FString::FromInt(Key.SizeX);
		DebugName += "_";
		DebugName += FString::FromInt(Key.SizeY);
		DebugName += "_";
		DebugName += GPixelFormats[Key.Format].Name;

		const TSharedRef<FVoxelTextureRefImpl> Ref = MakeVoxelShared<FVoxelTextureRefImpl>();
		Refs.Add(Ref);

		if (Key.Type == EVoxelTextureType::Texture2D)
		{
			Ref->Texture = FVoxelTextureUtilities::CreateTexture2D(
				*DebugName,
				Key.SizeX,
				Key.SizeY,
				false,
				TF_Bilinear,
				Key.Format);
		}
		else
		{
			check(Key.Type == EVoxelTextureType::GpuTexture2D);

			const FName ObjectName = MakeUniqueObjectName(GetTransientPackage(), UVoxelGpuTexture::StaticClass(), *FString::Printf(TEXT("VoxelGpuTexture2D_%s"), *DebugName));

			UVoxelGpuTexture* Texture = NewObject<UVoxelGpuTexture>(GetTransientPackage(), ObjectName);
			Texture->UpdateResource();
			Ref->Texture = Texture;
		}

		return Ref;
	}
};
FVoxelTexturePool* GVoxelTexturePool = MakeVoxelSingleton(FVoxelTexturePool);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedRef<FVoxelTextureRef> FVoxelTextureRef::Make(FName DebugName, const FVoxelTextureKey& Key)
{
	return GVoxelTexturePool->MakeTexture(DebugName, Key);
}

FVoxelTextureRef::FVoxelTextureRef(
	FName DebugName,
	const FVoxelTextureKey& Key,
	const TSharedRef<FVoxelTextureRefImpl>& Impl)
	: DynamicStatName(DebugName)
	, Key(Key)
	, Impl(Impl)
{
	INC_VOXEL_COUNTER(STAT_VoxelNumTexturesUsed);
	Voxel_AddAmountToDynamicStat(DynamicStatName, Key.GetSizeInBytes());
}

FVoxelTextureRef::~FVoxelTextureRef()
{
	DEC_VOXEL_COUNTER(STAT_VoxelNumTexturesUsed);
	Voxel_AddAmountToDynamicStat(DynamicStatName, -Key.GetSizeInBytes());

	const auto ReturnToPool = [Key = Key, Impl = Impl]
	{
		FVoxelUtilities::RunOnGameThread([=]
		{
			const FVoxelTexturePool::FPooledTexture PooledTexture
			{
				Impl,
				FPlatformTime::Seconds()
			};

			GVoxelTexturePool->KeyToTextures.FindOrAdd(Key).Add(PooledTexture);
		});
	};

	if (Key.Type == EVoxelTextureType::GpuTexture2D)
	{
		VOXEL_ENQUEUE_RENDER_COMMAND(ReturnToPool)([Impl = Impl, ReturnToPool](FRHICommandList& RHICmdList)
		{
			if (UVoxelGpuTexture* GpuTexture = Cast<UVoxelGpuTexture>(Impl->Texture))
			{
				GpuTexture->Update_RenderThread(nullptr);
			}

			ReturnToPool();
		});
	}
	else
	{
		ReturnToPool();
	}
}

UTexture* FVoxelTextureRef::Get() const
{
	ensure(IsInGameThread() || IsInRenderingThread());
	return Impl->Texture;
}