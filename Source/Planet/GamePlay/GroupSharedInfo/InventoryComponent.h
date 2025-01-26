#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "GenerateType.h"
#include "GroupSharedInterface.h"
#include "ItemProxy_Minimal.h"
#include "ItemProxy_Container.h"

#include "InventoryComponent.generated.h"

struct FSceneProxyContainer;
struct FProxy_FASI;
struct FCharacterSocket;
struct FSkillProxy;
struct FWeaponProxy;
class IPlanetControllerInterface;
class ACharacterBase;

/*
 *	持有物品相关
 */
UCLASS(BlueprintType, Blueprintable)
class UInventoryComponent :
	public UActorComponent,
	public IGroupSharedInterface
{
	GENERATED_BODY()

public:
	friend ACharacterBase;
	friend FProxy_FASI;

	using FOwnerType = ACharacterBase;

	using IDType = FGuid;

	using FOnSkillProxyChanged = TCallbackHandleContainer<void(const TSharedPtr<FSkillProxy>&, bool)>;

	using FOnToolProxyChanged = TCallbackHandleContainer<void(const TSharedPtr<FToolProxy>&)>;

	using FOnGroupmateProxyChanged = TCallbackHandleContainer<void(const TSharedPtr<FCharacterProxy>&, bool)>;

	using FOnConsumableProxyChanged = TCallbackHandleContainer<void(const TSharedPtr<FConsumableProxy>&,
	                                                                EProxyModifyType)>;

	using FOnCoinProxyChanged = TCallbackHandleContainer<void(const TSharedPtr<FCoinProxy>&, bool, int32)>;

	using FOnWeaponProxyChanged = TCallbackHandleContainer<void(const TSharedPtr<FWeaponProxy>&, bool)>;

	static FName ComponentName;

	UInventoryComponent(const FObjectInitializer& ObjectInitializer);

#if UE_EDITOR || UE_CLIENT
	TSharedPtr<FBasicProxy> AddProxy_SyncHelper(const TSharedPtr<FBasicProxy>& ProxySPtr);

	TSharedPtr<FBasicProxy> UpdateProxy_SyncHelper(const TSharedPtr<FBasicProxy>& ProxySPtr);

	void SetAllocationCharacterProxy(
		const FGuid& Proxy_ID,
		const FGuid& CharacterProxy_ID,
		const FGameplayTag& InSocketTag
	);
#endif


#if UE_EDITOR || UE_SERVER
	TSharedPtr<FBasicProxy> AddProxy(const FGameplayTag& ProxyType, int32 Num);

	TSharedPtr<FBasicProxy> FindProxy(const IDType& ID);

	TSharedPtr<FAllocationbleProxy> FindAllocationableProxy(const IDType& ID);

	TSharedPtr<FAllocationbleProxy> FindProxy_BySocket(const FCharacterSocket& Socket);


	TSharedPtr<FCharacterProxy> AddProxy_Character(const FGameplayTag& ProxyType);

	TSharedPtr<FCharacterProxy> Update_Character(const TSharedPtr<FCharacterProxy>& Proxy);

	TSharedPtr<FCharacterProxy> FindProxy_Character(const IDType& ID) const;

	TSharedPtr<FCharacterProxy> InitialOwnerCharacterProxy(ACharacterBase* OwnerCharacterPtr);

	TArray<TSharedPtr<FCharacterProxy>> GetCharacterProxyAry() const;

	void UpdateSocket(const TSharedPtr<FCharacterProxy>& CharacterProxySPtr, const FCharacterSocket& Socket);


	TSharedPtr<FWeaponProxy> AddProxy_Weapon(const FGameplayTag& ProxyType);

	TSharedPtr<FWeaponProxy> Update_Weapon(const TSharedPtr<FWeaponProxy>& Proxy);

	TSharedPtr<FWeaponProxy> FindProxy_Weapon(const IDType& ID);


	TSharedPtr<FSkillProxy> FindProxy_Skill(const FGameplayTag& ProxyType);

	TSharedPtr<FSkillProxy> FindProxy_Skill(const IDType& ID) const;

	TSharedPtr<FSkillProxy> AddProxy_Skill(const FGameplayTag& ProxyType);

	TSharedPtr<FSkillProxy> Update_Skill(const TSharedPtr<FSkillProxy>& Proxy);


	TSharedPtr<FCoinProxy> FindProxy_Coin(const FGameplayTag& ProxyType) const;

	TSharedPtr<FCoinProxy> AddProxy_Coin(const FGameplayTag& ProxyType, int32 Num);


	TSharedPtr<FConsumableProxy> AddProxy_Consumable(const FGameplayTag& ProxyType, int32 Num);

	TSharedPtr<FConsumableProxy> FindProxy_Consumable(const IDType& ID) const;

	void RemoveProxy_Consumable(const TSharedPtr<FConsumableProxy>& ProxyPtr, int32 Num);


	TSharedPtr<FToolProxy> AddProxy_ToolProxy(const FGameplayTag& ProxyType);


	const TArray<TSharedPtr<FBasicProxy>>& GetSceneUintAry() const;

	const TMap<FGameplayTag, TSharedPtr<FCoinProxy>>& GetCoinUintAry() const;

	TArray<TSharedPtr<FBasicProxy>> GetProxys() const;
#endif

	UFUNCTION(Server, Reliable)
	void AddProxys_Server(const FGuid&RewardsItemID);

	void AddProxy_Pending(FGameplayTag ProxyType, int32 Num, FGuid Guid);

	void SyncPendingProxy(FGuid Guid);


	virtual void OnGroupSharedInfoReady(AGroupSharedInfo* NewGroupSharedInfoPtr) override;

	UPROPERTY(Replicated)
	FProxy_FASI_Container Proxy_Container;

	FOnSkillProxyChanged OnSkillProxyChanged;

	FOnToolProxyChanged OnToolProxyChanged;

	FOnConsumableProxyChanged OnConsumableProxyChanged;

	FOnCoinProxyChanged OnCoinProxyChanged;

	FOnGroupmateProxyChanged OnGroupmateProxyChanged;

	FOnWeaponProxyChanged OnWeaponProxyChanged;

protected:
	virtual void InitializeComponent() override;

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UFUNCTION(Server, Reliable)
	void UpdateSocket_Server(const FGuid& CharacterProxyID, const FCharacterSocket& Socket);

	// 同步到服務器
	UFUNCTION(Server, Reliable)
	void SetAllocationCharacterProxy_Server(
		const FGuid& Proxy_ID,
		const FGuid& CharacterProxy_ID,
		const FGameplayTag& InSocketTag
	);

	// 等待加入“库存”的物品
	TMap<FGuid, TMap<FGameplayTag, int32>> PendingMap;

	// 自定义的“排列方式”
	TArray<TSharedPtr<FBasicProxy>> SceneToolsAry;

	TMap<IDType, TSharedPtr<FBasicProxy>> SceneMetaMap;

	TMap<FGameplayTag, TSharedPtr<FCoinProxy>> CoinProxyMap;
};
