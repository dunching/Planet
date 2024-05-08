// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelRuntimePinValue.h"
#include "VoxelFutureValue.generated.h"

class FVoxelQuery;
class FVoxelTaskGroup;
class FVoxelFutureValueStateImpl;
struct FVoxelTask;

template<typename, typename = void>
class TVoxelFutureValue;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT()
struct FVoxelFutureValueDummyStruct
{
	GENERATED_BODY()
};

using FVoxelDummyFutureValue = TVoxelFutureValue<FVoxelFutureValueDummyStruct>;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

enum class EVoxelTaskThread : uint8
{
	GameThread,
	RenderThread,
	AsyncThread
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class VOXELGRAPHCORE_API IVoxelFutureValueState
{
public:
	const FVoxelPinType Type;
	const bool bHasComplexState;

	FORCEINLINE explicit IVoxelFutureValueState(const FVoxelRuntimePinValue& Value)
		: Type(Value.GetType())
		, bHasComplexState(false)
		, bIsComplete(true)
		, Value(Value)
	{
	}

	FORCEINLINE bool IsComplete() const
	{
		return bIsComplete.Load();
	}
	FORCEINLINE const FVoxelRuntimePinValue& GetValue_CheckCompleted() const
	{
		checkVoxelSlow(IsComplete());
		return Value;
	}

	FVoxelFutureValueStateImpl& GetStateImpl();

protected:
	TVoxelAtomic<bool> bIsComplete = false;
	FVoxelRuntimePinValue Value;

	FORCEINLINE explicit IVoxelFutureValueState(const FVoxelPinType& Type)
		: Type(Type)
		, bHasComplexState(true)
	{
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class VOXELGRAPHCORE_API FVoxelFutureValueStateImpl
	: public IVoxelFutureValueState
	, public TSharedFromThis<FVoxelFutureValueStateImpl>
{
public:
	FORCEINLINE explicit FVoxelFutureValueStateImpl(const FVoxelPinType& Type)
		: IVoxelFutureValueState(Type)
	{
	}
	~FVoxelFutureValueStateImpl();
	UE_NONCOPYABLE(FVoxelFutureValueStateImpl);

	void AddDependentTask(FVoxelTask& Task);
	void AddLinkedState(const TSharedRef<FVoxelFutureValueStateImpl>& State);

	void SetValue(const FVoxelRuntimePinValue& NewValue);

private:
	struct FDependentTask
	{
		TWeakPtr<FVoxelTaskGroup> WeakGroup;
		FVoxelTask* Task = nullptr;
	};
	using FAllocator = TVoxelInlineAllocator<8>;

	FVoxelFastCriticalSection CriticalSection;
	TVoxelArray<FDependentTask, FAllocator> DependentTasks;
	TVoxelArray<TSharedPtr<FVoxelFutureValueStateImpl>, FAllocator> LinkedStates;
};

template<typename T>
class TVoxelFutureValueState : public FVoxelFutureValueStateImpl
{
public:
	TVoxelFutureValueState()
		: FVoxelFutureValueStateImpl(FVoxelPinType::Make<T>())
	{
	}

	void SetValue(const TSharedRef<const T>& NewValue)
	{
		FVoxelFutureValueStateImpl::SetValue(FVoxelRuntimePinValue::Make(NewValue));
	}
};

FORCEINLINE FVoxelFutureValueStateImpl& IVoxelFutureValueState::GetStateImpl()
{
	checkVoxelSlow(bHasComplexState);
	return static_cast<FVoxelFutureValueStateImpl&>(*this);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class VOXELGRAPHCORE_API FVoxelFutureValue
{
public:
	using Type = void;

	FVoxelFutureValue() = default;

	FORCEINLINE FVoxelFutureValue(const FVoxelRuntimePinValue& Value)
	{
		if (Value.IsValid())
		{
			State = MakeVoxelShared<IVoxelFutureValueState>(Value);
		}
	}
	explicit FVoxelFutureValue(const TSharedRef<IVoxelFutureValueState>& State)
		: State(State)
	{
	}

	FORCEINLINE bool IsValid() const
	{
		return State.IsValid();
	}
	FORCEINLINE bool IsComplete() const
	{
		return State->IsComplete();
	}

	// Not the type of the actual value - just the type of this future
	// Use .Get().GetType() to get actual type once complete
	FORCEINLINE const FVoxelPinType& GetParentType() const
	{
		return State->Type;
	}
	FORCEINLINE const FVoxelRuntimePinValue& GetValue_CheckCompleted() const
	{
		return State->GetValue_CheckCompleted();
	}

	template<typename T>
	FORCEINLINE auto Get_CheckCompleted() const -> decltype(auto)
	{
		return GetValue_CheckCompleted().Get<T>();
	}
	template<typename T>
	FORCEINLINE TSharedRef<const T> GetShared_CheckCompleted() const
	{
		return GetValue_CheckCompleted().GetSharedStruct<T>();
	}

public:
	static FVoxelDummyFutureValue MakeDummy();
	void MarkDummyAsCompleted() const;

private:
	TSharedPtr<IVoxelFutureValueState> State;

	friend class FVoxelTaskFactory;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template<typename T>
class TVoxelFutureValueImpl : public FVoxelFutureValue
{
public:
	using Type = T;
	checkStatic(TIsSafeVoxelPinValue<T>::Value);

	TVoxelFutureValueImpl() = default;

	FORCEINLINE TVoxelFutureValueImpl(const FVoxelRuntimePinValue& Value)
		: FVoxelFutureValue(Value)
	{
	}
	template<typename OtherType, typename = typename TEnableIf<TIsDerivedFrom<OtherType, T>::Value && !std::is_same_v<OtherType, T>>::Type>
	FORCEINLINE TVoxelFutureValueImpl(const TVoxelFutureValueImpl<OtherType>& Value)
		: FVoxelFutureValue(FVoxelFutureValue(Value))
	{
		checkVoxelSlow(Value.GetParentType().template CanBeCastedTo<T>());
	}
	FORCEINLINE TVoxelFutureValueImpl(const TSharedRef<TVoxelFutureValueState<T>>& State)
		: FVoxelFutureValue(State)
	{
	}
	FORCEINLINE explicit TVoxelFutureValueImpl(const FVoxelFutureValue& Value)
		: FVoxelFutureValue(Value)
	{
		checkVoxelSlow(!Value.IsValid() || Value.GetParentType().CanBeCastedTo<T>());
	}

	template<
		typename OtherType,
		typename = typename TEnableIf<std::is_same_v<OtherType, typename TVoxelBufferInnerType<T>::Type>>::Type,
		typename = void>
	FORCEINLINE TVoxelFutureValueImpl(const OtherType& Value)
		: TVoxelFutureValueImpl(T(Value))
	{
	}
	template<typename OtherType, typename = typename TEnableIf<TIsDerivedFrom<OtherType, T>::Value>::Type>
	FORCEINLINE TVoxelFutureValueImpl(const OtherType& Value)
		: FVoxelFutureValue(FVoxelRuntimePinValue::Make(Value))
	{
	}
	template<typename OtherType, typename = typename TEnableIf<TIsDerivedFrom<OtherType, T>::Value>::Type>
	FORCEINLINE TVoxelFutureValueImpl(const TSharedPtr<OtherType>& Value)
		: FVoxelFutureValue(Value.IsValid() ? FVoxelRuntimePinValue::Make(Value.ToSharedRef()) : FVoxelRuntimePinValue())
	{
	}
	template<typename OtherType, typename = typename TEnableIf<TIsDerivedFrom<OtherType, T>::Value>::Type>
	FORCEINLINE TVoxelFutureValueImpl(const TSharedPtr<const OtherType>& Value)
		: FVoxelFutureValue(Value.IsValid() ? FVoxelRuntimePinValue::Make(Value.ToSharedRef()) : FVoxelRuntimePinValue())
	{
	}
	template<typename OtherType, typename = typename TEnableIf<TIsDerivedFrom<OtherType, T>::Value>::Type>
	FORCEINLINE TVoxelFutureValueImpl(const TSharedRef<OtherType>& Value)
		: FVoxelFutureValue(FVoxelRuntimePinValue::Make(Value))
	{
	}
	template<typename OtherType, typename = typename TEnableIf<TIsDerivedFrom<OtherType, T>::Value>::Type>
	FORCEINLINE TVoxelFutureValueImpl(const TSharedRef<const OtherType>& Value)
		: FVoxelFutureValue(FVoxelRuntimePinValue::Make(Value))
	{
	}

public:
	template<typename OtherType, typename = typename TEnableIf<TIsDerivedFrom<OtherType, T>::Value && !std::is_same_v<OtherType, T>>::Type>
	FORCEINLINE auto Get_CheckCompleted() const -> decltype(auto)
	{
		return FVoxelFutureValue::Get_CheckCompleted<OtherType>();
	}
	template<typename OtherType, typename = typename TEnableIf<TIsDerivedFrom<OtherType, T>::Value && !std::is_same_v<OtherType, T>>::Type>
	FORCEINLINE TSharedRef<const OtherType> GetShared_CheckCompleted() const
	{
		return FVoxelFutureValue::GetShared_CheckCompleted<OtherType>();
	}

public:
	FORCEINLINE auto Get_CheckCompleted() const -> decltype(auto)
	{
		return FVoxelFutureValue::Get_CheckCompleted<T>();
	}
	FORCEINLINE TSharedRef<const T> GetShared_CheckCompleted() const
	{
		return FVoxelFutureValue::GetShared_CheckCompleted<T>();
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template<typename T>
class TVoxelFutureValue<T, typename TEnableIf<!std::is_same_v<T, void>>::Type> : public TVoxelFutureValueImpl<T>
{
public:
	using TVoxelFutureValueImpl<T>::TVoxelFutureValueImpl;
};

template<typename T>
using TVoxelFutureValueType = typename TChooseClass<std::is_same_v<T, void>, FVoxelFutureValue, TVoxelFutureValue<T>>::Result;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

using FVoxelGetAllocatedSize = TVoxelUniqueFunction<int64(const FVoxelRuntimePinValue& Value)>;

using FVoxelComputeValue = TVoxelUniqueFunction<FVoxelFutureValue(const FVoxelQuery& Query)>;

template<typename T>
using TVoxelComputeValue = TVoxelUniqueFunction<TVoxelFutureValue<T>(const FVoxelQuery& Query)>;

template<typename T>
static constexpr bool VoxelPassByValue =
	TIsTriviallyDestructible<T>::Value ||
	std::is_same_v<T, FVoxelRuntimePinValue> ||
	(TIsDerivedFrom<T, FVoxelBuffer>::Value && !std::is_same_v<T, FVoxelBuffer>);