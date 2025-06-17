// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
#include <variant>

#include "CoreMinimal.h"
#include <GameplayTagContainer.h>

#include "TemplateHelper.h"

#include "ItemProxy_GenericType.h"

#include "ItemProxy.generated.h"

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
class UTexture2D;
class AToolProxyBase;
class AWeapon_Base;
class USkill_Consumable_Base;
class USkill_Consumable_Generic;
class AConsumable_Base;
class IPlanetControllerInterface;
class APlanetPlayerState;
class APlanetGameMode;

class UItemProxy_Description;
class USkill_Base;
class ACharacterBase;
class AHumanCharacter_AI;
class AHumanCharacter;
class UInventoryComponentBase;
struct FCharacterProxy;

struct FAllocationSkills;

struct FTalentHelper;
struct FSceneProxyContainer;
struct FProxy_FASI_Container;
struct FSkillCooldownHelper;
struct IProxy_Allocationble;

struct FItemProxyVisitor_Hover_Base;
struct FItemProxyVisitor_InAllocation_Base;

enum struct ECharacterPropertyType : uint8;

/**
 * ItemProxy的数值
 * 例如药剂类的数据，持续多长时间，每隔几秒回复多少血量
 */
UCLASS()
class ITEMPROXY_API UItemProxy_Description : public UDataAsset
{
	GENERATED_BODY()

public:
	/**
	 * 简要说明
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FString Summary;

	/**
	 * 详细说明
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<FString> DecriptionText;

	/**
	 * 数值
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<FString, FPerLevelValue_Float> Values;

	/**
	 * 作为物品时的显示图片
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSoftObjectPtr<UTexture2D> DefaultIcon;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FString ProxyName = TEXT("ProxyName");

	/**
	 * 立绘，
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSoftObjectPtr<UTexture2D> RaffleIcon;
};

/**
 * 序列化&反序列化
 * 通用数据记录在DataTable，变化数据记录在对象内
 * 场景内的对象代理
 */
USTRUCT()
struct ITEMPROXY_API FBasicProxy
{
	GENERATED_USTRUCT_BODY()

public:
	friend UInventoryComponentBase;

	using FOnAllocationCharacterProxyChanged =
	TCallbackHandleContainer<void(
		const TWeakPtr<FCharacterProxy>&

		
		)>;

	using FItemProxy_Description = UItemProxy_Description;
	
	using IDType = FGuid;

	FBasicProxy();

	virtual ~FBasicProxy();

	// FBasicProxy operator=(const FBasicProxy&) = delete;

	virtual bool NetSerialize(
		FArchive& Ar,
		class UPackageMap* Map,
		bool& bOutSuccess
		);

	virtual void InitialProxy(
		const FGameplayTag& InProxyType
		);

	/**
	 * 通过远程的更新客户端的数据
	 * @param RemoteSPtr 
	 */
	void UpdateByRemote(
		const TSharedPtr<FBasicProxy>& RemoteSPtr
		);

	virtual void ProcessProxyInteraction(
		EItemProxyInteractionType ItemProxyInteractionType
		);

	virtual TSet<EItemProxyInteractionType> GetInteractionsType() const;

	// 装备至插槽
	virtual void Allocation();

	// 从插槽移除
	virtual void UnAllocation();

	// 是否可以激活
	virtual bool CanActive() const;

	// 激活
	virtual bool Active();

	//  取消 激活
	virtual void Cancel();

	//  取消
	virtual void End();

	IDType GetID() const;

	FGameplayTag GetProxyType() const;

	TSoftObjectPtr<UTexture2D> GetIcon() const;

	// 
	FString GetProxyName() const;

	TObjectPtr<FItemProxy_Description> GetItemProxy_Description() const;

	void SetInventoryComponentBase(
		const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr
		);

	TObjectPtr<UInventoryComponentBase> GetInventoryComponentBase() const;

	/**
	 * 更新数据
	 */
	virtual void UpdateData();

private:
	/**
	 * 是否是唯一的，比如货币
	 * @return 
	 */
	virtual bool IsUnique() const;

protected:
	const FTableRowProxy* GetTableRowProxy() const;

	void SetID(
		const IDType& NewID
		);

private:
	// Root组件,不为空
	TObjectPtr<UInventoryComponentBase> InventoryComponentPtr = nullptr;

	UPROPERTY(Transient)
	FGameplayTag ProxyType = FGameplayTag::EmptyTag;

	IDType ID;
};

template <>
struct TStructOpsTypeTraits<FBasicProxy> :
	public TStructOpsTypeTraitsBase2<FBasicProxy>
{
	enum
	{
		WithNetSerializer = true,
	};
};
