#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "ItemProxy_Container.h"
#include "ItemProxy_GenericType.h"

#include "InventoryComponentBase.generated.h"

struct FSceneProxyContainer;
struct FBasicProxy;
struct FCharacterProxy;
struct FProxy_FASI;
struct FCharacterSocket;
struct FSkillProxy;
struct FWeaponProxyBase;
struct IProxy_Allocationble;
struct FWeaponSkillProxyBase;
struct IProxy_Allocationble;

struct FModifyItemProxyStrategyInterface;

class IPlanetControllerInterface;
class ACharacterBase;

/*
 *	持有物品相关
 */
UCLASS(BlueprintType, Blueprintable)
class ITEMPROXY_API UInventoryComponentBase :
	public UActorComponent
{
	GENERATED_BODY()

	friend ACharacterBase;
	friend FProxy_FASI;
	friend IProxy_Allocationble;
	friend FWeaponSkillProxyBase;
	friend IProxy_Allocationble;
	friend FBasicProxy;
	friend FCharacterProxy;

public:
	using FOwnerType = ACharacterBase;

	using IDType = FGuid;

	static FName ComponentName;

	UInventoryComponentBase(
		const FObjectInitializer& ObjectInitializer
		);

#if UE_EDITOR || UE_CLIENT
	TSharedPtr<FBasicProxy> AddProxy_SyncHelper(
		const TSharedPtr<FBasicProxy>& ProxySPtr
		);

	void UpdateProxy_SyncHelper(
		const TSharedPtr<FBasicProxy>& LocalProxySPtr,
		const TSharedPtr<FBasicProxy>& RemoteProxySPtr
		);

	void RemoveProxy_SyncHelper(
		const TSharedPtr<FBasicProxy>& ProxySPtr
		);
#endif

#pragma region Basic
	UFUNCTION(Server, Reliable)
	void AddProxys_Server(
		const FGuid& RewardsItemID
		);

	void AddProxy_Pending(
		FGameplayTag ProxyType,
		int32 Num,
		FGuid Guid
		);

	void SyncPendingProxy(
		FGuid Guid
		);
#pragma endregion

#if UE_EDITOR || UE_SERVER

#pragma region Basic
	TSharedPtr<FBasicProxy> FindProxy(
		const IDType& ID
		) const;

	template <typename ModifyItemProxyStrategyType, bool bIsFullStrategy = false>
	auto FindProxy(
		const IDType& ID
		) const->std::conditional<bIsFullStrategy, TSharedPtr<ModifyItemProxyStrategyType>, TSharedPtr<typename ModifyItemProxyStrategyType::FItemProxyType>>::type;

	TSharedPtr<FBasicProxy> FindProxyType(
		const FGameplayTag& ProxyType
		) const;

	template <typename ModifyItemProxyStrategyType, bool bIsFullStrategy = false>
	auto FindProxyType(
		const FGameplayTag& ProxyType
		) const->std::conditional<bIsFullStrategy, TSharedPtr<ModifyItemProxyStrategyType>, TSharedPtr<typename ModifyItemProxyStrategyType::FItemProxyType>>::type;
	
	/**
	 * 修改物品的数量
	 * 如果是可堆叠的，则增加一个物品并修改数量
	 * 如果是不可堆叠的，则仅增加对应数量个物品？
	 * @param ProxyType 
	 * @param Num 
	 * @return 
	 */
	TArray<TSharedPtr<FBasicProxy>> AddProxyNum(
		const FGameplayTag& ProxyType,
		int32 Num
		);

	TSharedPtr<FBasicProxy> AddProxy(
		const FGameplayTag& ProxyType
		);

	template <typename ModifyItemProxyStrategyType>
	TSharedPtr<typename ModifyItemProxyStrategyType::FItemProxyType> AddProxy(
		const FGameplayTag& ProxyType
		);

	/**
	 * 修改物品的数量
	 * 如果是可堆叠的，则减少一个物品的数量
	 * 如果是不可堆叠的，则移除物品？
	 * @param ProxyType 
	 * @param Num 
	 * @return 
	 */
	void RemoveProxyNum(
		const IDType& ID,
		int32 Num
		);

	TArray<TSharedPtr<FBasicProxy>> GetProxys(
		const FGameplayTag& ProxyType
		) const;

	const TArray<TSharedPtr<FBasicProxy>>& GetSceneUintAry() const;

	TArray<TSharedPtr<FBasicProxy>> GetProxys() const;

#pragma endregion

#endif

	void UpdateInContainer(
		const TSharedPtr<FBasicProxy>& ItemProxySPtr
		);

	void AddToContainer(
		const TSharedPtr<FBasicProxy>& ItemProxySPtr
		);

	template <typename ModifyItemProxyStrategyType>
	TSharedPtr<ModifyItemProxyStrategyType> GetModifyItemProxyStrategy() const;

	UPROPERTY(Replicated)
	FProxy_FASI_Container Proxy_Container;

private:
	void UpdateID(
		const FGuid& NewID,
		const FGuid& OldID
		);

protected:
	virtual void InitializeComponent() override;

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
		) const override;

private:
	// 等待加入“库存”的物品
	TMap<FGuid, TMap<FGameplayTag, int32>> PendingMap;

	// 自定义的“排列方式”
	TArray<TSharedPtr<FBasicProxy>> ProxysAry;

	TMap<IDType, TSharedPtr<FBasicProxy>> ProxysMap;

	TMap<FGameplayTag, TSharedPtr<FBasicProxy>> ProxyTypeMap;

	TMap<FGameplayTag, TSharedPtr<FModifyItemProxyStrategyInterface>> ModifyItemProxyStrategiesMap;
};

template <typename ModifyItemProxyStrategyType, bool bIsFullStrategy>
auto UInventoryComponentBase::FindProxy(
	const IDType& ID
	) const -> typename std::conditional<bIsFullStrategy, TSharedPtr<ModifyItemProxyStrategyType>, TSharedPtr<typename
	ModifyItemProxyStrategyType::FItemProxyType>>::type
{
	if (ProxysMap.Contains(ID))
	{
		auto ResultSPtr = ProxysMap[ID];

		auto Result = MakeShared<ModifyItemProxyStrategyType>();

		Result->FindByID(ID, ResultSPtr, this);
		
		if constexpr (bIsFullStrategy)
		{
			return Result;
		}
		else
		{
			return Result->ResultSPtr;
		}
	}

	return nullptr;
}

template <typename ModifyItemProxyStrategyType, bool bIsFullStrategy>
auto UInventoryComponentBase::FindProxyType(
	const FGameplayTag& ProxyType
	) const -> typename std::conditional<bIsFullStrategy, TSharedPtr<ModifyItemProxyStrategyType>, TSharedPtr<typename
	ModifyItemProxyStrategyType::FItemProxyType>>::type
{
	if (ProxyTypeMap.Contains(ProxyType))
	{
		auto ResultSPtr = ProxyTypeMap[ProxyType];
		
		auto Result = MakeShared<ModifyItemProxyStrategyType>();

		Result->FindByType(ProxyType, ResultSPtr, this);
		
		if constexpr (bIsFullStrategy)
		{
			return Result;
		}
		else
		{
			return Result->ResultSPtr;
		}
	}

	return nullptr;
}

template <typename ModifyItemProxyStrategyType>
TSharedPtr<typename ModifyItemProxyStrategyType::FItemProxyType> UInventoryComponentBase::AddProxy(
	const FGameplayTag& ProxyType
	)
{
	return nullptr;
}

template <typename ModifyItemProxyStrategyType>
TSharedPtr<ModifyItemProxyStrategyType> UInventoryComponentBase::GetModifyItemProxyStrategy() const
{
	TSharedPtr<ModifyItemProxyStrategyType> Result = MakeShared<ModifyItemProxyStrategyType>();

	if (ModifyItemProxyStrategiesMap.Contains(Result->GetCanOperationType()))
	{
		return DynamicCastSharedPtr<ModifyItemProxyStrategyType>(
		                                                         ModifyItemProxyStrategiesMap[
			                                                         Result->GetCanOperationType()]
		                                                        );
	}

	return nullptr;
}
