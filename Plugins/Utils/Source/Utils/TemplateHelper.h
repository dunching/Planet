// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
#include <variant>
#include <map>

#include "CoreMinimal.h"

#include "TemplateHelper.generated.h"

#pragma region 回调容器
template<typename ValueType>
class TOnValueChangedCallbackHandle final
{
public:

	using FCallbackIDType = int32;

	using FCallbackType = std::function<void(ValueType, ValueType)>;

	// 注意：这里使用TMap，有概率出现前四条保存的function失效，具体原因未知
	using FMapType = std::map<FCallbackIDType, FCallbackType>;

	TOnValueChangedCallbackHandle();

	TOnValueChangedCallbackHandle(FCallbackIDType InCallbackID, const TSharedPtr<FMapType>& InContainerSPtr);

	~TOnValueChangedCallbackHandle();

	void UnBindCallback();

	bool bIsAutoUnregister = true;

protected:

private:

	FCallbackIDType CallbackID = 0;

	TSharedPtr<FMapType> ContainerSPtr = MakeShared<FMapType>();

};

template<typename ValueType>
TOnValueChangedCallbackHandle<ValueType>::~TOnValueChangedCallbackHandle()
{
	if (bIsAutoUnregister)
	{
		UnBindCallback();
	}
}

template<typename ValueType>
TOnValueChangedCallbackHandle<ValueType>::TOnValueChangedCallbackHandle()
{
	ContainerSPtr = MakeShared<FMapType>();
}

template<typename ValueType>
TOnValueChangedCallbackHandle<ValueType>::TOnValueChangedCallbackHandle(
	FCallbackIDType InCallbackID, const TSharedPtr<FMapType>& InContainerSPtr
) :
	CallbackID(InCallbackID),
	ContainerSPtr(InContainerSPtr)
{

}

template<typename ValueType>
void TOnValueChangedCallbackHandle<ValueType>::UnBindCallback()
{
	if (ContainerSPtr->erase(CallbackID))
	{
	}
}

template<typename ValueType>
class TOnValueChangedCallbackContainer final
{
public:

	TOnValueChangedCallbackContainer<ValueType>& operator=(const TOnValueChangedCallbackContainer<ValueType>& RightValue);

	using FCallbackHandle = TOnValueChangedCallbackHandle<ValueType>;

	using FCallbackHandleSPtr = TSharedPtr<FCallbackHandle>;

	using FMapType = FCallbackHandle::FMapType;

	_NODISCARD TSharedPtr<FCallbackHandle> AddOnValueChanged(
		const typename FCallbackHandle::FCallbackType& Callback
	);

	void ValueChanged(ValueType OldValue, ValueType NewValue)const;

	TSharedPtr<FMapType> CallbacksMapSPtr = MakeShared<FMapType>();

private:

};

template<typename ValueType>
TOnValueChangedCallbackContainer<ValueType>& TOnValueChangedCallbackContainer<ValueType>::operator=(const TOnValueChangedCallbackContainer<ValueType>& RightValue)
{
	CallbacksMapSPtr = MakeShared<FMapType>();
	*CallbacksMapSPtr = *RightValue.CallbacksMapSPtr;

	return *this;
}

template<typename ValueType>
TSharedPtr<typename TOnValueChangedCallbackContainer<ValueType>::FCallbackHandle> TOnValueChangedCallbackContainer<ValueType>::AddOnValueChanged(
	const typename FCallbackHandle::FCallbackType& Callback
)
{
	if (CallbacksMapSPtr)
	{
		for (;;)
		{
			const auto ID = FMath::RandRange(1, std::numeric_limits<int32>::max());
			if (!CallbacksMapSPtr->contains(ID))
			{
				CallbacksMapSPtr->emplace(ID, Callback);

				return MakeShared<FCallbackHandle>(ID, CallbacksMapSPtr);
			}
		}
	}

	return nullptr;
}

template<typename ValueType>
void TOnValueChangedCallbackContainer<ValueType>::ValueChanged(ValueType OldValue, ValueType NewValue)const
{
	if (!CallbacksMapSPtr)
	{
		return;
	}
	const auto Temp = *CallbacksMapSPtr;
	for (auto Iter : Temp)
	{
		if (Iter.second)
		{
			Iter.second(OldValue, NewValue);
		}
	}
}

template<typename Ret, typename... ParamTypes>
class TCallbackHandle final
{
};

template<typename Ret, typename... ParamTypes>
class TCallbackHandle<Ret(ParamTypes...)> final
{
public:

	using FCallbackIDType = int32;

	using FCallbackType = std::function<Ret(ParamTypes...)>;

	using FMapType = std::map<FCallbackIDType, FCallbackType>;

	TCallbackHandle();

	TCallbackHandle(FCallbackIDType InCallbackID, const TSharedPtr<FMapType>& InContainerSPtr);

	~TCallbackHandle();

	bool bIsAutoUnregister = true;

protected:

private:

	void UnBindCallback();

	FCallbackIDType CallbackID = 0;

	TSharedPtr<FMapType> ContainerSPtr = MakeShared<FMapType>();

};

template<typename Ret, typename... ParamTypes>
TCallbackHandle<Ret(ParamTypes...)>::~TCallbackHandle()
{
	if (bIsAutoUnregister)
	{
		UnBindCallback();
	}
}

template<typename Ret, typename... ParamTypes>
TCallbackHandle<Ret(ParamTypes...)>::TCallbackHandle()
{

}

template<typename Ret, typename... ParamTypes>
TCallbackHandle<Ret(ParamTypes...)>::TCallbackHandle(
	FCallbackIDType InCallbackID, const TSharedPtr<FMapType>& InContainerSPtr
) :
	CallbackID(InCallbackID),
	ContainerSPtr(InContainerSPtr)
{

}

template<typename Ret, typename... ParamTypes>
void TCallbackHandle<Ret(ParamTypes...)>::UnBindCallback()
{
	if (ContainerSPtr->erase(CallbackID))
	{
	}
}

template<typename Ret, typename... ParamTypes>
class TCallbackHandleContainer
{};

template<typename Ret, typename... ParamTypes>
class TCallbackHandleContainer<Ret(ParamTypes...)> final
{
public:

	using FCallbackHandle = TCallbackHandle<Ret(ParamTypes...)>;

	using FCallbackHandleSPtr = TSharedPtr<FCallbackHandle>;

	using FMapType = FCallbackHandle::FMapType;

	const TCallbackHandleContainer<Ret(ParamTypes...)>& operator=(const TCallbackHandleContainer<Ret(ParamTypes...)>& RightValue);

	const TCallbackHandleContainer<Ret(ParamTypes...)>& operator()(ParamTypes...Args);

	const TCallbackHandleContainer<Ret(ParamTypes...)>& operator()(ParamTypes...Args)const;

	_NODISCARD TSharedPtr<FCallbackHandle> AddCallback(
		const typename FCallbackHandle::FCallbackType& Callback
	);

	void ExcuteCallback(ParamTypes...Args)const;

	TSharedPtr<FMapType> GetCallbacks()const;

	void AppendCallbacks(const TCallbackHandleContainer<Ret(ParamTypes...)>&Right);

private:

	TSharedPtr<FMapType> CallbacksMapSPtr = MakeShared<FMapType>();

};

template<typename Ret, typename... ParamTypes>
const TCallbackHandleContainer<Ret(ParamTypes...)>& TCallbackHandleContainer<Ret(ParamTypes...)>::operator=(const TCallbackHandleContainer<Ret(ParamTypes...)>& RightValue)
{
	CallbacksMapSPtr = MakeShared<FMapType>();
	*CallbacksMapSPtr = *RightValue.CallbacksMapSPtr;

	return *this;
}

template<typename Ret, typename... ParamTypes>
const TCallbackHandleContainer<Ret(ParamTypes...)>& TCallbackHandleContainer<Ret(ParamTypes...)>::operator()(ParamTypes...Args)
{
	if (CallbacksMapSPtr)
	{
		const auto Temp = *CallbacksMapSPtr;
		for (auto Iter : Temp)
		{
			if (Iter.second)
			{
				Iter.second(Args...);
			}
		}
	}

	return *this;
}

template<typename Ret, typename... ParamTypes>
const TCallbackHandleContainer<Ret(ParamTypes...)>& TCallbackHandleContainer<Ret(ParamTypes...)>::operator()(ParamTypes...Args)const
{
	if (CallbacksMapSPtr)
	{
		const auto Temp = *CallbacksMapSPtr;
		for (auto Iter : Temp)
		{
			if (Iter.second)
			{
				Iter.second(Args...);
			}
		}
	}

	return *this;
}

template<typename Ret, typename... ParamTypes>
void TCallbackHandleContainer<Ret(ParamTypes...)>::ExcuteCallback(ParamTypes...Args)const
{
	if (!CallbacksMapSPtr)
	{
		return;
	}
	const auto Temp = *CallbacksMapSPtr;
	for (auto Iter : Temp)
	{
		if (Iter.second)
		{
			Iter.second(Args...);
		}
	}
}

template<typename Ret, typename... ParamTypes>
TSharedPtr<typename TCallbackHandleContainer<Ret(ParamTypes...)>::FMapType> TCallbackHandleContainer<Ret(ParamTypes...)>::GetCallbacks() const
{
	return CallbacksMapSPtr;
}

template<typename Ret, typename... ParamTypes>
void TCallbackHandleContainer<Ret(ParamTypes...)>::AppendCallbacks(const TCallbackHandleContainer<Ret(ParamTypes...)>& Right) 
{
	for (const auto& Iter : *Right.CallbacksMapSPtr)
	{
		AddCallback(Iter.second);
	}
}

template<typename Ret, typename... ParamTypes>
TSharedPtr<typename TCallbackHandleContainer<Ret(ParamTypes...)>::FCallbackHandle> TCallbackHandleContainer<Ret(ParamTypes...)>::AddCallback(
	const typename FCallbackHandle::FCallbackType& Callback
)
{
	if (CallbacksMapSPtr)
	{
		for (;;)
		{
			const auto ID = FMath::RandRange(1, std::numeric_limits<int32>::max());
			if (!CallbacksMapSPtr->contains(ID))
			{
				CallbacksMapSPtr->emplace(ID, Callback);

				return MakeShared<FCallbackHandle>(ID, CallbacksMapSPtr);
			}
		}
	}

	return nullptr;
}
#pragma endregion Callback

template<typename InheritsType>
struct TStructVariable
{
	static InheritsType Get()
	{
		InheritsType InheritsTypeIns;

		return InheritsTypeIns;
	}
};

template<typename ChildType, typename ParentType>
TSharedPtr<ChildType> DynamicCastSharedPtr(const TSharedPtr<ParentType>& SPtr)
{
	return TSharedPtr<ChildType>(SPtr, dynamic_cast<ChildType*>(SPtr.Get()));
}

template<typename GameplayAbilityTargetDataType>
GameplayAbilityTargetDataType* DeepClone_GameplayAbilityTargetData(const GameplayAbilityTargetDataType* GameplayAbilityTargetDataPtr)
{
	auto ResultPtr = new GameplayAbilityTargetDataType;

	*ResultPtr = *GameplayAbilityTargetDataPtr;

	return ResultPtr;
}

template<typename GameplayAbilityTargetDataType, typename OtherType>
TSharedPtr<GameplayAbilityTargetDataType> MakeSPtr_GameplayAbilityTargetData(const OtherType* GameplayAbilityTargetDataPtr)
{
	auto ParentPtr = dynamic_cast<const GameplayAbilityTargetDataType*>(GameplayAbilityTargetDataPtr);

	auto ResultSPtr = TSharedPtr<GameplayAbilityTargetDataType>(ParentPtr->Clone());

	return ResultSPtr;
}

class ABuildingBase;
class UPlanetGameplayAbility;
class UPlanetGameplayAbility_SkillBase;

USTRUCT(BlueprintType)
struct FMaterialAry
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TArray<class UMaterialInterface*>MaterialsAry;
};

struct FCapturesInfo
{
	struct FCaptureInfo
	{
		ABuildingBase* CapturePtPtr = nullptr;
	};

	TArray<TSharedPtr<FCaptureInfo>> CapturesInfoAry;
};

