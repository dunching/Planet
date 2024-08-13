// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
#include <variant>
#include <map>

#include "CoreMinimal.h"

#include "TemplateHelper.h"

#include "GenerateType.generated.h"

UENUM(BlueprintType)
enum class ECharacterPropertyType : uint8
{
	LiDao,
	GenGu,
	ShenFa,
	DongCha,
	TianZi,

	GoldElement,
	WoodElement,
	WaterElement,
	FireElement,
	SoilElement,
	
	BaseAttackPower,
	Penetration,
	PercentPenetration,
	Resistance,
	GAPerformSpeed,
	kHP,
	kPP,
	Evade,
	HitRate,
	Toughness,
	CriticalHitRate,
	CriticalDamage,
	MoveSpeed,
};

enum class EDashDirection : uint8
{
	kForward,
	kBackward,
	kLeft,
	kRight,
};

enum class EAffectedDirection : uint8
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
	// Groupmate
	kFree,

	// Teammate
	kFollow,	// 跟随 
	kAssistance,// 协助
	kFireTarget,// 集火目标

	// 
	kEnemy,

	// Test
	kTest,
};

enum class EItemChangeType
{
	kAdd,
	kSub,
};

enum class EWeaponSocket
{
	kNone,
	kMain,
	kSecondary,
};

UENUM(BlueprintType)
enum class ETalent_State_Type : uint8
{
	kYin,
	kYang,
};

UENUM(BlueprintType)
enum class ERawMaterialsType : uint8
{
	kNone,
};

UENUM(BlueprintType)
enum class EBuildingsType : uint8
{
	kNone,
};

UENUM(BlueprintType)
enum class EWuXingType : uint8
{
	kGold,
	kWood,
	kWater,
	kFire,
	kSoil,
};

// 以前的？
#pragma region DEPRECATED
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

#pragma endregion  