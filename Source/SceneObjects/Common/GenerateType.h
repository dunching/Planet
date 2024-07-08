// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
#include <variant>
#include <map>

#include "CoreMinimal.h"

#include "GenerateType.generated.h"

class ABuildingBase;
class UPlanetGameplayAbility;
class UPlanetGameplayAbility_SkillBase;

enum class EDashDirection : uint8
{
	kForward,
	kBackward,
	kLeft,
	kRight,
};

UENUM(BlueprintType)
enum class EAnimLinkClassType : uint8
{
	kUnarmed,
	kPickAxe,
};

UENUM(BlueprintType)
enum class ECharacterCampType : uint8
{
	kTeamMate,
	kEnemy,
	kNeutral,
};

UENUM(BlueprintType)
enum class EGroupMateChangeType : uint8
{
	kAdd,
	kRemove,
};

UENUM(BlueprintType)
enum class ETagChangeType : uint8
{
	kAdd,
	kRemove,
};

UENUM(BlueprintType)
enum class ETeammateOption : uint8
{
	// 
	kFollow,
	kAssistance,

	// Test
	kTest,

	//
	kEnemy,
};

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

	void ValueChanged(ValueType OldValue, ValueType NewValue);

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
void TOnValueChangedCallbackContainer<ValueType>::ValueChanged(ValueType OldValue, ValueType NewValue)
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

	void ExcuteCallback(ParamTypes...Args);

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
void TCallbackHandleContainer<Ret(ParamTypes...)>::ExcuteCallback(ParamTypes...Args)
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

UENUM(BlueprintType)
enum class EInputProcessorType : uint8
{
	kNone,

	kHorseProcessor,
	kHorseViewBackpackProcessor,
	kHorseAICtrlProcessor,
	kHorseRegularProcessor,

	kHumanProcessor,
	kHumanRegularProcessor,
	kHumanViewBackpackProcessor,
	kPigInteractionUI,
	kPlaceBuildingArea,
	kPlaceFortState,
	kPlaceTrackState,
	kPlaceTrackVehicleState,
	kPlaceGroundState,
	kPlaceWallState,
	kPlacePillarState,
	kPlaceCellState,
	kPlaceRoofState,
	kPlaceStairState,
	kPlaceWallWithDoorState,
	kPlaceDoorState,
};

enum class ETrackVehicleState
{
	kFowward,
	kBackward,
	kStop,
};

enum class ETrackVehiclePosState
{
	kFront,
	kBack,
};

enum class FBearerState
{
	kHaveVehicleTrack,
	kNoVehicleTrack,
};

UENUM(BlueprintType)
enum class EAttackState : uint8
{
	kNone,

	kNeutral,			// 中立的
	kActive,			// 主动(被联合)
	kPassive,			// 被动(被联合)
	kCount,
};

UENUM(BlueprintType)
enum class EFactionType : uint8
{
	kNone,

	kNeutral,		// 中立的
	kSelf,			// 玩家的
	kEnemy,			// 敌对的
	kCount,
};

UENUM(BlueprintType)
enum class EAICMDType : uint8
{
	kNone,

	kCollectItem,
	kAttack,
	kRunaway,
	kFollow,
	kPatrol,
	kMoveTo,
	kCount,
};

UENUM(BlueprintType)
enum class EJointType : uint8
{
	kNone,
	kDirect,			// 直接驯服
	kFeed,				// 喂食
	kStun,				// 击晕
	kJointTypeCount,
};

UENUM(BlueprintType)
enum class EPlayerPawnType : uint8
{
	// Pawn Begin
	kPigPawn,
	kHumanPawn,
	// Pawn Begin
};

UENUM(BlueprintType)
enum class EMaterialInstanceType : uint8
{
	// 材质实例 Begin
	kPlacePreviewMatIns_Success,
	kPlacePreviewMatIns_Fail,
	// 材质实例 Begin
};

UENUM(BlueprintType)
enum class EMaterialParamNameType : uint8
{
	// 材质參數名 Begin
	kPlacePreviewMatParam,
	kDestroyProgressColorNameParam,
	// 材质參數名 Begin
};

UENUM(BlueprintType)
enum class EMaterialParamType : uint8
{
	// 材质參數 Begin
	kPlacePreviewMatParamVectorSuc,
	kPlacePreviewMatParamVectorFail,
	// 材质參數 Begin
};

UENUM(BlueprintType)
enum class EVehicleType : uint8
{
	// 载具类 Begin
	kVechicleArticle,

	kNormalCarArticle,			// 车
	kTrackVehicleArticle,		// 履带车
	// 载具类 End
};

// 物品所处的插槽位置
UENUM(BlueprintType)
enum class EItemSocketType : uint8
{
	// 手上的插槽 Begin
	kSkeletalHandSock,

	kSkeletalLeftHandSock,		// 左手
	kSkeletalRightHandSock,		// 右手
	// 手上的插槽 End


	// 脚上的插槽 Begin
	kSkeletalLeftFootSock,		// 左脚
	kSkeletalRightFootSock,		// 右脚
	// 脚上的插槽 End

	// 头上的插槽 Begin	
	kSkeletalHeadSock,			// 头顶
	// 头上的插槽 End	

	kNone, // 计数
};

UENUM(BlueprintType)
enum class EPickType : uint8
{
	kNone,

	kRawMaterial,			// 被攻击拾取
	kInteraction,		// 交互拾取
	kCount,
};

enum class EMainUIType
{
	kLeftBackpackUI,
	kRightBackpackUI,
	kEquipUI,
};

UENUM(BlueprintType)
enum class EEquipmentActionType : uint8
{
	kStartAction,
	kStopAction,
};

UENUM(BlueprintType)
enum class EAnimCMDType : uint8
{
	kNone,

	kNormal,
	kBow,
	kM4A1,
	kTorch,
	kKnife,
	kAxe,

	kStartAttack,
	kStopAttack,
	kAttckOnce,
};

UENUM(BlueprintType)
enum class EAnimationNotify : uint8
{
	kNone,

	kAttackBegin,
	kAttackEnd,
};

enum ECMDType
{
	kCMDTypeNone,

	kTypeAttck_LeftBtnRelease,
	kTypeAttck_LeftBtnPressed,

	kTypeAttck_RightBtnRelease,
	kTypeAttck_RightBtnPressed,
	kTypeStopAttck,

	kTypeInteraction_LeftBtnRelease,
	kTypeInteraction_LeftBtnPressed,

	kTypeInteraction_RightBtnRelease,
	kTypeInteraction_RightBtnPressed,
	kTypeStopInteraction,

	kStartCrounch,
	kStopCrounch,

	kStartJump,
	kStopJump,

	kTypeCount,
};

enum EActionType
{
	kStartAttack,
	kStopAttack,

	kStartPlace,
	kStopPlace,

	kStartPlacing,
	kStoplacing,

	kStartAim,
	kStopAim,

	kActionCount,
};

UENUM(BlueprintType)
enum EInteractionType
{
	kInteractionNone,
	kUnion,
	kTheft,
	kInteractionCount,
};

enum class EBuildingState : uint8
{
	kRegular,
	kStartCapture,
	kStopCapture,
	kPreviewPlacing,
	kPreviewPlacing_Sucess,
	kPreviewPlacing_Fail,
	kPlaced,
};

enum class EItemChangeType
{
	kAdd,
	kSub,
};

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

