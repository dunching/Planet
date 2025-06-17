// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <GameplayTagContainer.h>

#include "ItemProxy.h"
#include "ItemProxy_Interface.h"
#include "Planet_ItemProxy.h"

#include "ItemProxy_Material.generated.h"

struct FGameplayAbilityTargetData_RegisterParam;
struct FTableRowProxy_CommonCooldownInfo;
struct FTableRowProxy;
struct FTableRowProxy_WeaponExtendInfo;
struct FTableRowProxy_ActiveSkillExtendInfo;
struct FTableRowProxy_PassiveSkillExtendInfo;
struct FTableRowProxy_WeaponSkillExtendInfo;
struct FTableRowProxy_CharacterGrowthAttribute;
struct FTableRowProxy_Consumable;
struct FTableRowProxy_GeneratiblePropertyEntrys;
struct FTableRowProxy_CharacterType;
struct FCharacterProxy;

struct FAllocationSkills;
struct FTalentHelper;
struct FSceneProxyContainer;
struct FProxy_FASI_Container;
struct FSkillCooldownHelper;

class APlanet_Consumable_Base;
class USkill_Consumable_Base;
class UInventoryComponent;
class UItemProxy_Description_Consumable;

UCLASS()
class PLANET_API UItemProxy_Description_Material : public UItemProxy_Description
{
	GENERATED_BODY()

public:
};

/**
 * 原材料，
 * 如
 * 伙伴道具
 * 用于伙伴类FCharacterProxy强化的道具
 * 如【射雕】里面的【侠士经略】
 * 或者 逆水寒的心法升级材料
 * 与消耗品的区别为：无法主动使用
 */
USTRUCT()
struct PLANET_API FMaterialProxy :
	public FPlanet_BasicProxy,
	public IProxy_Unique
{
	GENERATED_USTRUCT_BODY()

public:
	friend FSceneProxyContainer;
	friend FProxy_FASI_Container;
	friend UInventoryComponent;

	FMaterialProxy();

	virtual bool NetSerialize(
		FArchive& Ar,
		class UPackageMap* Map,
		bool& bOutSuccess
		) override;

	virtual void InitialProxy(
		const FGameplayTag& InProxyType
		) override;

	void UpdateByRemote(
		const TSharedPtr<FMaterialProxy>& RemoteSPtr
		);

	virtual bool Active() override final;

	UItemProxy_Description_Material* GetTableRowProxy_Material() const;

	virtual void UpdateData() override;

#pragma region IProxy_Unique
	virtual void ModifyNum(int32 Value) override;
#pragma endregion

protected:
};

template <>
struct TStructOpsTypeTraits<FMaterialProxy> :
	public TStructOpsTypeTraitsBase2<FMaterialProxy>
{
	enum
	{
		WithNetSerializer = true,
	};
};

UCLASS()
class PLANET_API UItemProxy_Description_ExperienceMaterial : public UItemProxy_Description_Material
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 ExperienceValue = 50;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 Level = 1;

};

USTRUCT()
struct PLANET_API FExperienceMaterialProxy :
	public FMaterialProxy
{
	GENERATED_USTRUCT_BODY()

public:
	using FItemProxy_Description = UItemProxy_Description_ExperienceMaterial;
	
	virtual bool NetSerialize(
		FArchive& Ar,
		class UPackageMap* Map,
		bool& bOutSuccess
		) override;

	TObjectPtr<FItemProxy_Description> GetTableRowProxy_ExperienceMaterial() const;

	/**
	 * 返回提供的经验值
	 * @return 
	 */
	int32 GetExperienceValue() const;
	
};

template <>
struct TStructOpsTypeTraits<FExperienceMaterialProxy> :
	public TStructOpsTypeTraitsBase2<FExperienceMaterialProxy>
{
	enum
	{
		WithNetSerializer = true,
	};
};
