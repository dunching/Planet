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

#pragma region Locomotion
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities Tag")
	FGameplayTag Jump =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.Locomotion.Jump")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities Tag")
	FGameplayTag Running =
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
#pragma endregion 
	
#pragma region BaseFeature Tags
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities Tag")
	FGameplayTag MoveToAttaclArea =
		FGameplayTag::RequestGameplayTag(FName(TEXT("BaseFeature.MoveToAttaclArea")));
#pragma endregion 

#pragma region MovementStateAble
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FGameplayTag MovementStateAble =
		FGameplayTag::RequestGameplayTag(FName(TEXT("MovementStateAble")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FGameplayTag MovementStateAble_Jump =
		FGameplayTag::RequestGameplayTag(FName(TEXT("MovementStateAble.Jump")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FGameplayTag MovementStateAble_Move =
		FGameplayTag::RequestGameplayTag(FName(TEXT("MovementStateAble.Move")));
#pragma endregion 
	
#pragma region State Tags
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities Tag")
	FGameplayTag UsingConsumable =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.IsUsingConsumable")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities Tag")
	FGameplayTag Ridering =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.Ridering")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities Tag")
	FGameplayTag SuperArmor =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.Buff.SuperArmor")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities Tag")
	FGameplayTag Debuff =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.Debuff")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities Tag")
	FGameplayTag Stun =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.Debuff.Stun")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities Tag")
	FGameplayTag Silent =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.Debuff.Silent")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities Tag")
	FGameplayTag RootMotion =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.RootMotion")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities Tag")
	FGameplayTag FlyAway =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.RootMotion.FlyAway")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities Tag")
	FGameplayTag KnockDown =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.RootMotion.KnockDown")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities Tag")
	FGameplayTag Traction =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.RootMotion.Traction")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities Tag")
	FGameplayTag TornadoTraction =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.RootMotion.TornadoTraction")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities Tag")
	FGameplayTag MoveAlongSpline =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.RootMotion.MoveAlongSpline")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities Tag")
	FGameplayTag DeathingTag = 
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.Deathing")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities Tag")
	FGameplayTag InFightingTag = 
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.InFighting")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Gameplay Tag")
	FGameplayTag GameplayTag1 =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.ReleasingSkill.Continuous")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Gameplay Tag")
	FGameplayTag GameplayTag2 =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.ReleasingSkill.WeaponActive")));
#pragma endregion 

#pragma region UISocket Tags
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI Socket Tag")
	FGameplayTag ConsumableSocket1 =
		FGameplayTag::RequestGameplayTag(FName(TEXT("UI.SkillSocket.ConsumableSocket1")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI Socket Tag")
	FGameplayTag ConsumableSocket2 =
		FGameplayTag::RequestGameplayTag(FName(TEXT("UI.SkillSocket.ConsumableSocket2")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI Socket Tag")
	FGameplayTag ConsumableSocket3 =
		FGameplayTag::RequestGameplayTag(FName(TEXT("UI.SkillSocket.ConsumableSocket3")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI Socket Tag")
	FGameplayTag ConsumableSocket4 =
		FGameplayTag::RequestGameplayTag(FName(TEXT("UI.SkillSocket.ConsumableSocket4")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI Socket Tag")
	FGameplayTag WeaponActiveSocket1 =
		FGameplayTag::RequestGameplayTag(FName(TEXT("UI.SkillSocket.WeaponActiveSocket1")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI Socket Tag")
	FGameplayTag WeaponActiveSocket2 =
		FGameplayTag::RequestGameplayTag(FName(TEXT("UI.SkillSocket.WeaponActiveSocket2")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI Socket Tag")
	FGameplayTag ActiveSocket1 =
		FGameplayTag::RequestGameplayTag(FName(TEXT("UI.SkillSocket.ActiveSocket1")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI Socket Tag")
	FGameplayTag ActiveSocket2 =
		FGameplayTag::RequestGameplayTag(FName(TEXT("UI.SkillSocket.ActiveSocket2")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI Socket Tag")
	FGameplayTag ActiveSocket3 =
		FGameplayTag::RequestGameplayTag(FName(TEXT("UI.SkillSocket.ActiveSocket3")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI Socket Tag")
	FGameplayTag ActiveSocket4 =
		FGameplayTag::RequestGameplayTag(FName(TEXT("UI.SkillSocket.ActiveSocket4")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI Socket Tag")
	FGameplayTag TalentSocket =
		FGameplayTag::RequestGameplayTag(FName(TEXT("UI.SkillSocket.Talent")));
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
	FGameplayTag Unit_GroupMate =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.GroupMate")));
	
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
	FGameplayTag Unit_Skill_Active =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Active")));
	
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
#pragma endregion 
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "DataSource")
	FGameplayTag DataSource_Regular =
		FGameplayTag::RequestGameplayTag(FName(TEXT("DataSource.Regular")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "DataSource")
	FGameplayTag DataSource_Character =
		FGameplayTag::RequestGameplayTag(FName(TEXT("DataSource.Character")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "DataSource")
	FGameplayTag DataSource_EquipmentModify =
		FGameplayTag::RequestGameplayTag(FName(TEXT("DataSource.EquipmentModify")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "DataSource")
	FGameplayTag DataSource_TalentModify =
		FGameplayTag::RequestGameplayTag(FName(TEXT("DataSource.TalentModify")));
};