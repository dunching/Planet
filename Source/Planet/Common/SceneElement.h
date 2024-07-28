// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
#include <variant>

#include "CoreMinimal.h"

#include "GenerateType.h"
#include "BaseData.h"

#include "SceneElement.generated.h"

struct FTableRowUnit;
class AToolUnitBase;
class USkill_Consumable_Base;
class AConsumable_Base;
class IPlanetControllerInterface;

class USkill_Base;
class ACharacterBase;
class AHumanCharacter;

struct FSceneToolsContainer;

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

UENUM(BlueprintType)
enum class ESceneToolsType : uint8
{
	kNone,

	kWeapon,
	kTool,
	kConsumables,
	kWeaponSkill,
	kActiveSkill,
	kPassveSkill,
	kTalentSkill,
	kGroupMate,
	kCoin,
};

UENUM(BlueprintType)
enum class ECoinUnitType : uint8
{
	kNone,

	// 
	kRegular,

	// ³£×¤³é
	kRafflePermanent,

	// ÏÞ¶¨³é
	kRaffleLimit,
};

UENUM(BlueprintType)
enum class EToolUnitType : uint8
{
	kNone,

	kPickAxe,
};

UENUM(BlueprintType)
enum class EConsumableUnitType : uint8
{
	kNone,

	kGeneric_HP,
	kGeneric_PP,
};

UENUM(BlueprintType)
enum class EWeaponUnitType : uint8
{
	kNone,

	kPickAxe,
	kWeaponHandProtection,
	kRangeTest,
};

UENUM(BlueprintType)
enum class ESkillUnitType : uint8
{
	kNone,

	kHumanSkill_Passive_ZMJZ,

	kHumanSkill_WeaponActive_PickAxe_Attack1,
	kHumanSkill_WeaponActive_HandProtection_Attack1,
	kHumanSkill_WeaponActive_RangeTest,

	kHumanSkill_Active_Displacement,
	kHumanSkill_Active_GroupTherapy,
	kHumanSkill_Active_ContinuousGroupTherapy,
	kHumanSkill_Active_Tornado,
	kHumanSkill_Active_FlyAway,

	kHumanSkill_Talent_NuQi,
	kHumanSkill_Talent_YinYang,
};

UENUM(BlueprintType)
enum class ESkillType : uint8
{
	kActive,
	kWeapon,
	kPassive,
	kTalent,
};

UCLASS(BlueprintType)
class PLANET_API UBasicUnit : public UObject
{
	GENERATED_BODY()

public:

	friend FSceneToolsContainer;

	using IDType = int32;

	UBasicUnit();

	virtual ~UBasicUnit();

	UBasicUnit(ESceneToolsType InSceneToolsType);

	IDType GetID()const;

	ESceneToolsType GetSceneToolsType()const;

	TSoftObjectPtr<UTexture2D> GetIcon()const;

	template<typename SceneElementType>
	SceneElementType GetSceneElementType()const;

	// 
	FString GetUnitName()const;

protected:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ToolsIcons")
	TSoftObjectPtr<UTexture2D> DefaultIcon;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ToolsIcons")
	FString UnitName = TEXT("UnitName");

	std::variant<
		EToolUnitType, 
		EWeaponUnitType,
		EConsumableUnitType,
		ESkillUnitType,
		ECoinUnitType
	> UnitType;

private:

	IDType ID;

	ESceneToolsType SceneToolsType = ESceneToolsType::kNone;

};

template<typename SceneElementType>
SceneElementType UBasicUnit::GetSceneElementType() const
{
	return std::get<SceneElementType>(UnitType);
}

UCLASS(BlueprintType, Blueprintable)
class PLANET_API UCoinUnit : public UBasicUnit
{
	GENERATED_BODY()

public:

	friend FSceneToolsContainer;

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

	friend FSceneToolsContainer;

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

	friend FSceneToolsContainer;

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

	friend FSceneToolsContainer;

	USkillUnit();

	USkillUnit(ESceneToolsType InSceneToolsType);

	int32 Level = 1;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "All Abilities")
	TSubclassOf<USkill_Base>SkillClass;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SkillType")
	ESkillType SkillType = ESkillType::kActive;

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

	friend FSceneToolsContainer;

	UWeaponUnit();

	int32 DamageDegree = 0;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ToolClass")
	TSubclassOf<AToolUnitBase> ToolActorClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Skill")
	ESkillUnitType FirstSkillClass;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Skill")
	EAnimLinkClassType AnimLinkClassType = EAnimLinkClassType::kUnarmed;

	USkillUnit* FirstSkill = nullptr;

protected:

};

USTRUCT(BlueprintType)
struct FSceneToolsContainer
{
	GENERATED_USTRUCT_BODY()

	using FOnSkillUnitChanged = TCallbackHandleContainer<void(USkillUnit*, bool)>;

	using FOnToolUnitChanged = TCallbackHandleContainer<void(UToolUnit*)>;
	
	using FOnConsumableUnitChanged = TCallbackHandleContainer<void(UConsumableUnit*, bool, int32)>;

	using FOnCoinUnitChanged = TCallbackHandleContainer<void(UCoinUnit*, bool, int32)>;


	UBasicUnit* AddUnit(FGuid UnitGuid, int32 Num);

	UBasicUnit* FindUnit(UBasicUnit::IDType ID);


	UCoinUnit* AddUnit_Coin(FGuid UnitGuid, int32 Num);
	
	UCoinUnit* AddUnit_Coin(ECoinUnitType CoinUnitType, int32 Num);

	UCoinUnit* FindUnit_Coin(FGuid UnitGuid);

	UCoinUnit* FindUnit_Coin(ECoinUnitType CoinUnitType);


	UConsumableUnit* AddUnit_Consumable(FGuid UnitGuid, int32 Num = 1);

	UConsumableUnit* AddUnit_Consumable(EConsumableUnitType ConsumableUnitType, int32 Num = 1);

	void RemoveUnit_Consumable(UConsumableUnit*UnitPtr, int32 Num = 1);


	UToolUnit* AddUnit_ToolUnit(FGuid UnitGuid);


	UWeaponUnit* AddUnit_Weapon(FGuid UnitGuid);

	UWeaponUnit* FindUnit_Weapon(FGuid UnitGuid);


	USkillUnit* AddUnit_Skill(FGuid UnitGuid);

	USkillUnit* AddUnit_Skill(ESkillUnitType SkillUnitType);

	USkillUnit* FindUnit_Skill(FGuid UnitGuid);

	USkillUnit* FindUnit_Skill(ESkillUnitType SkillUnitType);


	const TArray<UBasicUnit*>& GetSceneUintAry()const;

	const TMap<ECoinUnitType, UCoinUnit*>& GetCoinUintAry()const;

	void AddUnit_Apending(FGuid UnitGuid, FGuid Guid);

	void SyncApendingUnit(FGuid Guid);

	FOnSkillUnitChanged OnSkillUnitChanged;

	FOnToolUnitChanged OnToolUnitChanged;

	FOnConsumableUnitChanged OnConsumableUnitChanged;

	FOnCoinUnitChanged OnCoinUnitChanged;

private:

	FTableRowUnit* GetTableRowUnit(FGuid UnitGuid)const;

	USkillUnit* GetUnitByType(ESkillUnitType SkillUnitType)const;
	
	UCoinUnit* GetUnitByType(ECoinUnitType CoinUnitType)const;
	
	UWeaponUnit* GetUnitByType(EWeaponUnitType WeaponUnitType)const;
	
	UConsumableUnit* GetUnitByType(EConsumableUnitType ConsumableUnitType)const;
	
	UPROPERTY()
	TArray<UBasicUnit*> SceneToolsAry;

	TMap<UBasicUnit::IDType, UBasicUnit*> SceneMetaMap;

	TMap<EConsumableUnitType, UConsumableUnit*> ConsumablesUnitMap;

	TMap<ESkillUnitType, USkillUnit*> SkillUnitMap;
	
	TMap<ECoinUnitType, UCoinUnit*> CoinUnitMap;

	TMap<FGuid, TTuple<FGuid, int32>> SkillUnitApendingMap;

};

#pragma endregion HoldingItems
