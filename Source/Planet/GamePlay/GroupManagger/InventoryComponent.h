#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "GenerateType.h"
#include "GroupManaggerInterface.h"
#include "ItemProxy_Minimal.h"
#include "ItemProxy_Container.h"

#include "InventoryComponent.generated.h"

struct FSceneProxyContainer;
struct FBasicProxy;
struct FProxy_FASI;
struct FCharacterSocket;
struct FSkillProxy;
struct FWeaponProxy;
struct IProxy_Allocationble;
struct FWeaponSkillProxy;
struct IProxy_Allocationble;
class IPlanetControllerInterface;
class ACharacterBase;

/*
 *	持有物品相关
 */
UCLASS(BlueprintType, Blueprintable)
class UInventoryComponent :
	public UActorComponent,
	public IGroupManaggerInterface
{
	GENERATED_BODY()

	friend FBasicProxy;
	
public:
	friend ACharacterBase;
	friend FProxy_FASI;
	friend IProxy_Allocationble;
	friend FWeaponSkillProxy;
	friend IProxy_Allocationble;

	using FOwnerType = ACharacterBase;

	using IDType = FGuid;

	using FOnSkillProxyChanged = TCallbackHandleContainer<void(
		const TSharedPtr<
			FSkillProxy>&,
		EProxyModifyType
		)>;

	using FOnGroupmateProxyChanged = TCallbackHandleContainer<void(
		const TSharedPtr<
			FCharacterProxy>&,
		EProxyModifyType
		)
	>;

	using FOnConsumableProxyChanged = TCallbackHandleContainer<void(
		const TSharedPtr<
			FConsumableProxy>&,
			EProxyModifyType
		)
	>;

	using FOnCoinProxyChanged = TCallbackHandleContainer<void(
		const TSharedPtr<
			FCoinProxy>&,
		EProxyModifyType,
		int32
		)>;

	using FOnWeaponProxyChanged = TCallbackHandleContainer<void(
		const TSharedPtr<
			FWeaponProxy>&,
		EProxyModifyType
		)
	>;

	static FName ComponentName;

	UInventoryComponent(
		const FObjectInitializer& ObjectInitializer
		);

#if UE_EDITOR || UE_CLIENT
	TSharedPtr<FBasicProxy> AddProxy_SyncHelper(
		const TSharedPtr<FBasicProxy>& ProxySPtr
		);

	TSharedPtr<FBasicProxy> UpdateProxy_SyncHelper(
		const TSharedPtr<FBasicProxy>& LocalProxySPtr,
		const TSharedPtr<FBasicProxy>& RemoteProxySPtr
		);
#endif


#if UE_EDITOR || UE_SERVER
	TSharedPtr<FBasicProxy> AddProxy(
		const FGameplayTag& ProxyType,
		int32 Num
		);

	TSharedPtr<FBasicProxy> FindProxy(
		const IDType& ID
		)const;

	TArray<TSharedPtr<FBasicProxy>> GetProxys(
		const FGameplayTag& ProxyType
		)const;

	TSharedPtr<IProxy_Allocationble> FindAllocationableProxy(
		const IDType& ID
		)const;

	TSharedPtr<IProxy_Allocationble> FindProxy_BySocket(
		const FCharacterSocket& Socket
		)const;


	TSharedPtr<FCharacterProxy> AddProxy_Character(
		const FGameplayTag& ProxyType
		);

	TSharedPtr<FCharacterProxy> FindProxy_Character(
		const IDType& ID
		) const;

	TSharedPtr<FCharacterProxy> InitialOwnerCharacterProxy(
		ACharacterBase* OwnerCharacterPtr
		);

	TArray<TSharedPtr<FCharacterProxy>> GetCharacterProxyAry() const;

	TSharedPtr<FWeaponProxy> AddProxy_Weapon(
		const FGameplayTag& ProxyType
		);

	TSharedPtr<FWeaponProxy> Update_Weapon(
		const TSharedPtr<FWeaponProxy>& Proxy
		);

	TSharedPtr<FWeaponProxy> FindProxy_Weapon(
		const IDType& ID
		)const;


	TSharedPtr<FSkillProxy> FindProxy_Skill(
		const FGameplayTag& ProxyType
		)const;

	TSharedPtr<FSkillProxy> FindProxy_Skill(
		const IDType& ID
		) const;

	TSharedPtr<FSkillProxy> AddProxy_Skill(
		const FGameplayTag& ProxyType
		);


	TSharedPtr<FCoinProxy> FindProxy_Coin(
		const FGameplayTag& ProxyType
		) const;

	TSharedPtr<FCoinProxy> AddProxy_Coin(
		const FGameplayTag& ProxyType,
		int32 Num
		);


	TSharedPtr<FConsumableProxy> AddProxy_Consumable(
		const FGameplayTag& ProxyType,
		int32 Num
		);

	TSharedPtr<FConsumableProxy> FindProxy_Consumable(
		const IDType& ID
		) const;

	void RemoveProxy_Consumable(
		const TSharedPtr<FConsumableProxy>& ProxyPtr,
		int32 Num
		);


	const TArray<TSharedPtr<FBasicProxy>>& GetSceneUintAry() const;

	TArray<TSharedPtr<FBasicProxy>> GetProxys() const;
#endif

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


	virtual void OnGroupManaggerReady(
		AGroupManagger* NewGroupSharedInfoPtr
		) override;

	UPROPERTY(Replicated)
	FProxy_FASI_Container Proxy_Container;

	FOnSkillProxyChanged OnSkillProxyChanged;

	FOnConsumableProxyChanged OnConsumableProxyChanged;

	FOnCoinProxyChanged OnCoinProxyChanged;

	FOnGroupmateProxyChanged OnGroupmateProxyChanged;

	FOnWeaponProxyChanged OnWeaponProxyChanged;

private:

	void UpdateID(const FBasicProxy::IDType& NewID,const FBasicProxy::IDType& OldID);
	
protected:
	virtual void InitializeComponent() override;

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
		) const override;

private:
#if UE_EDITOR || UE_CLIENT
	void SetAllocationCharacterProxy(
		const FGuid& Proxy_ID,
		const FGuid& CharacterProxy_ID,
		const FGameplayTag& InSocketTag
		);
#endif

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
	TArray<TSharedPtr<FBasicProxy>> ProxysAry;

	TMap<IDType, TSharedPtr<FBasicProxy>> ProxysMap;

	TMap<FGameplayTag, TSharedPtr<FBasicProxy>> ProxyTypeMap;
};
