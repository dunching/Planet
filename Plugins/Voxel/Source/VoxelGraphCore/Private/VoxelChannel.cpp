// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelChannel.h"
#include "VoxelSurface.h"
#include "VoxelSettings.h"
#include "VoxelDependency.h"
#include "VoxelQueryChannelNode.h"
#include "Point/VoxelChunkedPointSet.h"
#include "VoxelPositionQueryParameter.h"
#include "Engine/Engine.h"
#include "Engine/AssetManager.h"
#include "AssetRegistry/ARFilter.h"
#include "AssetRegistry/AssetData.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Interfaces/ITargetPlatformManagerModule.h"

DEFINE_VOXEL_FACTORY(UVoxelChannelRegistry);
DEFINE_VOXEL_INSTANCE_COUNTER(FVoxelBrush);
DEFINE_VOXEL_INSTANCE_COUNTER(FVoxelRuntimeChannel);

FVoxelChannelManager* GVoxelChannelManager = MakeVoxelSingleton(FVoxelChannelManager);

VOXEL_CONSOLE_VARIABLE(
	VOXELGRAPHCORE_API, bool, GVoxelShowBrushBounds, false,
	"voxel.ShowBrushBounds",
	"");

VOXEL_CONSOLE_COMMAND(
	LogAllBrushes,
	"voxel.LogAllBrushes",
	"")
{
	GVoxelChannelManager->LogAllBrushes_GameThread();
}

VOXEL_CONSOLE_COMMAND(
	LogAllChannels,
	"voxel.LogAllChannels",
	"")
{
	GVoxelChannelManager->LogAllChannels_GameThread();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelChannelExposedDefinition::Fixup()
{
	if (!Type.IsValid())
	{
		Type = FVoxelPinType::Make<FVoxelFloatBuffer>();
	}

	DefaultValue.Fixup(Type.GetExposedType(), nullptr);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void UVoxelChannelRegistry::UpdateChannels()
{
	VOXEL_FUNCTION_COUNTER();

	TSet<FName> UsedNames;
	for (FVoxelChannelExposedDefinition& Channel : Channels)
	{
		if (Channel.Name.IsNone())
		{
			Channel.Name = "MyChannel";
		}

		while (UsedNames.Contains(Channel.Name))
		{
			Channel.Name.SetNumber(Channel.Name.GetNumber() + 1);
		}

		UsedNames.Add(Channel.Name);

		Channel.Fixup();
	}

	GVoxelChannelManager->UpdateChannelsFromAsset_GameThread(
		this,
		GetName(),
		Channels);
}

void UVoxelChannelRegistry::PostLoad()
{
	Super::PostLoad();

	if (!IsTemplate())
	{
		UpdateChannels();
	}
}

void UVoxelChannelRegistry::BeginDestroy()
{
	if (!IsTemplate())
	{
		GVoxelChannelManager->RemoveChannelsFromAsset_GameThread(this);
	}

	Super::BeginDestroy();
}

#if WITH_EDITOR
void UVoxelChannelRegistry::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.ChangeType == EPropertyChangeType::Interactive)
	{
		return;
	}

	UpdateChannels();
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FString FVoxelChannelDefinition::ToString() const
{
	return FString::Printf(TEXT("%s Type=%s Default=%s"),
		*Name.ToString(),
		*Type.ToString(),
		*GetExposedDefaultValue().ExportToString());
}

FVoxelPinValue FVoxelChannelDefinition::GetExposedDefaultValue() const
{
	return FVoxelPinType::MakeExposedValue(DefaultValue, Type.IsBufferArray());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelBrushRef::~FVoxelBrushRef()
{
	VOXEL_FUNCTION_COUNTER();

	const TSharedPtr<FVoxelWorldChannel> Channel = WeakChannel.Pin();
	if (!Channel)
	{
		return;
	}

	// Don't invalidate while CriticalSection is locked
	const FVoxelDependencyInvalidationScope DependencyInvalidationScope;

	VOXEL_SCOPE_LOCK(Channel->CriticalSection);
	Channel->RemoveBrush_RequiresLock(BrushId);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedPtr<const FVoxelBrush> FVoxelRuntimeChannel::GetNextBrush(
	const FVoxelQuery& Query,
	const FVoxelBox& Bounds,
	const FVoxelBrushPriority Priority) const
{
	VOXEL_FUNCTION_COUNTER();

	Query.GetDependencyTracker().AddDependency(
		Dependency,
		Bounds,
		Priority.Raw);

	VOXEL_SCOPE_LOCK(CriticalSection);

	TSharedPtr<const FVoxelBrush> BestBrush;
	for (const auto& It : RuntimeBrushes_RequiresLock)
	{
		const FRuntimeBrush& Brush = *It.Value;

		if (Brush.Priority >= Priority ||
			// Can happen due to race condition in FVoxelRuntimeChannel::AddBrush
			!ensureVoxelSlow(Brush.RuntimeBounds_RequiresLock.IsSet()) ||
			!Brush.RuntimeBounds_RequiresLock.GetValue().Intersect(Bounds))
		{
			continue;
		}

		if (BestBrush &&
			BestBrush->Priority > Brush.Priority)
		{
			continue;
		}

		ensure(Brush.Priority < Priority);
		ensure(!BestBrush || BestBrush->Priority < Brush.Priority);

		BestBrush = Brush.Brush;
	}
	return BestBrush;
}

FVoxelFutureValue FVoxelRuntimeChannel::Get(const FVoxelQuery& Query) const
{
	const FVoxelBox Bounds = INLINE_LAMBDA
	{
		if (const FVoxelPositionQueryParameter* PositionQueryParameter = Query.GetParameters().Find<FVoxelPositionQueryParameter>())
		{
			return PositionQueryParameter->GetBounds();
		}
		if (const FVoxelQueryChannelBoundsQueryParameter* QueryChannelBoundsQueryParameter = Query.GetParameters().Find<FVoxelQueryChannelBoundsQueryParameter>())
		{
			return QueryChannelBoundsQueryParameter->Bounds;
		}
		if (const FVoxelPointChunkRefQueryParameter* PointChunkRefQueryParameter = Query.GetParameters().Find<FVoxelPointChunkRefQueryParameter>())
		{
			return PointChunkRefQueryParameter->ChunkRef.GetBounds();
		}

		ensure(false);
		return FVoxelBox();
	};

	float MinExactDistance = 0.f;
	if (const FVoxelMinExactDistanceQueryParameter* MinExactDistanceQueryParameter = Query.GetParameters().Find<FVoxelMinExactDistanceQueryParameter>())
	{
		MinExactDistance = MinExactDistanceQueryParameter->MinExactDistance;
	}

	FVoxelBrushPriority Priority = FVoxelBrushPriority::Max();
	if (const FVoxelBrushPriorityQueryParameter* BrushPriorityQueryParameter = Query.GetParameters().Find<FVoxelBrushPriorityQueryParameter>())
	{
		if (const FVoxelBrushPriority* PriorityPtr = BrushPriorityQueryParameter->ChannelToPriority.Find(Definition.Name))
		{
			Priority = *PriorityPtr;
		}
	}

	const TSharedPtr<const FVoxelBrush> Brush = GetNextBrush(
		Query,
		Bounds.Extend(MinExactDistance),
		Priority);

	if (!Brush)
	{
		return Definition.DefaultValue;
	}

	return Brush->Compute(Query);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelBrushPriority FVoxelRuntimeChannel::GetFullPriority(
	const int32 Priority,
	const FString& GraphPath,
	const FString* NodeId,
	const FString& InstanceName)
{
	ensure(!GraphPath.IsEmpty());
	ensure(!InstanceName.IsEmpty());

	FVoxelBrushPriority Result;
	Result.BasePriority = FMath::Clamp<int64>(int64(Priority) + MAX_uint16 / 2, 0, MAX_uint16 - 1);
	Result.GraphHash = FVoxelUtilities::MurmurHash(FCrc::StrCrc32(*GraphPath));
	Result.InstanceHash = FVoxelUtilities::MurmurHash(FCrc::StrCrc32(*InstanceName));
	Result.NodeHash =
		NodeId
		? FVoxelUtilities::MurmurHash(FCrc::StrCrc32(**NodeId))
		// For preview meshes we want to skip any brush node in the current graph
		: 0;
	return Result;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelRuntimeChannel::FVoxelRuntimeChannel(
	const TSharedRef<FVoxelWorldChannel>& WorldChannel,
	const FVoxelTransformRef& RuntimeLocalToWorld)
	: WorldChannel(WorldChannel)
	, Definition(WorldChannel->Definition)
	, RuntimeLocalToWorld(RuntimeLocalToWorld)
	, Dependency(FVoxelDependency::Create(STATIC_FNAME("Channel"), WorldChannel->Definition.Name))
{
}

void FVoxelRuntimeChannel::AddBrush(
	const FVoxelBrushId BrushId,
	const TSharedRef<const FVoxelBrush>& Brush)
{
	VOXEL_FUNCTION_COUNTER();

	const FVoxelTransformRef BrushToRuntime = Brush->LocalToWorld * RuntimeLocalToWorld.Inverse();
	const TSharedRef<FRuntimeBrush> RuntimeBrush = MakeVoxelShared<FRuntimeBrush>(Brush, BrushToRuntime);

	{
		VOXEL_SCOPE_LOCK(CriticalSection);
		RuntimeBrushes_RequiresLock.Add(BrushId, RuntimeBrush);
	}

	BrushToRuntime.AddOnChanged(MakeWeakPtrDelegate(RuntimeBrush, MakeWeakPtrLambda(this, [this, &RuntimeBrush = *RuntimeBrush](const FMatrix& NewTransform)
	{
		VOXEL_SCOPE_LOCK(CriticalSection);

		if (RuntimeBrush.RuntimeBounds_RequiresLock.IsSet())
		{
			FVoxelDependency::FInvalidationParameters Parameters;
			Parameters.Bounds = RuntimeBrush.RuntimeBounds_RequiresLock.GetValue();
			Parameters.LessOrEqualTag = RuntimeBrush.Priority.Raw;
			Dependency->Invalidate(Parameters);
		}

		RuntimeBrush.RuntimeBounds_RequiresLock = RuntimeBrush.Brush->LocalBounds.TransformBy(NewTransform);

		if (RuntimeBrush.Brush->LocalBounds.IsInfinite())
		{
			RuntimeBrush.RuntimeBounds_RequiresLock = FVoxelBox::Infinite;
		}

		FVoxelDependency::FInvalidationParameters Parameters;
		Parameters.Bounds = RuntimeBrush.RuntimeBounds_RequiresLock.GetValue();
		Parameters.LessOrEqualTag = RuntimeBrush.Priority.Raw;
		Dependency->Invalidate(Parameters);
	})));
}

void FVoxelRuntimeChannel::RemoveBrush(
	const FVoxelBrushId BrushId,
	const TSharedRef<const FVoxelBrush>& Brush)
{
	VOXEL_FUNCTION_COUNTER();

	TSharedPtr<FRuntimeBrush> RuntimeBrush;
	{
		VOXEL_SCOPE_LOCK(CriticalSection);
		if (!ensure(RuntimeBrushes_RequiresLock.RemoveAndCopyValue(BrushId, RuntimeBrush)))
		{
			return;
		}
	}

	FVoxelDependency::FInvalidationParameters Parameters;
	Parameters.Bounds = RuntimeBrush->RuntimeBounds_RequiresLock.GetValue();
	Parameters.LessOrEqualTag = Brush->Priority.Raw;
	Dependency->Invalidate(Parameters);
}

TSharedRef<FVoxelRuntimeChannelCache> FVoxelRuntimeChannelCache::Create()
{
	ensure(IsInGameThread());

	const TSharedRef<FVoxelRuntimeChannelCache> Cache = MakeVoxelShareable(new (GVoxelMemory) FVoxelRuntimeChannelCache());
	GVoxelChannelManager->OnChannelDefinitionsChanged_GameThread.Add(MakeWeakPtrDelegate(Cache, [&Cache = *Cache]
	{
		ensure(IsInGameThread());
		VOXEL_SCOPE_LOCK(Cache.CriticalSection);
		Cache.Channels_RequiresLock.Reset();
	}));
	return Cache;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelWorldChannel::AddBrush(
	const TSharedRef<const FVoxelBrush>& Brush,
	TSharedPtr<FVoxelBrushRef>& OutBrushRef)
{
	VOXEL_FUNCTION_COUNTER();

	// Don't invalidate while CriticalSection is locked
	const FVoxelDependencyInvalidationScope DependencyInvalidationScope;

	// Never delete brush refs inside CriticalSection
	const TSharedPtr<FVoxelBrushRef> BrushRefToDelete = MoveTemp(OutBrushRef);
	ensure(!OutBrushRef);

	VOXEL_SCOPE_LOCK(CriticalSection);

	if (BrushRefToDelete &&
		BrushRefToDelete.IsUnique() &&
		BrushRefToDelete->WeakChannel == AsWeak())
	{
		// Remove any previous brush before adding it again to avoid priority duplicates
		RemoveBrush_RequiresLock(BrushRefToDelete->BrushId);
		BrushRefToDelete->WeakChannel = {};
		BrushRefToDelete->BrushId = {};
	}

#if VOXEL_DEBUG
	for (const TSharedPtr<const FVoxelBrush>& OtherBrush : Brushes_RequiresLock)
	{
		ensure(Brush->Priority != OtherBrush->Priority);
	}
#endif

	const FVoxelBrushId BrushId = Brushes_RequiresLock.Add(Brush);
	OutBrushRef = MakeVoxelShareable(new (GVoxelMemory) FVoxelBrushRef(AsShared(), BrushId));

	for (int32 Index = 0; Index < WeakRuntimeChannels_RequiresLock.Num(); Index++)
	{
		const TSharedPtr<FVoxelRuntimeChannel> RuntimeChannel = WeakRuntimeChannels_RequiresLock[Index].Pin();
		if (!RuntimeChannel)
		{
			WeakRuntimeChannels_RequiresLock.RemoveAtSwap(Index);
			Index--;
			continue;
		}

		RuntimeChannel->AddBrush(BrushId, Brush);
	}
}

TSharedRef<FVoxelRuntimeChannel> FVoxelWorldChannel::GetRuntimeChannel(
	const FVoxelTransformRef& RuntimeLocalToWorld,
	FVoxelRuntimeChannelCache& Cache)
{
	VOXEL_SCOPE_LOCK(Cache.CriticalSection);

	if (const TSharedPtr<FVoxelRuntimeChannel>* ChannelPtr = Cache.Channels_RequiresLock.Find(Definition.Name))
	{
		checkVoxelSlow((**ChannelPtr).RuntimeLocalToWorld == RuntimeLocalToWorld);
		return ChannelPtr->ToSharedRef();
	}

	VOXEL_FUNCTION_COUNTER();

	const TSharedRef<FVoxelRuntimeChannel> Channel = MakeVoxelShareable(new(GVoxelMemory) FVoxelRuntimeChannel(AsShared(), RuntimeLocalToWorld));
	Cache.Channels_RequiresLock.Add(Definition.Name, Channel);

	VOXEL_SCOPE_LOCK(CriticalSection);

	WeakRuntimeChannels_RequiresLock.Add(Channel);

	for (int32 Index = 0; Index < Brushes_RequiresLock.GetMaxIndex(); Index++)
	{
		if (!Brushes_RequiresLock.IsAllocated(Index))
		{
			continue;
		}

		const FVoxelBrushId BrushId = Brushes_RequiresLock.MakeIndex(Index);
		Channel->AddBrush(BrushId, Brushes_RequiresLock[BrushId].ToSharedRef());
	}

	return Channel;
}

void FVoxelWorldChannel::DrawBrushBounds(const FObjectKey World) const
{
	VOXEL_FUNCTION_COUNTER();
	VOXEL_SCOPE_LOCK(CriticalSection);

	for (const TSharedPtr<const FVoxelBrush>& Brush : Brushes_RequiresLock)
	{
		FVoxelGameUtilities::DrawBox(World, Brush->LocalBounds, Brush->LocalToWorld.Get_NoDependency());
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelWorldChannel::RemoveBrush_RequiresLock(const FVoxelBrushId BrushId)
{
	VOXEL_FUNCTION_COUNTER();
	checkVoxelSlow(CriticalSection.IsLocked());

	const TSharedRef<const FVoxelBrush> Brush = Brushes_RequiresLock[BrushId].ToSharedRef();
	Brushes_RequiresLock.RemoveAt(BrushId);

	for (int32 Index = 0; Index < WeakRuntimeChannels_RequiresLock.Num(); Index++)
	{
		const TSharedPtr<FVoxelRuntimeChannel> RuntimeChannel = WeakRuntimeChannels_RequiresLock[Index].Pin();
		if (!RuntimeChannel)
		{
			WeakRuntimeChannels_RequiresLock.RemoveAtSwap(Index);
			Index--;
			continue;
		}

		RuntimeChannel->RemoveBrush(BrushId, Brush);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool FVoxelWorldChannelManager::RegisterChannel(const FVoxelChannelDefinition& ChannelDefinition)
{
	VOXEL_SCOPE_LOCK(CriticalSection);

	if (GVoxelChannelManager->FindChannelDefinition(ChannelDefinition.Name))
	{
		return false;
	}

	if (Channels_RequiresLock.Contains(ChannelDefinition.Name))
	{
		return false;
	}

	const TSharedRef<FVoxelWorldChannel> Channel = MakeVoxelShareable(new (GVoxelMemory) FVoxelWorldChannel(ChannelDefinition));
	Channels_RequiresLock.Add(ChannelDefinition.Name, Channel);
	return true;
}

TSharedPtr<FVoxelWorldChannel> FVoxelWorldChannelManager::FindChannel(const FName Name)
{
	VOXEL_SCOPE_LOCK(CriticalSection);

	if (const TSharedPtr<FVoxelWorldChannel>* ChannelPtr = Channels_RequiresLock.Find(Name))
	{
		return ChannelPtr->ToSharedRef();
	}

	const TOptional<FVoxelChannelDefinition> ChannelDefinition = GVoxelChannelManager->FindChannelDefinition(Name);
	if (!ChannelDefinition)
	{
		// Invalid channel
		return nullptr;
	}
	check(ChannelDefinition->Name == Name);

	const TSharedRef<FVoxelWorldChannel> Channel = MakeVoxelShareable(new (GVoxelMemory) FVoxelWorldChannel(*ChannelDefinition));
	Channels_RequiresLock.Add(Name, Channel);
	return Channel;
}

TArray<FName> FVoxelWorldChannelManager::GetValidChannelNames() const
{
	VOXEL_SCOPE_LOCK(CriticalSection);

	TArray<FName> Result;
	Channels_RequiresLock.GenerateKeyArray(Result);
	Result.Append(GVoxelChannelManager->GetValidChannelNames());
	return TSet<FName>(Result).Array();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelWorldChannelManager::Tick()
{
	VOXEL_FUNCTION_COUNTER();

	if (!GVoxelShowBrushBounds)
	{
		return;
	}

	VOXEL_SCOPE_LOCK(CriticalSection);

	for (const auto& It : Channels_RequiresLock)
	{
		It.Value->DrawBrushBounds(GetWorld_AnyThread());
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool FVoxelChannelManager::IsReady(const bool bLog) const
{
	if (!bChannelRegistriesLoaded)
	{
		if (bLog)
		{
			LOG_VOXEL(Log, "Not ready: bChannelRegistriesLoaded = false");
		}
		return false;
	}

	if (PendingHandles.Num() == 0)
	{
		return true;
	}

	if (bLog)
	{
		for (const auto& Handle : PendingHandles)
		{
			LOG_VOXEL(Log, "Not ready: waiting for %s to load", *Handle->GetDebugName());
		}
	}
	return false;
}

TArray<FName> FVoxelChannelManager::GetValidChannelNames() const
{
	VOXEL_SCOPE_LOCK(CriticalSection);

	TArray<FName> Result;
	for (const auto& AssetIt : AssetToChannelDefinitions_RequiresLock)
	{
		for (const auto& It : AssetIt.Value)
		{
			Result.Add(It.Key);
		}
	}
	return Result;
}

TArray<const UObject*> FVoxelChannelManager::GetChannelAssets() const
{
	VOXEL_FUNCTION_COUNTER();
	VOXEL_SCOPE_LOCK(CriticalSection);

	TArray<const UObject*> Assets;
	for (const auto& It : AssetToChannelDefinitions_RequiresLock)
	{
		if (ensureVoxelSlow(It.Key))
		{
			Assets.Add(It.Key);
		}
	}
	return Assets;
}

TOptional<FVoxelChannelDefinition> FVoxelChannelManager::FindChannelDefinition(const FName Name) const
{
	VOXEL_SCOPE_LOCK(CriticalSection);

	const FVoxelChannelDefinition* Definition = nullptr;
	for (const auto& It : AssetToChannelDefinitions_RequiresLock)
	{
		if (const FVoxelChannelDefinition* NewDefinition = It.Value.Find(Name))
		{
			ensure(!Definition);
			Definition = NewDefinition;
		}
	}
	if (!Definition)
	{
		if (!IsReady(true))
		{
			VOXEL_MESSAGE(Error, "Querying channels before channel registry assets are done loading");
		}
		return {};
	}
	return *Definition;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelChannelManager::LogAllBrushes_GameThread()
{
	VOXEL_FUNCTION_COUNTER();
	VOXEL_SCOPE_LOCK(CriticalSection);

	for (const UWorld* World : TObjectRange<UWorld>())
	{
		const TSharedRef<FVoxelWorldChannelManager> ChannelManager = FVoxelWorldChannelManager::Get(World);

		VOXEL_SCOPE_LOCK(ChannelManager->CriticalSection);

		for (const auto& ChannelIt : ChannelManager->Channels_RequiresLock)
		{
			const FVoxelWorldChannel& Channel = *ChannelIt.Value;

			VOXEL_SCOPE_LOCK(Channel.CriticalSection);
			for (const TSharedPtr<const FVoxelBrush>& Brush : Channel.Brushes_RequiresLock)
			{
				LOG_VOXEL(Log, "World: %s: Channel %s: Brush %s: Priority=%llu LocalBounds=%s",
					*World->GetPathName(),
					*Channel.Definition.Name.ToString(),
					*Brush->DebugName.ToString(),
					Brush->Priority.Raw,
					Brush->LocalBounds.IsInfinite() ? TEXT("Infinite") : *Brush->LocalBounds.ToString());
			}
		}
	}
}

void FVoxelChannelManager::LogAllChannels_GameThread()
{
	VOXEL_FUNCTION_COUNTER();
	VOXEL_SCOPE_LOCK(CriticalSection);

	LOG_VOXEL(Log, "Global channels:");
	for (const auto& AssetIt : AssetToChannelDefinitions_RequiresLock)
	{
		LOG_VOXEL(Log, "\t%s:", AssetIt.Key ? *AssetIt.Key->GetPathName() : TEXT(""));

		for (const auto& It : AssetIt.Value)
		{
			LOG_VOXEL(Log, "\t\t%s", *It.Value.ToString());
		}
	}

	LOG_VOXEL(Log, "World channels:");
	for (const UWorld* World : TObjectRange<UWorld>())
	{
		const TSharedRef<FVoxelWorldChannelManager> ChannelManager = FVoxelWorldChannelManager::Get(World);

		VOXEL_SCOPE_LOCK(ChannelManager->CriticalSection);
		LOG_VOXEL(Log, "\t%s:", *World->GetPathName());

		for (const auto& ChannelIt : ChannelManager->Channels_RequiresLock)
		{
			LOG_VOXEL(Log, "\t\t%s (%d brushes)",
				*ChannelIt.Value->Definition.ToString(),
				ChannelIt.Value->Brushes_RequiresLock.Num());
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool FVoxelChannelManager::RegisterChannel(const FVoxelChannelDefinition& ChannelDefinition)
{
	VOXEL_FUNCTION_COUNTER();
	VOXEL_SCOPE_LOCK(CriticalSection);

	for (const auto& It : AssetToChannelDefinitions_RequiresLock)
	{
		if (It.Value.Contains(ChannelDefinition.Name))
		{
			return false;
		}
	}

	static UVoxelChannelRegistry* ManualChannelRegistry = nullptr;
	if (!ManualChannelRegistry)
	{
		ManualChannelRegistry = NewObject<UVoxelChannelRegistry>(GetTransientPackage(), "ManualChannelRegistry");
		ManualChannelRegistry->AddToRoot();
	}

	AssetToChannelDefinitions_RequiresLock.FindOrAdd(ManualChannelRegistry).Add(ChannelDefinition.Name, ChannelDefinition);
	bRefreshQueued = true;
	return true;
}

void FVoxelChannelManager::UpdateChannelsFromAsset_GameThread(
	const UObject* Asset,
	const FString& Prefix,
	const TArray<FVoxelChannelExposedDefinition>& Channels)
{
	VOXEL_FUNCTION_COUNTER();

	TVoxelMap<FName, FVoxelChannelDefinition> NewChannelDefinitions;
	for (const FVoxelChannelExposedDefinition& Channel : Channels)
	{
		const FName Name = Prefix + "." + Channel.Name;

		NewChannelDefinitions.Add(Name, FVoxelChannelDefinition
		{
			Name,
			Channel.Type,
			FVoxelPinType::MakeRuntimeValue(Channel.Type, Channel.DefaultValue)
		});
	}

	{
		VOXEL_SCOPE_LOCK(CriticalSection);

		TVoxelMap<FName, FVoxelChannelDefinition>& ChannelDefinitions = AssetToChannelDefinitions_RequiresLock.FindOrAdd(Asset);
		if (ChannelDefinitions.OrderIndependentCompareEqual(NewChannelDefinitions))
		{
			return;
		}

		ChannelDefinitions = NewChannelDefinitions;
	}

	bRefreshQueued = true;
}

void FVoxelChannelManager::RemoveChannelsFromAsset_GameThread(const UObject* Asset)
{
	VOXEL_FUNCTION_COUNTER();
	VOXEL_SCOPE_LOCK(CriticalSection);
	ensure(AssetToChannelDefinitions_RequiresLock.Remove(Asset));
	bRefreshQueued = true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelChannelManager::Initialize()
{
	VOXEL_FUNCTION_COUNTER();

	GetMutableDefault<UVoxelSettings>()->UpdateChannels();

	// Nothing should depend on channels so early, don't trigger a refresh
	ClearQueuedRefresh();
	// Force a tick now in case assets are loaded
	Tick();
	// Nothing should depend on channels so early, don't trigger a refresh
	ClearQueuedRefresh();
}

void FVoxelChannelManager::Tick()
{
	VOXEL_FUNCTION_COUNTER();

	if (bRefreshQueued)
	{
		bRefreshQueued = false;

		OnChannelDefinitionsChanged_GameThread.Broadcast();

		for (const TSharedRef<FVoxelWorldChannelManager>& Subsystem : FVoxelWorldChannelManager::GetAll())
		{
			VOXEL_SCOPE_LOCK(Subsystem->CriticalSection);
			Subsystem->Channels_RequiresLock.Reset();
		}

		if (ensure(GEngine))
		{
			GEngine->Exec(nullptr, TEXT("voxel.RefreshAll"));
		}
	}

	if (!bChannelRegistriesLoaded &&
		// No engine in commandlets
		GEngine)
	{
		IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();

		if (!FPlatformProperties::RequiresCookedData() &&
			!AssetRegistry.IsSearchAllAssets())
		{
			// Force search all assets in standalone
			AssetRegistry.SearchAllAssets(false);
		}

		if (!AssetRegistry.IsLoadingAssets())
		{
			bChannelRegistriesLoaded = true;

			TArray<FAssetData> AssetDatas;
			FARFilter Filter;
			Filter.ClassPaths.Add(UVoxelChannelRegistry::StaticClass()->GetClassPathName());
			AssetRegistry.GetAssets(Filter, AssetDatas);

			AssetRegistry.OnAssetAdded().AddLambda([](const FAssetData& AssetData)
			{
				ensure(GIsEditor);

				if (AssetData.GetClass() != StaticClassFast<UVoxelChannelRegistry>())
				{
					return;
				}

				ensure(TSoftObjectPtr<UVoxelChannelRegistry>(AssetData.GetSoftObjectPath()).LoadSynchronous());
			});

			FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
			for (const FAssetData& AssetData : AssetDatas)
			{
				const TSharedPtr<FStreamableHandle> Handle = StreamableManager.RequestAsyncLoad(AssetData.GetSoftObjectPath());
				if (!ensure(Handle))
				{
					continue;
				}
				ConstCast(Handle->GetDebugName()) = AssetData.GetSoftObjectPath().ToString();
				PendingHandles.Add(Handle);
			}
		}
	}

	PendingHandles.RemoveAllSwap([](const TSharedPtr<FStreamableHandle>& Handle)
	{
		return Handle->HasLoadCompleted();
	});

	for (const TSharedPtr<FStreamableHandle>& Handle : PendingHandles)
	{
		LOG_VOXEL(Verbose, "Waiting for %s", *Handle->GetDebugName());
	}
}

void FVoxelChannelManager::AddReferencedObjects(FReferenceCollector& Collector)
{
	VOXEL_FUNCTION_COUNTER();
	VOXEL_SCOPE_LOCK(CriticalSection);

	for (auto& It : AssetToChannelDefinitions_RequiresLock)
	{
		ensure(It.Key);
		Collector.AddReferencedObject(It.Key);
		ensure(It.Key);
	}
}