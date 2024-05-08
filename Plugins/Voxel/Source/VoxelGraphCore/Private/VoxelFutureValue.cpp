// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelFutureValue.h"
#include "VoxelTask.h"
#include "VoxelTaskGroup.h"

FVoxelFutureValueStateImpl::~FVoxelFutureValueStateImpl()
{
	if (!UObjectInitialized())
	{
		// Global exit, nothing to be done
		return;
	}

	if (!IsComplete())
	{
		SetValue(FVoxelRuntimePinValue(Type));
	}

	ensure(IsComplete());
	ensure(DependentTasks.Num() == 0);
	ensure(LinkedStates.Num() == 0);
}

void FVoxelFutureValueStateImpl::AddDependentTask(FVoxelTask& Task)
{
	if (IsComplete())
	{
		Task.Group.OnDependencyComplete(Task);
		return;
	}

	{
		VOXEL_SCOPE_LOCK(CriticalSection);

		if (!IsComplete())
		{
			DependentTasks.Add({ Task.Group.AsWeak(), &Task });
			return;
		}
	}

	checkVoxelSlow(IsComplete());
	Task.Group.OnDependencyComplete(Task);
}

void FVoxelFutureValueStateImpl::AddLinkedState(const TSharedRef<FVoxelFutureValueStateImpl>& State)
{
	check(Type.CanBeCastedTo(State->Type));

	if (IsComplete())
	{
		State->SetValue(Value);
		return;
	}

	{
		VOXEL_SCOPE_LOCK(CriticalSection);

		if (!IsComplete())
		{
			LinkedStates.Add(State);
			return;
		}
	}

	checkVoxelSlow(IsComplete());
	State->SetValue(Value);
}

void FVoxelFutureValueStateImpl::SetValue(const FVoxelRuntimePinValue& NewValue)
{
	checkVoxelSlow(!IsComplete());
	checkVoxelSlow(!Value.IsValid());

	checkVoxelSlow(NewValue.IsValid());
	checkVoxelSlow(NewValue.CanBeCastedTo(Type));

	Value = NewValue;

	TVoxelArray<FDependentTask, FAllocator> DependentTasksCopy;
	TVoxelArray<TSharedPtr<FVoxelFutureValueStateImpl>, FAllocator> LinkedStatesCopy;
	{
		VOXEL_SCOPE_LOCK(CriticalSection);

		ensure(!IsComplete());
		bIsComplete.Store(true);

		DependentTasksCopy = MoveTemp(DependentTasks);
		LinkedStatesCopy = MoveTemp(LinkedStates);
	}
	checkVoxelSlow(DependentTasks.Num() == 0);
	checkVoxelSlow(LinkedStates.Num() == 0);

	for (const FDependentTask& DependentTask : DependentTasksCopy)
	{
		if (const TSharedPtr<FVoxelTaskGroup> Group = DependentTask.WeakGroup.Pin())
		{
			Group->OnDependencyComplete(*DependentTask.Task);
		}
	}

	for (const TSharedPtr<FVoxelFutureValueStateImpl>& State : LinkedStatesCopy)
	{
		State->SetValue(Value);
	}
}

FVoxelDummyFutureValue FVoxelFutureValue::MakeDummy()
{
	const TSharedRef<FVoxelFutureValueStateImpl> State = MakeVoxelShared<FVoxelFutureValueStateImpl>(FVoxelPinType::Make<FVoxelFutureValueDummyStruct>());
	return TVoxelFutureValue<FVoxelFutureValueDummyStruct>(FVoxelFutureValue(State));
}

void FVoxelFutureValue::MarkDummyAsCompleted() const
{
	check(IsValid());
	check(GetParentType().Is<FVoxelFutureValueDummyStruct>());

	State->GetStateImpl().SetValue(
		FVoxelRuntimePinValue::Make<FVoxelFutureValueDummyStruct>(
			MakeVoxelShared<FVoxelFutureValueDummyStruct>()));
}