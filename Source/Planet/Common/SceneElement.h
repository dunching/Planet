// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
#include <variant>

#include "CoreMinimal.h"
#include "GenerateType.h"

#include "SceneElement.generated.h"

class AToolUnitBase;

class USkill_Base;

struct FSceneToolsContainer;

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
enum class ESceneToolsType : uint8
{
	kNone,
	kWeapon,
	kTool,
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
enum class EWeaponUnitType : uint8
{
	kNone,
	kPickAxe,
	kWeaponHandProtection,
};

UENUM(BlueprintType)
enum class ESkillUnitType : uint8
{
	kNone,
	kHumanSkills_ZMJZ,
	kHumanSkill_PickAxe_Attack1,
	kHumanSkill_WeaponHandProtection_Attack1,
	kHumanSkill_Displacement,
	kHumanSkill_TalentSkill_NuQi,
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
class UBasicUnit : public UObject
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
class UToolUnit : public UBasicUnit
{
	GENERATED_BODY()

public:

	friend FSceneToolsContainer;

	UToolUnit();

	int32 DamageDegree = 0;

	int32 Num = 1;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ToolsIcons")
	TSubclassOf<AToolUnitBase> ToolActorClass;

protected:

};

UCLASS(BlueprintType, Blueprintable)
class UGourpMateUnit : public UBasicUnit
{
	GENERATED_BODY()

public:

	UGourpMateUnit();

	void InitialByCharactor(ACharacterBase* InCharacterPtr);

	ACharacterBase* CharacterPtr = nullptr;

	int32 Level = 1;

	FName Name;

protected:

};

UCLASS(BlueprintType, Blueprintable)
class USkillUnit : public UBasicUnit
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
class UWeaponUnit : public UBasicUnit
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

	UToolUnit* AddUnit(EToolUnitType Type);

	UWeaponUnit* AddUnit(EWeaponUnitType Type);

	USkillUnit* AddUnit(ESkillUnitType Type);
	
	USkillUnit* FindUnit(ESkillUnitType Type);

	UBasicUnit* FindUnit(UBasicUnit::IDType ID);

	const TArray<UBasicUnit*>& GetSceneUintAry()const;

private:

	UPROPERTY()
	TArray<UBasicUnit*> SceneToolsAry;

	TMap<UBasicUnit::IDType, UBasicUnit*> SceneMetaMap;

	TMap<ESceneToolsType, TSharedPtr<int32>> NumIndexMap;
};

#pragma endregion HoldingItems