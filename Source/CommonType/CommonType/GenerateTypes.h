// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>

#include "CoreMinimal.h"

#include "GenerateTypes.generated.h"

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
	
	HP,
	PP,
	Mana,

	Shield,

	AD,
	AD_Penetration,
	AD_PercentPenetration,

	AP,
	AP_Penetration,
	AP_PercentPenetration,

	Resistance,
	GAPerformSpeed,
	Evade,
	HitRate,
	Toughness,
	CriticalHitRate,
	CriticalDamage,
	MoveSpeed,

	kMax,
};

UENUM()
enum class EDashDirection : uint8
{
	kForward,
	kBackward,
	kLeft,
	kRight,
};

enum class EAffectedDirection : uint8
{
	kNone,
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
	kBow,
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
enum class ECharacterStateType : uint8
{
	kActive,
	kEnd,
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

UENUM()
enum class EElementalType : uint8
{
	kMetal,
	kWood,
	kWater,
	kFire,
	kEarth,
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

UENUM(BlueprintType)
enum class EPointSkillType :uint8
{
	kNuQi,
	kYinYang,
	kDuXing,
	kFaLi,
	kGongMing,
};

UENUM(BlueprintType)
enum class EPointPropertyType :uint8
{
	// + 1X AD、3x AD穿透
	// + 0.1 AD 结算修正
	kLiDao,
	// + 50X HP
	// + 0.1 HP 结算修正
	kGenGu,
	// + 0.1 闪避 结算修正
	kShenFa,
	// + 0.1 命中、会心率 结算修正
	kDongCha,
	// + 0.1 AP 结算修正
	kTianZi,
};

UENUM(BlueprintType)
enum class EPointType :uint8
{
	kNone,
	kSkill,
	kProperty,
};

USTRUCT(BlueprintType ,Blueprintable)
struct COMMONTYPE_API FProductsForSale
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Num = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Value = 0;
	
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