// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
#include <variant>

#include "CoreMinimal.h"
#include <GameplayTagContainer.h>

#include "GenerateType.h"
#include "BaseData.h"

#include "SceneUnitContainer.generated.h"

struct FTableRowUnit;
struct FTableRowUnit_WeaponExtendInfo;
struct FTableRowUnit_ActiveSkillExtendInfo;
struct FTableRowUnit_CharacterInfo;
class AToolUnitBase;
class AWeapon_Base;
class USkill_Consumable_Base;
class AConsumable_Base;
class IPlanetControllerInterface;

class USkill_Base;
class ACharacterBase;
class AHumanCharacter;
struct FBasicProxy;
struct FSkillProxy;
struct FCoinProxy;
struct FConsumableProxy;
struct FToolProxy;
struct FCharacterProxy;
struct FSkillProxy;
struct FWeaponProxy;

struct FSceneUnitContainer;

enum class ECharacterPropertyType : uint8;

FTableRowUnit* GetTableRowUnit(const FGameplayTag &UnitType);

#pragma region HoldItem
USTRUCT(BlueprintType)
struct FSceneUnitContainer final
{
	GENERATED_USTRUCT_BODY()

	using IDType = FGuid;

	using FOnSkillUnitChanged = TCallbackHandleContainer<void(const TSharedPtr<FSkillProxy>&, bool)>;

	using FOnToolUnitChanged = TCallbackHandleContainer<void(const TSharedPtr < FToolProxy>&)>;
	
	using FOnGroupmateUnitChanged = TCallbackHandleContainer<void(const TSharedPtr < FCharacterProxy> &, bool)>;
	
	using FOnConsumableUnitChanged = TCallbackHandleContainer<void(const TSharedPtr < FConsumableProxy>&, bool, int32)>;

	using FOnCoinUnitChanged = TCallbackHandleContainer<void(const TSharedPtr<FCoinProxy>&, bool, int32)>;
	
	using FOnWeaponUnitChanged = TCallbackHandleContainer<void(const TSharedPtr<FWeaponProxy>&, bool)>;

	~FSceneUnitContainer();

	TSharedPtr<FBasicProxy> AddUnit(const FGameplayTag &UnitType, int32 Num);

	TSharedPtr<FBasicProxy> FindUnit(IDType ID);

	void RemoveUnit(IDType ID);

	TSharedPtr<FCoinProxy> AddUnit_Coin(const FGameplayTag &UnitType, int32 Num);
	
	TSharedPtr<FCoinProxy> FindUnit_Coin(const FGameplayTag &UnitType);

	TSharedPtr <FConsumableProxy> AddUnit_Consumable(const FGameplayTag &UnitType, int32 Num = 1);

	void RemoveUnit_Consumable(const TSharedPtr <FConsumableProxy>&UnitPtr, int32 Num = 1);

	TSharedPtr<FToolProxy> AddUnit_ToolUnit(const FGameplayTag& UnitType);

	TSharedPtr<FWeaponProxy> AddUnit_Weapon(const FGameplayTag& UnitType);

	TSharedPtr<FWeaponProxy> Update_Weapon(const FWeaponProxy& Unit);

	TSharedPtr<FWeaponProxy> FindUnit_Weapon(const FGameplayTag& UnitType);

	TSharedPtr<FSkillProxy>  AddUnit_Skill(const FGameplayTag& UnitType);

	TSharedPtr<FSkillProxy> Update_Skill(const FSkillProxy& Unit);

	TSharedPtr<FSkillProxy> FindUnit_Skill(const FGameplayTag& UnitType);

	TSharedPtr<FSkillProxy> FindUnit_Skill(const IDType& ID)const;

	TSharedPtr<FCharacterProxy> AddUnit_Character(const FGameplayTag &UnitType);

	TSharedPtr<FCharacterProxy> Update_Character(const FCharacterProxy& Unit);

	const TArray<TSharedPtr<FBasicProxy>>& GetSceneUintAry()const;

	const TMap<FGameplayTag, TSharedPtr<FCoinProxy>>& GetCoinUintAry()const;

	TArray<TSharedPtr<FCharacterProxy>> GetGourpmateUintAry()const;
	
	TSharedPtr<FCharacterProxy> FindUnit_Character(const IDType& ID)const;

	FOnSkillUnitChanged OnSkillUnitChanged;

	FOnToolUnitChanged OnToolUnitChanged;

	FOnConsumableUnitChanged OnConsumableUnitChanged;

	FOnCoinUnitChanged OnCoinUnitChanged;

	FOnGroupmateUnitChanged OnGroupmateUnitChanged;

	FOnWeaponUnitChanged OnWeaponUnitChanged;

	TSharedPtr<FCharacterProxy>OwnerCharacter = nullptr;

private:

	int32 GetValidID()const;

	// 自定义的“排列方式”
	TArray<TSharedPtr<FBasicProxy>> SceneToolsAry;
	
	TMap<IDType, TSharedPtr<FBasicProxy>> SceneMetaMap;
	
	TMap<FGameplayTag, TSharedPtr<FConsumableProxy>> ConsumablesUnitMap;
	
	TMap<FGameplayTag, TSharedPtr<FSkillProxy>> SkillUnitMap;
	
	TMap<FGameplayTag, TSharedPtr<FCoinProxy>> CoinUnitMap;
	
	TMap<FGameplayTag, TSharedPtr<FCharacterProxy>> CharacterCoinUnitMap;

};

#pragma endregion HoldingItems
