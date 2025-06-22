#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "ItemProxy_Container.h"
#include "ItemProxy_GenericType.h"
#include "GetModifyItemProxyStrategiesInterface.h"
#include "Tools.h"

#include "InventoryComponentBase.generated.h"

struct FSceneProxyContainer;
struct FBasicProxy;
struct FProxy_FASI;

struct FModifyItemProxyStrategyIterface;

/*
 *	持有物品相关
 */
UCLASS(BlueprintType, Blueprintable)
class ITEMPROXY_API UInventoryComponentBase :
	public UActorComponent
{
	GENERATED_BODY()

	friend FProxy_FASI;

public:
	using FOwnerType = ACharacterBase;

	using IDType = FGuid;

	static FName ComponentName;

	UInventoryComponentBase(
		const FObjectInitializer& ObjectInitializer
		);

	void ProcessProxyInteraction(
		const FGuid& ProxyID,
		EItemProxyInteractionType ItemProxyInteractionType
		);

#if UE_EDITOR || UE_CLIENT
	TSharedPtr<FBasicProxy> AddProxy_SyncHelper(
		const TSharedPtr<FBasicProxy>& CacheProxySPtr
		);

	void UpdateProxy_SyncHelper(
		const TSharedPtr<FBasicProxy>& LocalProxySPtr,
		const TSharedPtr<FBasicProxy>& RemoteProxySPtr
		);

	void RemoveProxy_SyncHelper(
		const TSharedPtr<FBasicProxy>& ProxySPtr
		);
#endif

#pragma region 合并添加
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

#pragma region 基础操作
	TSharedPtr<FBasicProxy> FindProxy(
		const IDType& ID
		) const;

	template <typename ModifyItemProxyStrategyType, bool bIsFullStrategy = false>
	auto FindProxy(
		const IDType& ID
		) const -> std::conditional<bIsFullStrategy, TSharedPtr<ModifyItemProxyStrategyType>, TSharedPtr<typename
			                            ModifyItemProxyStrategyType::FItemProxyType>>::type;

	TSharedPtr<FBasicProxy> FindProxyType(
		const FGameplayTag& ProxyType
		) const;

	TArray<TSharedPtr<FBasicProxy>> FindAllProxyType(
		const FGameplayTag& ProxyType
		) const;

	template <typename ModifyItemProxyStrategyType, bool bIsFullStrategy = false>
	auto FindProxyType(
		const FGameplayTag& ProxyType
		) const -> std::conditional<bIsFullStrategy, TSharedPtr<ModifyItemProxyStrategyType>, TArray<TSharedPtr<typename
										ModifyItemProxyStrategyType::FItemProxyType>>>::type;

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

	void UpdateProxy(
		const IDType& ID
		);

	/**
	 * 移除物品
	 */
	void RemoveProxy(
		const IDType& ID
		);

	TArray<TSharedPtr<FBasicProxy>> GetProxys(
		const FGameplayTag& ProxyType
		) const;

	const TArray<TSharedPtr<FBasicProxy>>& GetSceneUintAry() const;

	TArray<TSharedPtr<FBasicProxy>> GetProxys() const;

#pragma endregion

#endif

	void AddToContainer(
		const TSharedPtr<FBasicProxy>& ItemProxySPtr
		);

	void UpdateInContainer(
		const TSharedPtr<FBasicProxy>& ItemProxySPtr
		);

	void RemoveFromContainer(
		const TSharedPtr<FBasicProxy>& ItemProxySPtr
		);

	template <typename ModifyItemProxyStrategyType>
	TSharedPtr<ModifyItemProxyStrategyType> GetModifyItemProxyStrategy() const;

	const TMap<FGameplayTag, TSharedPtr<FModifyItemProxyStrategyIterface>>& GetModifyItemProxyStrategies() const;

	void UpdateID(
		const FGuid& NewID,
		const FGuid& OldID
		);

private:

#pragma region RPC
	
	UFUNCTION(Server, Reliable)
	void ProcessProxyInteraction_Server(
		const FGuid& ProxyID,
		EItemProxyInteractionType ItemProxyInteractionType
		);

#pragma endregion

protected:
	virtual void InitializeComponent() override;

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
		) const override;

	// 等待加入“库存”的物品
	TMap<FGuid, TMap<FGameplayTag, int32>> PendingMap;

	// 自定义的“排列方式”
	TArray<TSharedPtr<FBasicProxy>> ProxysAry;

	TMap<IDType, TSharedPtr<FBasicProxy>> ProxysMap;

	/**
	 * 单独的绑定
	 */
	TMap<FGameplayTag, TSharedPtr<FModifyItemProxyStrategyIterface>> ModifyItemProxyStrategiesMap;

	UPROPERTY(Replicated)
	FProxy_FASI_Container Proxy_Container;

private:
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
	) const -> typename std::conditional<bIsFullStrategy, TSharedPtr<ModifyItemProxyStrategyType>,  TArray<TSharedPtr<typename
										ModifyItemProxyStrategyType::FItemProxyType>>>::type
{
	ModifyItemProxyStrategyType ModifyItemProxyStrategy;

	if constexpr (bIsFullStrategy)
	{
		if (ModifyItemProxyStrategiesMap.Contains(ModifyItemProxyStrategy.GetCanOperationType()))
		{
			auto ResultSPtr = ModifyItemProxyStrategiesMap[ModifyItemProxyStrategy.GetCanOperationType()];

			ResultSPtr->FindByType(ProxyType, this);
			
			return ResultSPtr;
		}
		return nullptr;
	}
	else
	{
		if (ModifyItemProxyStrategiesMap.Contains(ModifyItemProxyStrategy.GetCanOperationType()))
		{
			auto ResultSPtr = ModifyItemProxyStrategiesMap[ModifyItemProxyStrategy.GetCanOperationType()];

			auto Ary = ResultSPtr->FindByType(ProxyType, this);
			
			TArray<TSharedPtr<typename ModifyItemProxyStrategyType::FItemProxyType>>Result;

			for (auto Iter : Ary)
			{
				Result.Add(DynamicCastSharedPtr<typename ModifyItemProxyStrategyType::FItemProxyType>(Iter));
			}
			
			return Result;
		}
		return {};
	}
}

template <typename ModifyItemProxyStrategyType>
TSharedPtr<typename ModifyItemProxyStrategyType::FItemProxyType> UInventoryComponentBase::AddProxy(
	const FGameplayTag& ProxyType
	)
{
	ModifyItemProxyStrategyType ModifyItemProxyStrategy;

	if (ModifyItemProxyStrategiesMap.Contains(ModifyItemProxyStrategy.GetCanOperationType()))
	{
		auto ResultSPtr = ModifyItemProxyStrategiesMap[ModifyItemProxyStrategy.GetCanOperationType()];

		auto AddResultAry = ResultSPtr->Add(this, ProxyType, 1);

		if (AddResultAry.IsEmpty())
		{
		}
		else
		{
			return DynamicCastSharedPtr<typename ModifyItemProxyStrategyType::FItemProxyType>(
				 AddResultAry[0]
				);
		}
	}

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
