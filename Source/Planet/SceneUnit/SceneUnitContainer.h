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

FTableRowUnit* GetTableRowUnit(FGameplayTag UnitType);

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

	~FSceneUnitContainer();

	TSharedPtr<FBasicProxy> AddUnit(FGameplayTag UnitType, int32 Num);

	TSharedPtr<FBasicProxy> FindUnit(IDType ID);

	void RemoveUnit(IDType ID);

	TSharedPtr<FCoinProxy> AddUnit_Coin(FGameplayTag UnitType, int32 Num);
	
	TSharedPtr<FCoinProxy> FindUnit_Coin(FGameplayTag UnitType);

	TSharedPtr <FConsumableProxy> AddUnit_Consumable(FGameplayTag UnitType, int32 Num = 1);

	void RemoveUnit_Consumable(const TSharedPtr <FConsumableProxy>&UnitPtr, int32 Num = 1);

	TSharedPtr<FToolProxy> AddUnit_ToolUnit(FGameplayTag UnitType);

	TSharedPtr<FWeaponProxy> AddUnit_Weapon(FGameplayTag UnitType);

	TSharedPtr<FWeaponProxy> FindUnit_Weapon(FGameplayTag UnitType);

	TSharedPtr<FSkillProxy>  AddUnit_Skill(FGameplayTag UnitType);

	TSharedPtr<FSkillProxy> FindUnit_Skill(FGameplayTag UnitType);

	void AddUnit_Groupmate(const TSharedPtr<FCharacterProxy>& UnitPtr);

	const TArray<TSharedPtr<FBasicProxy>>& GetSceneUintAry()const;

	const TMap<FGameplayTag, TSharedPtr<FCoinProxy>>& GetCoinUintAry()const;

	TArray<TSharedPtr<FCharacterProxy>> GetGourpmateUintAry()const;

	FOnSkillUnitChanged OnSkillUnitChanged;

	FOnToolUnitChanged OnToolUnitChanged;

	FOnConsumableUnitChanged OnConsumableUnitChanged;

	FOnCoinUnitChanged OnCoinUnitChanged;

	FOnGroupmateUnitChanged OnGroupmateUnitChanged;

private:

	int32 GetValidID()const;

	// 自定义的“排列方式”
	TArray<TSharedPtr<FBasicProxy>> SceneToolsAry;
	
	TMap<IDType, TSharedPtr<FBasicProxy>> SceneMetaMap;
	
	TMap<FGameplayTag, TSharedPtr<FConsumableProxy>> ConsumablesUnitMap;
	
	TMap<FGameplayTag, TSharedPtr<FSkillProxy>> SkillUnitMap;
	
	TMap<FGameplayTag, TSharedPtr<FCoinProxy>> CoinUnitMap;
};

#pragma endregion HoldingItems
