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
struct FTableRowUnit_CharacterInfo;
class AToolUnitBase;
class AWeapon_Base;
class USkill_Consumable_Base;
class AConsumable_Base;
class IPlanetControllerInterface;

class USkill_Base;
class ACharacterBase;
class AHumanCharacter;
class UCharacterUnit;

struct FAllocationSkills;
struct FCharacterAttributes;
struct FTalentHelper;
struct FSceneUnitContainer;

enum class ECharacterPropertyType : uint8;

// 场景内的对象代理
// 通用数据记录在DataTable，变化数据记录在对象内
// 序列化&反序列化
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
	
	void SetAllocationCharacterUnit(UCharacterUnit* AllocationCharacterUnitPtr);

	TCallbackHandleContainer<void(UCharacterUnit*)> OnAllocationCharacterUnitChanged;

	UCharacterUnit* GetAllocationCharacterUnit()const;

protected:

	FTableRowUnit* GetTableRowUnit()const;

	UPROPERTY(Transient)
	FGameplayTag UnitType = FGameplayTag::EmptyTag;
	
	// 这个物品被分配给的对象
	UPROPERTY(Transient)
	UCharacterUnit * AllocationCharacterUnitPtr = nullptr;

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
class PLANET_API UCharacterUnit : public UBasicUnit
{
	GENERATED_BODY()

public:

	using FPawnType = ACharacterBase;

	UCharacterUnit();

	void InitialByCharactor(FPawnType* InCharacterPtr);

	FTableRowUnit_CharacterInfo* GetTableRowUnit_CharacterInfo()const;

	// 解除这个类下AddToRoot的对象
	void RelieveRootBind();

	FPawnType* ProxyCharacterPtr = nullptr;

	TSharedPtr<FCharacterAttributes> CharacterAttributes;

	TSharedPtr<FAllocationSkills> AllocationSkills;

	TSharedPtr<FSceneUnitContainer> SceneUnitContainer;

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