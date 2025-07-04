// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <GameplayTagContainer.h>
#include <Subsystems/GameInstanceSubsystem.h>
#include <Subsystems/EngineSubsystem.h>

#include "GameplayTagsLibrary.generated.h"

UCLASS()
class GAMEPLAYTAGSLIBRARY_API UGameplayTagsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	static FString GetModifyItemTagName(const FGameplayTag&Tag);
	
	static FGameplayTag BaseFeature_Reply;

	static FGameplayTag BaseFeature_Dash;

	static FGameplayTag BaseFeature_Run;

	static FGameplayTag BaseFeature_Jump;

	static FGameplayTag BaseFeature_Respawn;

	static FGameplayTag BaseFeature_HasBeenAffected;

	static FGameplayTag BaseFeature_HasBeenRepel;

	static FGameplayTag BaseFeature_HasBeenFlyAway;

	static FGameplayTag BaseFeature_HasbeenTornodo;

	static FGameplayTag BaseFeature_HasbeenTraction;

	static FGameplayTag BaseFeature_HasBeenDisplacement;

	static FGameplayTag BaseFeature_HasBeenPull;

	static FGameplayTag BaseFeature_HasbeenSuppress;

	static FGameplayTag BaseFeature_MoveToLocation;

	static FGameplayTag BaseFeature_Dying;

	static FGameplayTag BaseFeature_SwitchWeapon;

#pragma region MovementStateAbl
	static FGameplayTag MovementStateAble;

	static FGameplayTag MovementStateAble_CantJump;

	static FGameplayTag MovementStateAble_CantPathFollowMove;

	static FGameplayTag MovementStateAble_CantPlayerInputMove;

	static FGameplayTag MovementStateAble_CantRootMotion;

	static FGameplayTag MovementStateAble_CantRotation_All;

	static FGameplayTag MovementStateAble_CantRotation_OrientToMovement;

	static FGameplayTag MovementStateAble_CantRotation_Controller;

	static FGameplayTag MovementStateAble_SkipSlideAlongSurface;

	static FGameplayTag MovementStateAble_SkipFlyingCheck;

	static FGameplayTag MovementStateAble_Orient2Acce;

	static FGameplayTag MovementStateAble_UseCustomRotation;
#pragma endregion

	static FGameplayTag Skill_CanBeInterrupted;

	static FGameplayTag Skill_CanBeInterrupted_Stagnation;

	static FGameplayTag Skill_CanBeInterrupted_Stun;

	static FGameplayTag Skill_Block_OtherSkill_Weapon;
	
	static FGameplayTag Skill_Block_OtherSkill_Active;
	
	static FGameplayTag Skill_Block_Displacement;

#pragma region State Tag
	static FGameplayTag UsingConsumable;

	static FGameplayTag Ridering;

	static FGameplayTag Respawning;

	static FGameplayTag InFightingTag;

	static FGameplayTag State_ActiveGuide_Challenge;

	static FGameplayTag State_Dying;

	static FGameplayTag State_Invisible;

	static FGameplayTag State_MoveToLocation;

	static FGameplayTag State_UsingCurrentWeapon;

	static FGameplayTag State_Running;

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

	static FGameplayTag State_ReleasingSkill;

	static FGameplayTag State_ReleasingSkill_Continuous;

	static FGameplayTag State_ReleasingSkill_Active;

	static FGameplayTag State_ReleasingSkil_WeaponActive;

	static FGameplayTag State_IgnoreLookInput;

	// 浮空状态（主动
	static FGameplayTag State_RootMotion;

	static FGameplayTag State_RootMotion_HasBeenFlyAway;

	static FGameplayTag State_RootMotion_FlyAway;

	static FGameplayTag State_RootMotion_KnockDown;

	static FGameplayTag State_RootMotion_Traction;

	static FGameplayTag State_RootMotion_TornadoTraction;

	static FGameplayTag State_RootMotion_MoveAlongSpline;

	static FGameplayTag State_RootMotion_MoveTo;
#pragma endregion

#pragma region Socket Tags
	static FGameplayTag ConsumableSocket;

	static FGameplayTag ConsumableSocket_1;

	static FGameplayTag ConsumableSocket_2;

	static FGameplayTag ConsumableSocket_3;

	static FGameplayTag ConsumableSocket_4;

	static FGameplayTag WeaponSocket;

	static FGameplayTag WeaponSocket_1;

	static FGameplayTag WeaponSocket_2;

	static FGameplayTag ActiveSocket;

	static FGameplayTag ActiveSocket_1;

	static FGameplayTag ActiveSocket_2;

	static FGameplayTag ActiveSocket_3;

	static FGameplayTag ActiveSocket_4;

	static FGameplayTag PassiveSocket_1;

	static FGameplayTag PassiveSocket;

	static FGameplayTag PassiveSocket_2;

	static FGameplayTag PassiveSocket_3;

	static FGameplayTag PassiveSocket_4;

	static FGameplayTag PassiveSocket_5;

	static FGameplayTag TalentSocket;
#pragma endregion

#pragma region ProxyTyp
	static FGameplayTag Proxy;

	
	static FGameplayTag Proxy_Weapon;

	static FGameplayTag Proxy_Weapon_Axe;

	static FGameplayTag Proxy_Weapon_Test;

	
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

	static FGameplayTag Proxy_Character_NPC_Functional;

	static FGameplayTag Proxy_Character_NPC_Functional_Dummy;

	static FGameplayTag Proxy_Character_NPC_Assistional;
	

	static FGameplayTag Proxy_Material;

	static FGameplayTag Proxy_Material_PassiveSkill_Experience_Book;

#pragma endregion



	// 人物身上自带的属性
	static FGameplayTag DataSource_Character;

	static FGameplayTag DataSource_EquipmentModify;

	static FGameplayTag DataSource_TalentModify;

	static FGameplayTag DataSource_Elemental_Metal;

	static FGameplayTag DataSource_Elemental_Wood;

	static FGameplayTag DataSource_Elemental_Water;

	static FGameplayTag DataSource_Elemental_Fire;

	static FGameplayTag DataSource_Elemental_Earth;

	static FGameplayTag DataSource_Reply;


	static FGameplayTag GEData_Duration;


	static FGameplayTag GEData_Info;

	static FGameplayTag GEData_CD;

	static FGameplayTag GEData_Damage;

	static FGameplayTag GEData_Damage_Callback;

	static FGameplayTag GEData_Damage_Callback_IsDeath;

	/**
	*	直接增加
	* 	SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyType_Permanent_Addtive);		
	*	SpecHandle.Data.Get()->SetSetByCallerMagnitude(UGameplayTagsLibrary::GEData_ModifyItem_Stamina, -Consume);
	*/
	static FGameplayTag GEData_ModifyType_Permanent_Addtive;

	/**
	 * 覆盖
	 */
	static FGameplayTag GEData_ModifyType_Permanent_Override;

	/**
	* 添加临时值
	* 增加一个临时量（比如修改移速时，先添加一个100的增速 再添加一个 20%增速，在移除这些增速是不好计算 所以用这种方式进行记录）
	* SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyType_Temporary);		
	* SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyItem_MoveSpeed);			// 修改的属性
	* SpecHandle.Data.Get()->SetSetByCallerMagnitude(														
	*													  UGameplayTagsLibrary::DataSource_TalentModify,	// 来源
	*													  TargetTalent.Value								// 值
	*													  );
	*/
	static FGameplayTag GEData_ModifyType_Temporary_Data_Addtive;
	
	static FGameplayTag GEData_ModifyType_Temporary_Data_Override;
	
	static FGameplayTag GEData_ModifyType_Temporary_Percent_Addtive;
	
	static FGameplayTag GEData_ModifyType_Temporary_Percent_Override;
	

	static FGameplayTag GEData_ModifyItem_Experience;

	static FGameplayTag GEData_ModifyItem_CriticalDamage;

	static FGameplayTag GEData_ModifyItem_CriticalHitRate;

	static FGameplayTag GEData_ModifyItem_HitRate;

	static FGameplayTag GEData_ModifyItem_EvadeRate;

	static FGameplayTag GEData_ModifyItem_MaxHP;

	static FGameplayTag GEData_ModifyItem_MaxStamina;

	static FGameplayTag GEData_ModifyItem_MaxMana;

	static FGameplayTag GEData_ModifyItem_HP;

	static FGameplayTag GEData_ModifyItem_Stamina;

	static FGameplayTag GEData_ModifyItem_Mana;

	static FGameplayTag GEData_ModifyItem_MoveSpeed;

	static FGameplayTag GEData_ModifyItem_PerformSpeed;

	static FGameplayTag GEData_ModifyItem_Shield;

	static FGameplayTag GEData_ModifyItem_Haste;

	static FGameplayTag GEData_ModifyItem_Metal_Value;

	static FGameplayTag GEData_ModifyItem_Metal_Level;

	static FGameplayTag GEData_ModifyItem_Metal_Penetration;

	static FGameplayTag GEData_ModifyItem_Metal_PercentPenetration;

	static FGameplayTag GEData_ModifyItem_Metal_Resistance;

	static FGameplayTag GEData_ModifyItem_Wood_Value;

	static FGameplayTag GEData_ModifyItem_Wood_Level;

	static FGameplayTag GEData_ModifyItem_Wood_Penetration;

	static FGameplayTag GEData_ModifyItem_Wood_PercentPenetration;

	static FGameplayTag GEData_ModifyItem_Wood_Resistance;

	static FGameplayTag GEData_ModifyItem_Water_Value;

	static FGameplayTag GEData_ModifyItem_Water_Level;

	static FGameplayTag GEData_ModifyItem_Water_Penetration;

	static FGameplayTag GEData_ModifyItem_Water_PercentPenetration;

	static FGameplayTag GEData_ModifyItem_Water_Resistance;

	static FGameplayTag GEData_ModifyItem_Fire_Value;

	static FGameplayTag GEData_ModifyItem_Fire_Level;

	static FGameplayTag GEData_ModifyItem_Fire_Penetration;

	static FGameplayTag GEData_ModifyItem_Fire_PercentPenetration;

	static FGameplayTag GEData_ModifyItem_Fire_Resistance;

	static FGameplayTag GEData_ModifyItem_Earth_Value;

	static FGameplayTag GEData_ModifyItem_Earth_Level;

	static FGameplayTag GEData_ModifyItem_Earth_Penetration;

	static FGameplayTag GEData_ModifyItem_Earth_PercentPenetration;

	static FGameplayTag GEData_ModifyItem_Earth_Resistance;

	static FGameplayTag GEData_ModifyItem_Damage_True;

	static FGameplayTag GEData_ModifyItem_Damage_Metal;

	static FGameplayTag GEData_ModifyItem_Damage_Wood;

	static FGameplayTag GEData_ModifyItem_Damage_Water;

	static FGameplayTag GEData_ModifyItem_Damage_Fire;

	static FGameplayTag GEData_ModifyItem_Damage_Earth;


	static FGameplayTag Region_Default;

	static FGameplayTag Region_NoviceVillage;
};