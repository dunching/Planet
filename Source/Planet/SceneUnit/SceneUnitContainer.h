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
class UBasicUnit;
class USkillUnit;
class UCoinUnit;
class UConsumableUnit;
class UToolUnit;
class UCharacterUnit;
class USkillUnit;
class UWeaponUnit;

struct FSceneUnitContainer;

enum class ECharacterPropertyType : uint8;

#pragma region HoldItem
USTRUCT(BlueprintType)
struct FSceneUnitContainer final
{
	GENERATED_USTRUCT_BODY()

	using FOnSkillUnitChanged = TCallbackHandleContainer<void(USkillUnit*, bool)>;

	using FOnToolUnitChanged = TCallbackHandleContainer<void(UToolUnit*)>;
	
	using FOnGroupmateUnitChanged = TCallbackHandleContainer<void(UCharacterUnit*, bool)>;
	
	using FOnConsumableUnitChanged = TCallbackHandleContainer<void(UConsumableUnit*, bool, int32)>;

	using FOnCoinUnitChanged = TCallbackHandleContainer<void(UCoinUnit*, bool, int32)>;

	~FSceneUnitContainer();

	UBasicUnit* AddUnit(FGameplayTag UnitType, int32 Num);

	UBasicUnit* FindUnit(int32 ID);

	void RemoveUnit(int32 ID);


	UCoinUnit* AddUnit_Coin(FGameplayTag UnitType, int32 Num);
	
	UCoinUnit* FindUnit_Coin(FGameplayTag UnitType);


	UConsumableUnit* AddUnit_Consumable(FGameplayTag UnitType, int32 Num = 1);

	void RemoveUnit_Consumable(UConsumableUnit*UnitPtr, int32 Num = 1);


	UToolUnit* AddUnit_ToolUnit(FGameplayTag UnitType);


	UWeaponUnit* AddUnit_Weapon(FGameplayTag UnitType);

	UWeaponUnit* FindUnit_Weapon(FGameplayTag UnitType);


	USkillUnit* AddUnit_Skill(FGameplayTag UnitType);

	USkillUnit* FindUnit_Skill(FGameplayTag UnitType);


	UCharacterUnit* AddUnit_Groupmate(FGameplayTag UnitType);

	void AddUnit_Groupmate(UCharacterUnit* UnitPtr);


	const TArray<UBasicUnit*>& GetSceneUintAry()const;

	const TMap<FGameplayTag, UCoinUnit*>& GetCoinUintAry()const;

	TArray<UCharacterUnit*> GetGourpmateUintAry()const;

	FOnSkillUnitChanged OnSkillUnitChanged;

	FOnToolUnitChanged OnToolUnitChanged;

	FOnConsumableUnitChanged OnConsumableUnitChanged;

	FOnCoinUnitChanged OnCoinUnitChanged;

	FOnGroupmateUnitChanged OnGroupmateUnitChanged;

private:

	FTableRowUnit* GetTableRowUnit(FGameplayTag UnitType)const;
	
	// 自定义的“排列方式”
	UPROPERTY(Transient)
	TArray<UBasicUnit*> SceneToolsAry;
	
	UPROPERTY(Transient)
	TMap<int32, UBasicUnit*> SceneMetaMap;
	
	UPROPERTY(Transient)
	TMap<FGameplayTag, UConsumableUnit*> ConsumablesUnitMap;
	
	UPROPERTY(Transient)
	TMap<FGameplayTag, USkillUnit*> SkillUnitMap;
	
	UPROPERTY(Transient)
	TMap<FGameplayTag, UCoinUnit*> CoinUnitMap;
};

#pragma endregion HoldingItems
