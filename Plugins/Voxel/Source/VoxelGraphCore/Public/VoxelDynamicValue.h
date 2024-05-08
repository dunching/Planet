// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelTask.h"
#include "VoxelQuery.h"

class VOXELGRAPHCORE_API FVoxelDynamicValueStateBase
{
public:
	enum class EState : uint8
	{
		NotComputed,
		Outdated,
		UpToDate
	};
	using FOnChanged = TVoxelUniqueFunction<void(const FVoxelRuntimePinValue& NewValue)>;

public:
	virtual ~FVoxelDynamicValueStateBase() = default;

	virtual FVoxelPinType GetType() const = 0;
	virtual void SetOnChanged(FOnChanged&& NewOnChanged) const = 0;

	FORCEINLINE EState GetState() const
	{
		return State;
	}

protected:
	EState State = EState::NotComputed;
};

class VOXELGRAPHCORE_API FVoxelDynamicValue
{
public:
	FVoxelDynamicValue() = default;
	FORCEINLINE explicit FVoxelDynamicValue(const TSharedRef<FVoxelDynamicValueStateBase>& State)
		: State(State)
	{
	}

	FORCEINLINE bool IsValid() const
	{
		return State.IsValid();
	}
	FORCEINLINE bool IsComputed() const
	{
		return State->GetState() != FVoxelDynamicValueStateBase::EState::NotComputed;
	}
	FORCEINLINE bool IsUpToDate() const
	{
		return State->GetState() == FVoxelDynamicValueStateBase::EState::UpToDate;
	}

	FORCEINLINE void OnChanged(FVoxelDynamicValueStateBase::FOnChanged&& Lambda) const
	{
		State->SetOnChanged(MoveTemp(Lambda));
	}

protected:
	TSharedPtr<FVoxelDynamicValueStateBase> State;
};

template<typename T>
class TVoxelDynamicValue : public FVoxelDynamicValue
{
public:
	TVoxelDynamicValue() = default;
	FORCEINLINE explicit TVoxelDynamicValue(const TSharedRef<FVoxelDynamicValueStateBase>& State)
		: FVoxelDynamicValue(State)
	{
		checkVoxelSlow(State->GetType().CanBeCastedTo<T>());
	}

	template<typename LambdaType>
	FORCEINLINE void OnChanged(LambdaType Lambda) const
	{
		if constexpr (VoxelPassByValue<T>)
		{
			FVoxelDynamicValue::OnChanged([Lambda = MoveTemp(Lambda)](const FVoxelRuntimePinValue& Value)
			{
				Lambda(Value.Get<T>());
			});
		}
		else
		{
			FVoxelDynamicValue::OnChanged([Lambda = MoveTemp(Lambda)](const FVoxelRuntimePinValue& Value)
			{
				Lambda(Value.GetSharedStruct<T>());
			});
		}
	}
	template<typename LambdaType>
	FORCEINLINE void OnChanged_GameThread(LambdaType Lambda) const
	{
		this->OnChanged([Lambda = MoveTemp(Lambda), WeakState = MakeWeakPtr(State)](const auto Value)
		{
			FVoxelUtilities::RunOnGameThread([Lambda, WeakState, Value]
			{
				if (WeakState.IsValid())
				{
					Lambda(Value);
				}
			});
		});
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class VOXELGRAPHCORE_API FVoxelDynamicValueFactoryBase
{
public:
	FVoxelDynamicValueFactoryBase() = default;
	FVoxelDynamicValueFactoryBase(
		const TSharedRef<const FVoxelComputeValue>& Compute,
		const FVoxelPinType& Type,
		const FName Name)
		: Compute(Compute)
		, Type(Type)
		, Name(Name)
		, Referencer(MakeVoxelShared<FVoxelTaskReferencer>(FVoxelUtilities::AppendName(TEXT("FVoxelDynamicValue - "), Name)))
	{
	}

	FORCEINLINE bool IsValid() const
	{
		return Compute.IsValid();
	}

protected:
	TSharedPtr<const FVoxelComputeValue> Compute;
	FVoxelPinType Type;
	FName Name;
	TSharedPtr<FVoxelTaskReferencer> Referencer;

	EVoxelTaskThread PrivateThread = EVoxelTaskThread::AsyncThread;
	FVoxelTaskPriority PrivatePriority;

	TSharedRef<FVoxelDynamicValueStateBase> ComputeState(
		const TSharedRef<FVoxelQueryContext>& Context,
		const TSharedRef<const FVoxelQueryParameters>& Parameters) const;
};

class FVoxelDynamicValueFactory : public FVoxelDynamicValueFactoryBase
{
public:
	using FVoxelDynamicValueFactoryBase::FVoxelDynamicValueFactoryBase;

	FORCEINLINE FVoxelDynamicValueFactory& Thread(const EVoxelTaskThread NewThread)
	{
		PrivateThread = NewThread;
		return *this;
	}
	FORCEINLINE FVoxelDynamicValueFactory& Priority(const FVoxelTaskPriority& NewPriority)
	{
		PrivatePriority = NewPriority;
		return *this;
	}

	template<typename RefType>
	FORCEINLINE FVoxelDynamicValueFactory& AddRef(const RefType& Ref)
	{
		Referencer->AddRef(Ref);
		return *this;
	}

	FORCEINLINE FVoxelDynamicValue Compute(
		const TSharedRef<FVoxelQueryContext>& Context,
		const TSharedRef<const FVoxelQueryParameters>& Parameters = MakeVoxelShared<FVoxelQueryParameters>()) const
	{
		return FVoxelDynamicValue(this->ComputeState(Context, Parameters));
	}
};

template<typename T>
class TVoxelDynamicValueFactory : public FVoxelDynamicValueFactoryBase
{
public:
	TVoxelDynamicValueFactory() = default;

	explicit TVoxelDynamicValueFactory(const FVoxelDynamicValueFactory& Factory)
		: FVoxelDynamicValueFactoryBase(Factory)
	{
		checkVoxelSlow(Type.CanBeCastedTo<T>());
	}

	TVoxelDynamicValueFactory(const FName Name, TVoxelComputeValue<T>&& Compute)
		: FVoxelDynamicValueFactoryBase(
			MakeVoxelShared<FVoxelComputeValue>([Compute = MoveTemp(Compute)](const FVoxelQuery& Query) -> FVoxelFutureValue
			{
				return Compute(Query);
			}),
			FVoxelPinType::Make<T>(),
			Name)
	{
	}

	FORCEINLINE TVoxelDynamicValueFactory& Thread(const EVoxelTaskThread NewThread)
	{
		PrivateThread = NewThread;
		return *this;
	}
	FORCEINLINE TVoxelDynamicValueFactory& Priority(const FVoxelTaskPriority& NewPriority)
	{
		PrivatePriority = NewPriority;
		return *this;
	}

	template<typename RefType>
	FORCEINLINE TVoxelDynamicValueFactory& AddRef(const RefType& Ref)
	{
		Referencer->AddRef(Ref);
		return *this;
	}
	FORCEINLINE TVoxelDynamicValue<T> Compute(
		const TSharedRef<FVoxelQueryContext>& Context,
		const TSharedRef<const FVoxelQueryParameters>& Parameters = MakeVoxelShared<FVoxelQueryParameters>()) const
	{
		return TVoxelDynamicValue<T>(this->ComputeState(Context, Parameters));
	}
};