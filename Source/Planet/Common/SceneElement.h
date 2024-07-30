// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
#include <variant>

#include "CoreMinimal.h"
#include <GameplayTagContainer.h>

#include "GenerateType.h"
#include "BaseData.h"

#include "SceneElement.generated.h"

struct FTableRowUnit;
struct FTableRowUnit_WeaponExtendInfo;
struct FTableRowUnit_ActiveSkillExtendInfo;
class AToolUnitBase;
class AWeapon_Base;
class USkill_Consumable_Base;
class AConsumable_Base;
class IPlanetControllerInterface;

class USkill_Base;
class ACharacterBase;
class AHumanCharacter;

struct FSceneUnitContainer;

enum class ECharacterPropertyType : uint8;

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

#pragma region HoldItem
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

UCLASS(BlueprintType)
class PLANET_API UBasicUnit : public UObject
{
	GENERATED_BODY()

public:

	friend FSceneUnitContainer;

	using IDType = int32;

	UBasicUnit();

	virtual ~UBasicUnit();

	IDType GetID()const;

	FGameplayTag GetUnitType()const;

	TSoftObjectPtr<UTexture2D> GetIcon()const;

	// 
	FString GetUnitName()const;

protected:

	FTableRowUnit* GetTableRowUnit()const;

	UPROPERTY(Transient)
	FGameplayTag UnitType = FGameplayTag::EmptyTag;

private:

	IDType ID;

};

UCLASS(BlueprintType, Blueprintable)
class PLANET_API UCoinUnit : public UBasicUnit
{
	GENERATED_BODY()

public:

	friend FSceneUnitContainer;

	UCoinUnit();

	void AddCurrentValue(int32 val);

	int32 GetCurrentValue()const;

	TOnValueChangedCallbackContainer<int32> CallbackContainerHelper;

protected:
	
	int32 Num = 1;

};

UCLASS(BlueprintType, Blueprintable)
class PLANET_API UConsumableUnit : public UBasicUnit
{
	GENERATED_BODY()

public:

	friend FSceneUnitContainer;

	UConsumableUnit();

	void AddCurrentValue(int32 val);

	int32 GetCurrentValue()const;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Type")
	TSubclassOf<USkill_Consumable_Base> Skill_Consumable_Class;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TMap<ECharacterPropertyType, FBaseProperty>ModifyPropertyMap;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	UAnimMontage* HumanMontage = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	float Duration = 3.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	float PerformActionInterval = 1.f;

protected:
	
	TOnValueChangedCallbackContainer<int32> CallbackContainerHelper;

	int32 Num = 1;

};

UCLASS(BlueprintType, Blueprintable)
class PLANET_API UToolUnit : public UBasicUnit
{
	GENERATED_BODY()

public:

	friend FSceneUnitContainer;

	UToolUnit();

	int32 GetNum()const;

	int32 DamageDegree = 0;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ToolType")
	TSubclassOf<AToolUnitBase> ToolActorClass;

protected:

	int32 Num = 1;

};

UCLASS(BlueprintType, Blueprintable)
class PLANET_API UGourpmateUnit : public UBasicUnit
{
	GENERATED_BODY()

public:

	using FPawnType = AHumanCharacter;

	UGourpmateUnit();

	void InitialByCharactor(FPawnType* InCharacterPtr);

	FPawnType* PCPtr = nullptr;

	int32 Level = 1;

	FName Name;

protected:

};

UCLASS(BlueprintType, Blueprintable)
class PLANET_API USkillUnit : public UBasicUnit
{
	GENERATED_BODY()

public:

	friend FSceneUnitContainer;

	USkillUnit();

	int32 Level = 1;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "All Abilities")
	TSubclassOf<USkill_Base>SkillClass;

protected:

};

UCLASS(BlueprintType, Blueprintable)
class PLANET_API UPassiveSkillUnit : public USkillUnit
{
	GENERATED_BODY()

public:

	UPassiveSkillUnit();

	// 
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Addtional Element")
	TMap<EWuXingType, int32>AddtionalElementMap;

	FGuid PropertuModify_GUID = FGuid::NewGuid();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "All Abilities")
	TSubclassOf<USkill_Base>AddtionalSkillClass;

protected:

};

UCLASS(BlueprintType, Blueprintable)
class PLANET_API UActiveSkillUnit : public USkillUnit
{
	GENERATED_BODY()

public:

	UActiveSkillUnit();

	FTableRowUnit_ActiveSkillExtendInfo* GetTableRowUnit_ActiveSkillExtendInfo()const;

protected:

};

UCLASS(BlueprintType, Blueprintable)
class PLANET_API UTalentSkillUnit : public USkillUnit
{
	GENERATED_BODY()

public:

	UTalentSkillUnit();

protected:

};

UCLASS(BlueprintType, Blueprintable)
class PLANET_API UWeaponSkillUnit : public USkillUnit
{
	GENERATED_BODY()

public:

	UWeaponSkillUnit();

protected:

};

UCLASS(BlueprintType, Blueprintable)
class PLANET_API UWeaponUnit : public UBasicUnit
{
	GENERATED_BODY()

public:

	friend FSceneUnitContainer;

	UWeaponUnit();

	FTableRowUnit_WeaponExtendInfo* GetTableRowUnit_WeaponExtendInfo()const;

	int32 DamageDegree = 0;

	UPROPERTY(Transient)
	USkillUnit* FirstSkill = nullptr;

protected:

};

USTRUCT(BlueprintType)
struct FSceneUnitContainer
{
	GENERATED_USTRUCT_BODY()

	using FOnSkillUnitChanged = TCallbackHandleContainer<void(USkillUnit*, bool)>;

	using FOnToolUnitChanged = TCallbackHandleContainer<void(UToolUnit*)>;
	
	using FOnConsumableUnitChanged = TCallbackHandleContainer<void(UConsumableUnit*, bool, int32)>;

	using FOnCoinUnitChanged = TCallbackHandleContainer<void(UCoinUnit*, bool, int32)>;


	UBasicUnit* AddUnit(FGameplayTag UnitType, int32 Num);

	UBasicUnit* FindUnit(UBasicUnit::IDType ID);


	UCoinUnit* AddUnit_Coin(FGameplayTag UnitType, int32 Num);
	
	UCoinUnit* FindUnit_Coin(FGameplayTag UnitType);


	UConsumableUnit* AddUnit_Consumable(FGameplayTag UnitType, int32 Num = 1);

	void RemoveUnit_Consumable(UConsumableUnit*UnitPtr, int32 Num = 1);


	UToolUnit* AddUnit_ToolUnit(FGameplayTag UnitType);


	UWeaponUnit* AddUnit_Weapon(FGameplayTag UnitType);

	UWeaponUnit* FindUnit_Weapon(FGameplayTag UnitType);


	USkillUnit* AddUnit_Skill(FGameplayTag UnitType);

	USkillUnit* FindUnit_Skill(FGameplayTag UnitType);


	const TArray<UBasicUnit*>& GetSceneUintAry()const;

	const TMap<FGameplayTag, UCoinUnit*>& GetCoinUintAry()const;

	void AddUnit_Apending(FGameplayTag UnitType, FGuid Guid);

	void SyncApendingUnit(FGuid Guid);

	FOnSkillUnitChanged OnSkillUnitChanged;

	FOnToolUnitChanged OnToolUnitChanged;

	FOnConsumableUnitChanged OnConsumableUnitChanged;

	FOnCoinUnitChanged OnCoinUnitChanged;

private:

	FTableRowUnit* GetTableRowUnit(FGameplayTag UnitType)const;

	UPROPERTY()
	TArray<UBasicUnit*> SceneToolsAry;

	TMap<UBasicUnit::IDType, UBasicUnit*> SceneMetaMap;

	TMap<FGameplayTag, UConsumableUnit*> ConsumablesUnitMap;

	TMap<FGameplayTag, USkillUnit*> SkillUnitMap;
	
	TMap<FGameplayTag, UCoinUnit*> CoinUnitMap;

	TMap<FGuid, TMap<FGameplayTag, int32>> SkillUnitApendingMap;

};

#pragma endregion HoldingItems
