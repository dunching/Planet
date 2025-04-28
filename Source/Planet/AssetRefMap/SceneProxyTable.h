// Zowee. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include <GenerateType.h>

#include "GameplayTagContainer.h"

#include "ItemProxy_Minimal.h"
#include "CharacterAttibutes.h"

#include "SceneProxyTable.generated.h"

class UDataTable;

struct FToolProxy;
struct FWeaponProxy;
struct FSkillProxy;
struct FCoinProxy;
struct FBasicProxy;
struct FCharacterProxy;
class AWeapon_Base; 
class AConsumable_Base;
class UItemProxy_Description;
class UItemDecription;

USTRUCT(BlueprintType)
struct PLANET_API FTableRowProxy_CommonCooldownInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int32 CoolDownTime = 10;

};

USTRUCT(BlueprintType)
struct PLANET_API FTableRowProxy : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName RowDescriptionText = TEXT("Row描述文字");

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSoftObjectPtr<UTexture2D> DefaultIcon;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FString ProxyName = TEXT("ProxyName");

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSoftObjectPtr<UTexture2D> RaffleIcon;

	/**
	 * 这个Item使用哪个数据
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSoftObjectPtr<UItemProxy_Description> ItemProxy_Description;

	/**
	 * 这个Item使用哪个Widget进行展示
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<UItemDecription> ItemDecriptionClass;

};

USTRUCT(BlueprintType)
struct PLANET_API FTableRowProxy_WeaponExtendInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<AWeapon_Base> ToolActorClass;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayTag WeaponSkillProxyType = FGameplayTag::EmptyTag;

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
struct PLANET_API FTableRowProxy_SkillExtendInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<USkill_Base>SkillClass;

};

USTRUCT(BlueprintType)
struct PLANET_API FTableRowProxy_ActiveSkillExtendInfo : public FTableRowProxy_SkillExtendInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayTag RequireWeaponProxyType = FGameplayTag::EmptyTag;

	// 技能公共CD
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSet<FGameplayTag>SkillCommonCooldownInfoMap;

	// 技能CD
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<int32, int32>SkillCooldownInfoMap;

};

USTRUCT(BlueprintType)
struct PLANET_API FTableRowProxy_PassiveSkillExtendInfo : public FTableRowProxy_SkillExtendInfo
{
	GENERATED_USTRUCT_BODY()
	
};

USTRUCT(BlueprintType)
struct PLANET_API FTableRowProxy_WeaponSkillExtendInfo : public FTableRowProxy_SkillExtendInfo
{
	GENERATED_USTRUCT_BODY()
	
};

USTRUCT(BlueprintType)
struct PLANET_API FTableRowProxy_Consumable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<AConsumable_Base> Consumable_Class;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<USkill_Consumable_Base> Skill_Consumable_Class;
	
	// UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	// TMap<ECharacterPropertyType, FBaseProperty>ModifyPropertyMap;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TMap<FGameplayTag, int32>ModifyPropertyMap;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	UAnimMontage* HumanMontage = nullptr;

	// 消耗品公共CD
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSet<FGameplayTag>CommonCooldownInfoMap;

	// 消耗品独立CD
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int32 CD = 1;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	float Duration = 3.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	float PerformActionInterval = 1.f;

};
