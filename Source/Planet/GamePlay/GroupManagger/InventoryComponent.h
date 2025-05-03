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
			EProxyModifyType,
		int32
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

	void RemoveProxy_SyncHelper(
		const TSharedPtr<FBasicProxy>& ProxySPtr
		);

	void UpdateProxy_SyncHelper(
		const TSharedPtr<FBasicProxy>& LocalProxySPtr,
		const TSharedPtr<FBasicProxy>& RemoteProxySPtr
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
		)const;

	/**
	 * 修改物品的数量
	 * 如果是可堆叠的，则增加一个物品并修改数量
	 * 如果是不可堆叠的，则仅增加一个物品？
	 * @param ProxyType 
	 * @param Num 
	 * @return 
	 */
	TSharedPtr<FBasicProxy> AddProxyNum(
		const FGameplayTag& ProxyType,
		int32 Num
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
		)const;

	TSharedPtr<IProxy_Allocationble> FindAllocationableProxy(
		const IDType& ID
		)const;

	TSharedPtr<IProxy_Allocationble> FindProxy_BySocket(
		const FCharacterSocket& Socket
		)const;

	const TArray<TSharedPtr<FBasicProxy>>& GetSceneUintAry() const;

	TArray<TSharedPtr<FBasicProxy>> GetProxys() const;
	
#pragma endregion

#pragma region Character
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
#pragma endregion

#pragma region Weapon
	TSharedPtr<FWeaponProxy> AddProxy_Weapon(
		const FGameplayTag& ProxyType
		);

	TSharedPtr<FWeaponProxy> Update_Weapon(
		const TSharedPtr<FWeaponProxy>& Proxy
		);

	TSharedPtr<FWeaponProxy> FindProxy_Weapon(
		const IDType& ID
		)const;
#pragma endregion

#pragma region Skill
	TSharedPtr<FSkillProxy> FindProxy_Skill(
		const FGameplayTag& ProxyType
		)const;

	TSharedPtr<FSkillProxy> FindProxy_Skill(
		const IDType& ID
		) const;

	TSharedPtr<FSkillProxy> AddProxy_Skill(
		const FGameplayTag& ProxyType
		);
#pragma endregion

#pragma region Coin
	TSharedPtr<FCoinProxy> FindProxy_Coin(
		const FGameplayTag& ProxyType
		) const;

	TSharedPtr<FCoinProxy> AddProxy_Coin(
		const FGameplayTag& ProxyType,
		int32 Num
		);

	void RemoveProxy_Coin(
		const TSharedPtr<FCoinProxy>& ProxyPtr,
		int32 Num
		);
#pragma endregion

#pragma region Consumable
	
	TSharedPtr<FConsumableProxy> FindProxy_Consumable(
		const IDType& ID
		) const;

	TSharedPtr<FConsumableProxy> AddProxy_Consumable(
		const FGameplayTag& ProxyType,
		int32 Num,
		const IDType& ID = IDType()
		);

	void RemoveProxy_Consumable(
		const TSharedPtr<FConsumableProxy>& ProxyPtr,
		int32 Num
		);
#pragma endregion
	
#endif

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
