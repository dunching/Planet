// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelExecNodeRuntimeWrapper.h"

void FVoxelExecNodeRuntimeWrapper::Initialize(const TSharedRef<FVoxelQueryContext>& NewContext)
{
	Context = NewContext->EnterScope(Node->GetNodeRef());

	EnableNodeValue =
		Node->GetNodeRuntime().MakeDynamicValueFactory(Node->EnableNodePin)
		.AddRef(Node)
		.Thread(EVoxelTaskThread::GameThread)
		.Compute(Context.ToSharedRef());

	EnableNodeValue.OnChanged_GameThread(MakeWeakPtrLambda(this, [=](const bool bNewEnableNode)
	{
		if (!bNewEnableNode)
		{
			NodeRuntime_GameThread.Reset();
			ConstantPins_GameThread = {};
			return;
		}

		if (NodeRuntime_GameThread)
		{
			return;
		}

		if (ConstantPins_GameThread.Num() == 0)
		{
			ComputeConstantPins();
		}

		// NodeRuntime_GameThread will be set if ComputeConstantPins completes right away
		if (!NodeRuntime_GameThread)
		{
			OnConstantValueUpdated();
		}
	}));
}

void FVoxelExecNodeRuntimeWrapper::Tick(FVoxelRuntime& Runtime)
{
	ensure(IsInGameThread());

	if (!NodeRuntime_GameThread)
	{
		return;
	}

	FVoxelQueryScope Scope(nullptr, Context.Get());
	NodeRuntime_GameThread->Tick(Runtime);
}

void FVoxelExecNodeRuntimeWrapper::AddReferencedObjects(FReferenceCollector& Collector)
{
	if (!NodeRuntime_GameThread)
	{
		return;
	}

	FVoxelQueryScope Scope(nullptr, Context.Get());
	NodeRuntime_GameThread->AddReferencedObjects(Collector);
}

FVoxelOptionalBox FVoxelExecNodeRuntimeWrapper::GetBounds() const
{
	ensure(IsInGameThread());

	if (!NodeRuntime_GameThread)
	{
		return {};
	}

	FVoxelQueryScope Scope(nullptr, Context.Get());
	return NodeRuntime_GameThread->GetBounds();
}

void FVoxelExecNodeRuntimeWrapper::ComputeConstantPins()
{
	VOXEL_FUNCTION_COUNTER();
	ensure(IsInGameThread());
	ensure(ConstantPins_GameThread.Num() == 0);

	// Pre-allocate ConstantPins_GameThread to avoid OnChanged calling OnConstantValueUpdated right away
	for (const auto& It : Node->GetNodeRuntime().GetPinDatas())
	{
		const FVoxelNodeRuntime::FPinData& PinData = *It.Value;
		if (!PinData.Metadata.bConstantPin ||
			!ensure(PinData.bIsInput))
		{
			continue;
		}

		const TSharedRef<FConstantValue> ConstantValue = MakeVoxelShared<FConstantValue>();

		// Don't use MakeDynamicValueFactory to skip bConstantPin assert
		ConstantValue->DynamicValue =
			FVoxelDynamicValueFactory(PinData.Compute.ToSharedRef(), PinData.Type, PinData.StatName)
			.AddRef(Node)
			.Thread(EVoxelTaskThread::GameThread)
			.Compute(Context.ToSharedRef());

		ConstantPins_GameThread.Add(It.Key, ConstantValue);
	}

	for (const auto& It : ConstantPins_GameThread)
	{
		FConstantValue& ConstantValue = *It.Value;

		const auto OnChanged = MakeWeakPtrLambda(this, MakeWeakPtrLambda(ConstantValue, [this, &ConstantValue](const FVoxelRuntimePinValue& NewValue)
		{
			checkVoxelSlow(IsInGameThread());
			ConstantValue.Value = NewValue;
			OnConstantValueUpdated();
		}));

		ConstantValue.DynamicValue.OnChanged([OnChanged](const FVoxelRuntimePinValue& NewValue)
		{
			FVoxelUtilities::RunOnGameThread([OnChanged, NewValue]
			{
				OnChanged(NewValue);
			});
		});
	}
}

void FVoxelExecNodeRuntimeWrapper::OnConstantValueUpdated()
{
	VOXEL_FUNCTION_COUNTER();
	ensure(IsInGameThread());

	for (const auto& It : ConstantPins_GameThread)
	{
		if (!It.Value->Value.IsValid())
		{
			// Not ready yet
			return;
		}
	}

	TVoxelMap<FName, FVoxelRuntimePinValue> ConstantValues;
	ConstantValues.Reserve(ConstantPins_GameThread.Num());
	for (const auto& It : ConstantPins_GameThread)
	{
		ConstantValues.Add(It.Key, It.Value->Value);
	}

	if (NodeRuntime_GameThread)
	{
		NodeRuntime_GameThread.Reset();

		LOG_VOXEL(Verbose, "Recreating exec node %s", *Node->GetNodeRef().EdGraphNodeTitle.ToString());
	}

	NodeRuntime_GameThread = Node->CreateSharedExecRuntime(Node);

	if (!NodeRuntime_GameThread)
	{
		// Some nodes don't create on server, like Generate Marching Cube Surface
		return;
	}

	NodeRuntime_GameThread->CallCreate(Context.ToSharedRef(), MoveTemp(ConstantValues));
}