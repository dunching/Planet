// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelRuntime.h"
#include "VoxelActor.h"
#include "VoxelQuery.h"
#include "VoxelGraph.h"
#include "VoxelChannel.h"
#include "VoxelExecNode.h"
#include "VoxelExecNodes.h"
#include "VoxelParameterValues.h"
#include "VoxelParameterContainer.h"
#include "Application/ThrottleManager.h"

DEFINE_VOXEL_INSTANCE_COUNTER(FVoxelRuntime);

TSharedRef<FVoxelRuntime> FVoxelRuntime::Create(
	UObject& Instance,
	USceneComponent& RootComponent,
	const FVoxelRuntimeParameters& RuntimeParameters,
	UVoxelParameterContainer& ParameterContainer)
{
	VOXEL_FUNCTION_COUNTER();
	ensure(CanBeCreated(true));

	UVoxelGraphInterface* Graph = ParameterContainer.GetTypedProvider<UVoxelGraphInterface>();
	if (!Graph)
	{
		// Create a dummy runtime so the IsRuntimeCreated logic in OnProviderChanged
		// doesn't break when assigning another graph
		Graph = LoadObject<UVoxelGraphInterface>(nullptr, TEXT("/Voxel/Default/VG_Empty.VG_Empty"));
	}
	check(Graph);

	LOG_VOXEL(Verbose, "CreateRuntime %s %s", *RootComponent.GetPathName(), *Graph->GetPathName());

	UWorld* World = RootComponent.GetWorld();
	ensure(World);

	const TSharedRef<FVoxelRuntime> Runtime = MakeVoxelShared<FVoxelRuntime>();

	FVoxelRuntimeInfoBase RuntimeInfoBase = FVoxelRuntimeInfoBase::MakeFromWorld(World);
	RuntimeInfoBase.WeakRuntime = Runtime;
	RuntimeInfoBase.WeakWorld = World;
	RuntimeInfoBase.WeakInstance = &Instance;
	RuntimeInfoBase.WeakRootComponent = &RootComponent;
	RuntimeInfoBase.GraphPath = Graph->GetPathName();
	RuntimeInfoBase.InstanceName = RootComponent.GetPathName();
	RuntimeInfoBase.Parameters = RuntimeParameters;
	RuntimeInfoBase.LocalToWorld = FVoxelTransformRef::Make(RootComponent);
	Runtime->RuntimeInfo = RuntimeInfoBase.MakeRuntimeInfo();

	// Create node runtime last

	const FVoxelGraphNodeRef NodeRef
	{
		// TODO Refactor graph instances, this is a hacky fix
		Graph->GetGraph(),
		// See FCompilerUtilities::AddExecOutput
		FVoxelNodeNames::ExecuteNodeId,
	};

	const TSharedRef<FVoxelRootExecuteNode> Node = MakeVoxelShared<FVoxelRootExecuteNode>();
	Node->InitializeNodeRuntime(NodeRef, false);
	Node->RemoveEditorData();

	const TSharedPtr<FVoxelExecNodeRuntime> NodeRuntime = Node->CreateSharedExecRuntime(Node);
	check(NodeRuntime);

	const TSharedRef<FVoxelQueryContext> QueryContext = FVoxelQueryContext::Make(
		Runtime->RuntimeInfo.ToSharedRef(),
		FVoxelParameterValues::Create(&ParameterContainer));

	NodeRuntime->CallCreate(QueryContext, {});

	Runtime->NodeRuntime = NodeRuntime;

	return Runtime;
}

FVoxelRuntime::~FVoxelRuntime()
{
	ensure(RuntimeInfo->IsDestroyed());
}

bool FVoxelRuntime::CanBeCreated(const bool bLog)
{
	if (!GVoxelChannelManager->IsReady(bLog))
	{
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelRuntime::Destroy()
{
	VOXEL_FUNCTION_COUNTER();

	const USceneComponent* RootComponent = RuntimeInfo->GetRootComponent();
	LOG_VOXEL(Verbose, "DestroyRuntime %s", RootComponent ? *RootComponent->GetPathName() : TEXT(""));

	NodeRuntime.Reset();
	RuntimeInfo->Destroy();

	for (const TWeakObjectPtr<USceneComponent> WeakComponent : Components)
	{
		USceneComponent* Component = WeakComponent.Get();
		if (!Component)
		{
			// Will be raised if the component is destroyed manually
			// instead of through FVoxelRuntime::DestroyComponent
			ensureVoxelSlow(!RootComponent || GIsGarbageCollecting);
			continue;
		}

		Component->DestroyComponent();
	}

	Components.Empty();
	ComponentPools.Empty();
}

void FVoxelRuntime::Tick()
{
	VOXEL_FUNCTION_COUNTER();

	RuntimeInfo->Tick();

	const USceneComponent* RootComponent = RuntimeInfo->GetRootComponent();
	if (!ensure(RootComponent))
	{
		return;
	}

	NodeRuntime->Tick(*this);
}

void FVoxelRuntime::AddReferencedObjects(FReferenceCollector& Collector)
{
	VOXEL_FUNCTION_COUNTER();

	RuntimeInfo->AddReferencedObjects(Collector);
	NodeRuntime->AddReferencedObjects(Collector);
}

FVoxelOptionalBox FVoxelRuntime::GetBounds() const
{
	return NodeRuntime->GetBounds();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USceneComponent* FVoxelRuntime::CreateComponent(UClass* Class)
{
	VOXEL_FUNCTION_COUNTER();
	check(IsInGameThread());

	USceneComponent* RootComponent = RuntimeInfo->GetRootComponent();

	if (!ensure(RootComponent) ||
		!ensure(Class) ||
		!ensure(!RuntimeInfo->IsDestroyed()))
	{
		return nullptr;
	}

	TArray<TWeakObjectPtr<USceneComponent>>& Pool = ComponentPools.FindOrAdd(Class);
	while (Pool.Num() > 0)
	{
		if (USceneComponent* Component = Pool.Pop(false).Get())
		{
			return Component;
		}
	}

	AVoxelActor* Actor = Cast<AVoxelActor>(RootComponent->GetOwner());
	if (Actor)
	{
		ensure(!Actor->bDisableModify);
		Actor->bDisableModify = true;
	}

	USceneComponent* Component = NewObject<USceneComponent>(
		RootComponent,
		Class,
		NAME_None,
		RF_Transient |
		RF_DuplicateTransient |
		RF_TextExportTransient);

	Component->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	Component->SetRelativeTransform(FTransform::Identity);
	Component->RegisterComponent();

	if (Actor)
	{
		ensure(Actor->bDisableModify);
		Actor->bDisableModify = false;
	}

	Components.Add(Component);

	return Component;
}

void FVoxelRuntime::DestroyComponent_ReturnToPoolCalled(USceneComponent& Component)
{
	VOXEL_FUNCTION_COUNTER();
	check(IsInGameThread());
	ensure(Components.Contains(&Component));

	if (!ensure(!RuntimeInfo->IsDestroyed()))
	{
		return;
	}

	Component.SetRelativeTransform(FTransform::Identity);
	ComponentPools.FindOrAdd(Component.GetClass()).Add(&Component);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#if WITH_EDITOR
void FVoxelRuntime::FEditorTicker::Tick()
{
	ensure(!Runtime.RuntimeInfo->IsDestroyed());

	if (FSlateThrottleManager::Get().IsAllowingExpensiveTasks())
	{
		return;
	}

	// If slate is throttling actor tick (eg, when dragging a float property), force tick
	Runtime.Tick();
}
#endif