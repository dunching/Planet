// Zowee. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include <GenerateType.h>

#include "GameplayTagContainer.h"

#include "SceneElement.h"
#include "CharacterAttibutes.h"

#include "SceneUnitTable.generated.h"

class UDataTable;

struct FToolProxy;
struct FWeaponProxy;
struct FSkillProxy;
struct FCoinProxy;
struct FBasicProxy;
struct FCharacterProxy;
class AWeapon_Base; 
class AConsumable_Base;

USTRUCT(BlueprintType)
struct PLANET_API FTableRowUnit_CommonCooldownInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int32 CoolDownTime = 10;

};

USTRUCT(BlueprintType)
struct PLANET_API FTableRowUnit : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName RowDescriptionText = TEXT("Row描述文字");

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSoftObjectPtr<UTexture2D> DefaultIcon;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FString UnitName = TEXT("UnitName");

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSoftObjectPtr<UTexture2D> RaffleIcon;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName DescriptionText = TEXT("描述文字");

};

USTRUCT(BlueprintType)
struct PLANET_API FTableRowUnit_WeaponExtendInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<AWeapon_Base> ToolActorClass;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayTag WeaponSkillUnitType = FGameplayTag::EmptyTag;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	EAnimLinkClassType AnimLinkClassType = EAnimLinkClassType::kUnarmed;
	
	// 武器的主属性词条
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Addtional Element")
	FGameplayTag PropertyEntry;
	
	// 在使用这个武器时，最大攻击范围为多少(AI会使用这个进行场景查询)
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Addtional Element")
	int32 MaxAttackDistance = 50;
	
};

USTRUCT(BlueprintType)
struct PLANET_API FTableRowUnit_SkillExtendInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<USkill_Base>SkillClass;

};

USTRUCT(BlueprintType)
struct PLANET_API FTableRowUnit_ActiveSkillExtendInfo : public FTableRowUnit_SkillExtendInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayTag RequireWeaponUnitType = FGameplayTag::EmptyTag;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSet<FGameplayTag>SkillCommonCooldownInfoMap;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<int32, int32>SkillCooldownInfoMap;

};

USTRUCT(BlueprintType)
struct PLANET_API FTableRowUnit_PassiveSkillExtendInfo : public FTableRowUnit_SkillExtendInfo
{
	GENERATED_USTRUCT_BODY()
	
	// 
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Addtional Element")
	TMap<ECharacterPropertyType, int32>AddtionalElementMap;
	
};

USTRUCT(BlueprintType)
struct PLANET_API FTableRowUnit_WeaponSkillExtendInfo : public FTableRowUnit_SkillExtendInfo
{
	GENERATED_USTRUCT_BODY()
	
};

USTRUCT(BlueprintType)
struct PLANET_API FTableRowUnit_CharacterInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	
	FTableRowUnit_CharacterInfo();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayTag FirstWeaponSocketInfo;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayTag SecondWeaponSocketInfo;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayTag ActiveSkillSet_1;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayTag ActiveSkillSet_2;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FCharacterAttributes CharacterAttributes;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FCharacterAttributes CharacterAttributesPerLevel;

};

USTRUCT(BlueprintType)
struct PLANET_API FTableRowUnit_Consumable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<AConsumable_Base> Consumable_Class;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<USkill_Consumable_Base> Skill_Consumable_Class;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TMap<ECharacterPropertyType, FBaseProperty>ModifyPropertyMap;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	UAnimMontage* HumanMontage = nullptr;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSet<FGameplayTag>CommonCooldownInfoMap;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	float Duration = 3.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	float PerformActionInterval = 1.f;

};
