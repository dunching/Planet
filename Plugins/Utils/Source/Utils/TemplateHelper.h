// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
#include <variant>
#include <map>

#include "CoreMinimal.h"

#include "TemplateHelper.generated.h"

#pragma region Callback
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

protected:

private:

	FCallbackIDType CallbackID = 0;

	TSharedPtr<FMapType> ContainerSPtr = MakeShared<FMapType>();

};

template<typename ValueType>
TOnValueChangedCallbackHandle<ValueType>::~TOnValueChangedCallbackHandle()
{
	UnBindCallback();
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

	void UnBindCallback();

	bool bIsAutoUnregister = true;

protected:

private:

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

	_NODISCARD TSharedPtr<FCallbackHandle> AddCallback(
		const typename FCallbackHandle::FCallbackType& Callback
	);

	void ExcuteCallback(ParamTypes...Args)const;

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

class ABuildingBase;
class UPlanetGameplayAbility;
class UPlanetGameplayAbility_SkillBase;

USTRUCT(BlueprintType)
struct FMaterialAry
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TArray<UMaterialInterface*>MaterialsAry;
};

struct FCapturesInfo
{
	struct FCaptureInfo
	{
		ABuildingBase* CapturePtPtr = nullptr;
	};

	TArray<TSharedPtr<FCaptureInfo>> CapturesInfoAry;
};

