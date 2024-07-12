// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
#include <variant>

#include "CoreMinimal.h"

#include "GenerateType.h"
#include "BaseData.h"

#include "SceneElement.generated.h"

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
	kSkill,
	kGroupMate,
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
	kTest,
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

	kHumanSkill_Talent_NuQi,
	kHumanSkill_Talent_YinYang,
};

UENUM(BlueprintType)
enum class ESkillType : uint8
{
	kActive,
	kWeaponActive,
	kPassive,
	kTalentPassive,
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

protected:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ToolsIcons")
	TSoftObjectPtr<UTexture2D> DefaultIcon;

	std::variant<
		EToolUnitType, 
		EWeaponUnitType,
		EConsumableUnitType,
		ESkillUnitType
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

	int32 DamageDegree = 0;

	int32 Num = 1;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ToolType")
	TSubclassOf<AToolUnitBase> ToolActorClass;

protected:

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

	int32 Level = 1;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "All Abilities")
	TSubclassOf<USkill_Base>SkillClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "SkillType")
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

	UConsumableUnit* AddUnit(EConsumableUnitType Type, int32 Num = 1);

	UToolUnit* AddUnit(EToolUnitType Type);

	UWeaponUnit* AddUnit(EWeaponUnitType Type);

	USkillUnit* AddUnit(ESkillUnitType Type);
	
	UWeaponUnit* FindUnit(EWeaponUnitType Type);

	USkillUnit* FindUnit(ESkillUnitType Type);

	UBasicUnit* FindUnit(UBasicUnit::IDType ID);

	const TArray<UBasicUnit*>& GetSceneUintAry()const;

private:

	UPROPERTY()
	TArray<UBasicUnit*> SceneToolsAry;

	TMap<UBasicUnit::IDType, UBasicUnit*> SceneMetaMap;

	TMap<EConsumableUnitType, UConsumableUnit*> ConsumablesUnitMap;
};

#pragma endregion HoldingItems
