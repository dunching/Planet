// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <GameplayTagContainer.h>
#include <Subsystems/GameInstanceSubsystem.h>
#include <Subsystems/EngineSubsystem.h>

#include "GameplayTagsSubSystem.generated.h"

UCLASS()
class PLANET_API UGameplayTagsSubSystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:

	static UGameplayTagsSubSystem* GetInstance();
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "BaseFeature Tag")
	FGameplayTag BaseFeature_Send =
		FGameplayTag::RequestGameplayTag(FName(TEXT("BaseFeature.Send")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "BaseFeature Tag")
	FGameplayTag BaseFeature_Received =
		FGameplayTag::RequestGameplayTag(FName(TEXT("BaseFeature.Received")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "BaseFeature Tag")
	FGameplayTag BaseFeature_Respawn =
		FGameplayTag::RequestGameplayTag(FName(TEXT("BaseFeature.Respawn")));
	
#pragma region Locomotion
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "BaseFeature Tag")
	FGameplayTag Jump =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.Locomotion.Jump")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities Tag")
	FGameplayTag State_Locomotion_Run =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.Locomotion.Run")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities Tag")
	FGameplayTag Dash =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.Locomotion.Dash")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities Tag")
	FGameplayTag Mount =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.Locomotion.Mount")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities Tag")
	FGameplayTag DisMount =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.Locomotion.DisMount")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities Tag")
	FGameplayTag Affected =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.Locomotion.Affected")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities Tag")
	FGameplayTag State_MoveToAttaclArea =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.MoveToAttaclArea")));
#pragma endregion 
	
#pragma region MovementStateAble
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FGameplayTag MovementStateAble =
		FGameplayTag::RequestGameplayTag(FName(TEXT("MovementStateAble")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FGameplayTag MovementStateAble_CantJump =
		FGameplayTag::RequestGameplayTag(FName(TEXT("MovementStateAble.CantJump")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FGameplayTag MovementStateAble_CantPathFollowMove =
		FGameplayTag::RequestGameplayTag(FName(TEXT("MovementStateAble.CantPathFollowMove")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FGameplayTag MovementStateAble_CantPlayerInputMove =
		FGameplayTag::RequestGameplayTag(FName(TEXT("MovementStateAble.CantPlayerInputMove")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FGameplayTag MovementStateAble_CantRootMotion =
		FGameplayTag::RequestGameplayTag(FName(TEXT("MovementStateAble.CantRootMotion")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FGameplayTag MovementStateAble_CantRotation =
		FGameplayTag::RequestGameplayTag(FName(TEXT("MovementStateAble.CantRotation")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FGameplayTag MovementStateAble_IntoFly =
		FGameplayTag::RequestGameplayTag(FName(TEXT("MovementStateAble.IntoFly")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FGameplayTag MovementStateAble_Orient2Acce =
		FGameplayTag::RequestGameplayTag(FName(TEXT("MovementStateAble.Orient2Acce")));
#pragma endregion 
	
#pragma region State Tags
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "State Tag")
	FGameplayTag UsingConsumable =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.IsUsingConsumable")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "State Tag")
	FGameplayTag Ridering =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.Ridering")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "State Tag")
	FGameplayTag State_NoPhy =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.NoPhy")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "State Tag")
	FGameplayTag State_Buff_Purify =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.Buff.Purify")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "State Tag")
	FGameplayTag State_Buff_SuperArmor =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.Buff.SuperArmor")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "State Tag")
	FGameplayTag State_Buff_CantBeSlected =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.Buff.CantBeSelected")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "State Tag")
	FGameplayTag State_Buff_Stagnation =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.Buff.Stagnation")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "State Tag")
	FGameplayTag Debuff =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.Debuff")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "State Tag")
	FGameplayTag State_Debuff_Fire =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.Debuff.Fire")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "State Tag")
	FGameplayTag State_Debuff_Stun =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.Debuff.Stun")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "State Tag")
	FGameplayTag State_Debuff_Charm =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.Debuff.Charm")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "State Tag")
	FGameplayTag State_Debuff_Fear =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.Debuff.Fear")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "State Tag")
	FGameplayTag State_Debuff_Silent =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.Debuff.Silent")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "State Tag")
	FGameplayTag State_Debuff_Slow =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.Debuff.Slow")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "State Tag")
	FGameplayTag RootMotion =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.RootMotion")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "State Tag")
	FGameplayTag FlyAway =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.RootMotion.FlyAway")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "State Tag")
	FGameplayTag KnockDown =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.RootMotion.KnockDown")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "State Tag")
	FGameplayTag State_RootMotion_Traction =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.RootMotion.Traction")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "State Tag")
	FGameplayTag TornadoTraction =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.RootMotion.TornadoTraction")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "State Tag")
	FGameplayTag MoveAlongSpline =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.RootMotion.MoveAlongSpline")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "State Tag")
	FGameplayTag State_RootMotion_MoveTo =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.RootMotion.MoveTo")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "State Tag")
	FGameplayTag DeathingTag = 
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.Deathing")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "State Tag")
	FGameplayTag Respawning =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.Respawning")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "State Tag")
	FGameplayTag InFightingTag = 
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.InFighting")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "State Tag")
	FGameplayTag State_ReleasingSkill=
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.ReleasingSkill")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "State Tag")
	FGameplayTag State_ReleasingSkill_Continuous =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.ReleasingSkill.Continuous")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "State Tag")
	FGameplayTag State_ReleasingSkill_Active =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.ReleasingSkill.Active")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "State Tag")
	FGameplayTag State_ReleasingSkil_WeaponActive =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.ReleasingSkill.WeaponActive")));
#pragma endregion 

#pragma region Socket Tags
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI Socket Tag")
	FGameplayTag ConsumableSocket =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.ConsumableSocket")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI Socket Tag")
	FGameplayTag ConsumableSocket1 =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.ConsumableSocket.1")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI Socket Tag")
	FGameplayTag ConsumableSocket2 =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.ConsumableSocket.2")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI Socket Tag")
	FGameplayTag ConsumableSocket3 =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.ConsumableSocket.3")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI Socket Tag")
	FGameplayTag ConsumableSocket4 =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.ConsumableSocket.4")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI Socket Tag")
	FGameplayTag WeaponSocket =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.WeaponSocket")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI Socket Tag")
	FGameplayTag WeaponSocket_1 =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.WeaponSocket.1")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI Socket Tag")
	FGameplayTag WeaponSocket_2 =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.WeaponSocket.2")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI Socket Tag")
	FGameplayTag WeaponActiveSocket =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.WeaponActiveSocket")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI Socket Tag")
	FGameplayTag WeaponActiveSocket_1 =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.WeaponActiveSocket.1")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI Socket Tag")
	FGameplayTag WeaponActiveSocket_2 =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.WeaponActiveSocket.2")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI Socket Tag")
	FGameplayTag ActiveSocket =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.ActiveSocket")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI Socket Tag")
	FGameplayTag ActiveSocket_1 =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.ActiveSocket.1")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI Socket Tag")
	FGameplayTag ActiveSocket_2 =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.ActiveSocket.2")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI Socket Tag")
	FGameplayTag ActiveSocket_3 =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.ActiveSocket.3")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI Socket Tag")
	FGameplayTag ActiveSocket_4 =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.ActiveSocket.4")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI Socket Tag")
	FGameplayTag PassiveSocket_1 =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.PassiveSocket.1")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI Socket Tag")
	FGameplayTag PassiveSocket_2 =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.PassiveSocket.2")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI Socket Tag")
	FGameplayTag PassiveSocket_3 =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.PassiveSocket.3")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI Socket Tag")
	FGameplayTag PassiveSocket_4 =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.PassiveSocket.4")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI Socket Tag")
	FGameplayTag PassiveSocket_5 =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.PassiveSocket.5")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI Socket Tag")
	FGameplayTag TalentSocket =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Socket.SkillSocket.Talent")));
#pragma endregion 

#pragma region UnitType
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UnitType")
	FGameplayTag Unit_Weapon =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Weapon")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UnitType")
	FGameplayTag Unit_Weapon_Axe =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Weapon.Axe")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UnitType")
	FGameplayTag Unit_Tool =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Tool")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UnitType")
	FGameplayTag Unit_Tool_Axe =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Tool.Axe")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UnitType")
	FGameplayTag Unit_Consumables =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Consumables")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UnitType")
	FGameplayTag Unit_Consumables_HP =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Consumables.HP")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UnitType")
	FGameplayTag Unit_Consumables_PP =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Consumables.PP")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UnitType")
	FGameplayTag Unit_Coin =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Coin")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UnitType")
	FGameplayTag Unit_Coin_Regular =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Coin.Regular")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UnitType")
	FGameplayTag Unit_Coin_RafflePermanent =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Coin.RafflePermanent")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UnitType")
	FGameplayTag Unit_Coin_RaffleLimit =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Coin.RaffleLimit")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UnitType")
	FGameplayTag Unit_Skill =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UnitType")
	FGameplayTag Unit_Skill_Weapon =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Weapon")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UnitType")
	FGameplayTag Unit_Skill_Weapon_Axe =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Weapon.Axe")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UnitType")
	FGameplayTag Unit_Skill_Weapon_HandProtection =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Weapon.HandProtection")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UnitType")
	FGameplayTag Unit_Skill_Weapon_RangeTest =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Weapon.RangeTest")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UnitType")
	FGameplayTag Unit_Skill_Weapon_Bow =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Weapon.Bow")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UnitType")
	FGameplayTag Unit_Skill_Weapon_FoldingFan =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Weapon.FoldingFan")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UnitType")
	FGameplayTag Unit_Skill_Active =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Active")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UnitType")
	FGameplayTag Unit_Skill_Active_Switch=
		FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Active.Switch")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UnitType")
	FGameplayTag Unit_Skill_Active_Switch_Test =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Active.Switch.Test")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UnitType")
	FGameplayTag Unit_Skill_Active_Control =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Active.Control")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UnitType")
	FGameplayTag Unit_Skill_Active_Displacement =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Active.Displacement")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UnitType")
	FGameplayTag Unit_Skill_Active_GroupTherapy =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Active.GroupTherapy")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UnitType")
	FGameplayTag Unit_Skill_Active_ContinuousGroupTherapy =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Active.ContinuousGroupTherapy")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UnitType")
	FGameplayTag Unit_Skill_Active_Tornado =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Active.Tornado")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UnitType")
	FGameplayTag Unit_Skill_Active_FlyAway =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Active.FlyAway")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UnitType")
	FGameplayTag Unit_Skill_Active_Stun =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Active.Stun")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UnitType")
	FGameplayTag Unit_Skill_Active_Charm =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Active.Charm")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UnitType")
	FGameplayTag Unit_Skill_Passve =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Passve")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UnitType")
	FGameplayTag Unit_Skill_Passve_ZMJZ =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Passve.ZMJZ")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UnitType")
	FGameplayTag Unit_Skill_Talent =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Talent")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UnitType")
	FGameplayTag Unit_Skill_Talent_NuQi =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Talent.NuQi")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UnitType")
	FGameplayTag Unit_Skill_Talent_YinYang =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Talent.YinYang")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UnitType")
	FGameplayTag Unit_GroupMate=
		FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.GroupMate")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UnitType")
	FGameplayTag Unit_GroupMate_Player =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.GroupMate.Player")));

#pragma endregion 
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "DataSource")
	FGameplayTag DataSource_Regular =
		FGameplayTag::RequestGameplayTag(FName(TEXT("DataSource.Regular")));

	// 人物身上自带的属性
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "DataSource")
	FGameplayTag DataSource_Character =
		FGameplayTag::RequestGameplayTag(FName(TEXT("DataSource.Character")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "DataSource")
	FGameplayTag DataSource_EquipmentModify =
		FGameplayTag::RequestGameplayTag(FName(TEXT("DataSource.EquipmentModify")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "DataSource")
	FGameplayTag DataSource_TalentModify =
		FGameplayTag::RequestGameplayTag(FName(TEXT("DataSource.TalentModify")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "DataSource")
	FGameplayTag Skill_CanBeInterrupted=
		FGameplayTag::RequestGameplayTag(FName(TEXT("Skill.CanBeInterrupted")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "DataSource")
	FGameplayTag Skill_CanBeInterrupted_Stagnation =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Skill.CanBeInterrupted.Stagnation")));
};