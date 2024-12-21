// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <GameplayTagContainer.h>
#include <Subsystems/GameInstanceSubsystem.h>
#include <Subsystems/EngineSubsystem.h>

#include "GameplayTagsLibrary.generated.h"

UCLASS()
class PLANET_API UGameplayTagsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static FGameplayTag BaseFeature_Send;

	static FGameplayTag BaseFeature_Received;

	static FGameplayTag BaseFeature_Respawn;

#pragma region Locomotio
	static FGameplayTag Jump;
	
	static FGameplayTag State_Locomotion_Run;

	static FGameplayTag Dash;

	static FGameplayTag Mount;

	static FGameplayTag DisMount;
	
	static FGameplayTag Affected;
	
	static FGameplayTag State_MoveToAttaclArea;
#pragma endregion

#pragma region MovementStateAbl
	static FGameplayTag MovementStateAble;

	static FGameplayTag MovementStateAble_CantJump;

	static FGameplayTag MovementStateAble_CantPathFollowMove;
	
	static FGameplayTag MovementStateAble_CantPlayerInputMove;
	
	static FGameplayTag MovementStateAble_CantRootMotion;
	
	static FGameplayTag MovementStateAble_CantRotation;

	// 浮空状态（被动
	static FGameplayTag MovementStateAble_IntoFly;
	
	static FGameplayTag MovementStateAble_Orient2Acce;
	
	static FGameplayTag MovementStateAble_UseCustomRotation;
#pragma endregion

#pragma region State Tag
	static FGameplayTag UsingConsumable;
	
	static FGameplayTag Ridering;
	
	static FGameplayTag State_NoPhy;
	
	static FGameplayTag State_Buff_Purify;

	static FGameplayTag State_Buff_SuperArmor;

	static FGameplayTag State_Buff_CantBeSlected;

	static FGameplayTag State_Buff_Stagnation;

	static FGameplayTag Debuff;
	
	static FGameplayTag State_Debuff_Fire;
	
	static FGameplayTag State_Debuff_Stun;
	
	static FGameplayTag State_Debuff_Charm;

	static FGameplayTag State_Debuff_Fear;
	
	static FGameplayTag State_Debuff_Silent;

	static FGameplayTag State_Debuff_Slow;

	static FGameplayTag State_Debuff_Suppress;
	
	static FGameplayTag RootMotion;

	// 浮空状态（主动
	static FGameplayTag FlyAway;
	
	static FGameplayTag KnockDown;
	
	static FGameplayTag State_RootMotion_Traction;
	
	static FGameplayTag TornadoTraction;
	
	static FGameplayTag MoveAlongSpline;

	static FGameplayTag State_RootMotion_MoveTo;

	static FGameplayTag DeathingTag;
	
	static FGameplayTag Respawning;
	
	static FGameplayTag InFightingTag;

	static FGameplayTag State_ReleasingSkill;

	static FGameplayTag State_ReleasingSkill_Continuous;

	static FGameplayTag State_ReleasingSkill_Active;

	static FGameplayTag State_ReleasingSkil_WeaponActive;
#pragma endregion

#pragma region Socket Tags
	static FGameplayTag ConsumableSocket;
	
	static FGameplayTag ConsumableSocket1;
	
	static FGameplayTag ConsumableSocket2;

	static FGameplayTag ConsumableSocket3;
	
	static FGameplayTag ConsumableSocket4;
	
	static FGameplayTag WeaponSocket;
	
	static FGameplayTag WeaponSocket_1;
	
	static FGameplayTag WeaponSocket_2;

	static FGameplayTag ActiveSocket;
	
	static FGameplayTag ActiveSocket_1;
	
	static FGameplayTag ActiveSocket_2;
	
	static FGameplayTag ActiveSocket_3;
	
	static FGameplayTag ActiveSocket_4;

	static FGameplayTag PassiveSocket_1;

	static FGameplayTag PassiveSocket_2;

	static FGameplayTag PassiveSocket_3;

	static FGameplayTag PassiveSocket_4;
	
	static FGameplayTag PassiveSocket_5;
	
	static FGameplayTag TalentSocket;
#pragma endregion

#pragma region ProxyTyp
	static FGameplayTag Proxy_Weapon;

	static FGameplayTag Proxy_Weapon_Axe;
	
	static FGameplayTag Proxy_Tool;

	static FGameplayTag Proxy_Tool_Axe;

	static FGameplayTag Proxy_Consumables;

	static FGameplayTag Proxy_Consumables_HP;

	static FGameplayTag Proxy_Consumables_PP;

	static FGameplayTag Proxy_Coin;

	static FGameplayTag Proxy_Coin_Regular;

	static FGameplayTag Proxy_Coin_RafflePermanent;

	static FGameplayTag Proxy_Coin_RaffleLimit;

	static FGameplayTag Proxy_Skill;

	static FGameplayTag Proxy_Skill_Weapon;

	static FGameplayTag Proxy_Skill_Weapon_Axe;

	static FGameplayTag Proxy_Skill_Weapon_HandProtection;

	static FGameplayTag Proxy_Skill_Weapon_RangeTest;
	
	static FGameplayTag Proxy_Skill_Weapon_Bow;

	static FGameplayTag Proxy_Skill_Weapon_FoldingFan;
	
	static FGameplayTag Proxy_Skill_Active;

	static FGameplayTag Proxy_Skill_Active_Switch;
	
	static FGameplayTag Proxy_Skill_Active_Switch_Test;
	
	static FGameplayTag Proxy_Skill_Active_Control;

	static FGameplayTag Proxy_Skill_Active_Displacement;

	static FGameplayTag Proxy_Skill_Active_GroupTherapy;

	static FGameplayTag Proxy_Skill_Active_ContinuousGroupTherapy;

	static FGameplayTag Proxy_Skill_Active_Tornado;

	static FGameplayTag Proxy_Skill_Active_FlyAway;

	static FGameplayTag Proxy_Skill_Active_Stun;

	static FGameplayTag Proxy_Skill_Active_Charm;

	static FGameplayTag Proxy_Skill_Passve;

	static FGameplayTag Proxy_Skill_Passve_ZMJZ;

	static FGameplayTag Proxy_Skill_Talent;
	
	static FGameplayTag Proxy_Skill_Talent_NuQi;

	static FGameplayTag Proxy_Skill_Talent_YinYang;

	static FGameplayTag Proxy_Character;

	static FGameplayTag Proxy_Character_Player;

	static FGameplayTag Proxy_Character_TestNPC_1;

#pragma endregion

	
	static FGameplayTag Skill_CanBeInterrupted;

	static FGameplayTag Skill_CanBeInterrupted_Stagnation;

	
	static FGameplayTag DataSource_Regular;

	// 人物身上自带的属性
	static FGameplayTag DataSource_Character;

	static FGameplayTag DataSource_EquipmentModify;

	static FGameplayTag DataSource_TalentModify;

	
	static FGameplayTag GEData_Duration;

	
	static FGameplayTag GEData_Info;

	
	static FGameplayTag GEData_ModifyDuration_Immediate;
	
	static FGameplayTag GEData_ModifyDuration_Temporary;
	
	static FGameplayTag GEData_ModifyDuration_RemoveTemporary;

	
	static FGameplayTag GEData_ModifyType_HP;
	
	static FGameplayTag GEData_ModifyType_Damage_Base;
	
	static FGameplayTag GEData_ModifyType_MoveSpeed;
	
};
