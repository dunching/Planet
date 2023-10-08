// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <functional>
#include "GenerateType.generated.h"

class AActor;

class ABuildingBase;

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
enum class EEquipmentType : uint8
{
	kNone,
	// 人形持有的装备 Begin
	kCharacterEquipment,
	kM4A1Equipment,				// M4A1
	kHandEquipment,				// 手上的、空手时
	kPistolEquipment,			// 手枪

	kKnifeEquipment,			// 刀

	kTorchEquipment,			// 火把
	kAxeEquipment,				// 斧头
	// 人形持有的装备 End

	// 犬类持有的装备 Begin
	kDogsEquiment,
	// 犬类持有的装备 End

	// 犬类持有的装备 Begin
	kPigEquiment,
	// 犬类持有的装备 End

	// 鸟类持有的装备 Begin
	kBirdEquiment,
	// 鸟类持有的装备 End
	// 
	// 狼类持有的装备 Begin
	kWolfEquiment,
	// 狼类持有的装备 End
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
enum class ERawMaterialType : uint8
{
	kNone,
	kTree,
	kStone,
	kGrass,
};

UENUM(BlueprintType)
enum class EBuildingType : uint8
{
	kNone,
	kTrack,
	kWoodCell,
	kWoodWall,
	kStoneWall,
	kWoodGroundStyle1,
	kWoodGroundStyle2,
	kStoneGround,
	kWoodRoof,
	kWoodStair,
	kWoodWallWithDoor,
	kWoodDoor,
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

enum class EItemChangeType
{
	kAdd,
	kSub,
};

enum class EOnItemChangeNotityObjType
{
	kLeftBackpackUI,
	kRightBackpackUI,
	kEquipUI,
};

UENUM(BlueprintType)
enum class EEnumtype : uint8
{
	kNone,
	kEquipment,
	kBuilding,
	kRawMaterialType,
};

UENUM(BlueprintType)
enum class EEquipmentActionType : uint8
{
	kStartAction1,
	kStopAction1,
	kStartAction2,
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
