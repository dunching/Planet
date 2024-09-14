#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "GenerateType.h"
#include <SceneElement.h>
#include "SceneUnitContainer.h"

#include "HoldingItemsComponent.generated.h"

struct FSceneUnitContainer;
struct FProxy_FASI;
class IPlanetControllerInterface;
class ACharacterBase;

/*
	持有物品相关
*/
UCLASS(BlueprintType, Blueprintable)
class UHoldingItemsComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	friend ACharacterBase;
	friend FProxy_FASI;

	using FOwnerType = ACharacterBase;

	using IDType = FGuid;

	using FOnSkillUnitChanged = TCallbackHandleContainer<void(const TSharedPtr<FSkillProxy>&, bool)>;

	using FOnToolUnitChanged = TCallbackHandleContainer<void(const TSharedPtr < FToolProxy>&)>;

	using FOnGroupmateUnitChanged = TCallbackHandleContainer<void(const TSharedPtr < FCharacterProxy>&, bool)>;

	using FOnConsumableUnitChanged = TCallbackHandleContainer<void(const TSharedPtr < FConsumableProxy>&, bool, int32)>;

	using FOnCoinUnitChanged = TCallbackHandleContainer<void(const TSharedPtr<FCoinProxy>&, bool, int32)>;

	using FOnWeaponUnitChanged = TCallbackHandleContainer<void(const TSharedPtr<FWeaponProxy>&, bool)>;

	static FName ComponentName;

	UHoldingItemsComponent(const FObjectInitializer& ObjectInitializer);

#if UE_EDITOR || UE_CLIENT
	TSharedPtr<FBasicProxy> AddProxy_SyncHelper(const TSharedPtr<FBasicProxy>& ProxySPtr);

	TSharedPtr<FBasicProxy> UpdateProxy_SyncHelper(const TSharedPtr<FBasicProxy>& ProxySPtr);
#endif


#if UE_EDITOR || UE_SERVER
	TSharedPtr<FBasicProxy> AddProxy(const FGameplayTag& UnitType, int32 Num);

	TSharedPtr<FBasicProxy> FindProxy(const IDType& ID);


	TSharedPtr<FCharacterProxy> InitialDefaultCharacter();

	TSharedPtr<FCharacterProxy> AddUnit_Character(const FGameplayTag& UnitType);

	TSharedPtr<FCharacterProxy> Update_Character(const TSharedPtr<FCharacterProxy>& Unit);

	TSharedPtr<FCharacterProxy> FindUnit_Character(const IDType& ID)const;

	TArray<TSharedPtr<FCharacterProxy>> GetCharacterProxyAry()const;


	TSharedPtr<FWeaponProxy> AddUnit_Weapon(const FGameplayTag& UnitType);

	TSharedPtr<FWeaponProxy> Update_Weapon(const TSharedPtr<FWeaponProxy>& Unit);

	TSharedPtr<FWeaponProxy> FindUnit_Weapon(const IDType& ID);


	TSharedPtr<FSkillProxy> FindUnit_Skill(const FGameplayTag& UnitType);

	TSharedPtr<FSkillProxy> FindUnit_Skill(const IDType& ID)const;

	TSharedPtr<FSkillProxy> AddUnit_Skill(const FGameplayTag& UnitType);

	TSharedPtr<FSkillProxy> Update_Skill(const TSharedPtr<FSkillProxy>& Unit);


	TSharedPtr<FCoinProxy> FindUnit_Coin(const FGameplayTag& UnitType)const;

	TSharedPtr<FCoinProxy> AddUnit_Coin(const FGameplayTag& UnitType, int32 Num);


	TSharedPtr <FConsumableProxy> AddUnit_Consumable(const FGameplayTag& UnitType, int32 Num = 1);

	void RemoveUnit_Consumable(const TSharedPtr <FConsumableProxy>& UnitPtr, int32 Num = 1);

	TSharedPtr<FToolProxy> AddUnit_ToolUnit(const FGameplayTag& UnitType);

	const TArray<TSharedPtr<FBasicProxy>>& GetSceneUintAry()const;

	const TMap<FGameplayTag, TSharedPtr<FCoinProxy>>& GetCoinUintAry()const;

	TArray<TSharedPtr<FBasicProxy>>GetProxys()const;
#endif


	void AddUnit_Apending(FGameplayTag UnitType, int32 Num, FGuid Guid);

	void SyncApendingUnit(FGuid Guid);
	
	// 同步到服務器
	UFUNCTION(Server, Reliable)
	void SetAllocationCharacterUnit(
		const FGuid& Proxy_ID,
		const FGuid& CharacterProxy_ID
	);
	
	UPROPERTY(ReplicatedUsing = OnRep_ProxyChanged)
	FProxy_FASI_Container Proxy_Container;

	FOnSkillUnitChanged OnSkillUnitChanged;

	FOnToolUnitChanged OnToolUnitChanged;

	FOnConsumableUnitChanged OnConsumableUnitChanged;

	FOnCoinUnitChanged OnCoinUnitChanged;

	FOnGroupmateUnitChanged OnGroupmateUnitChanged;

	FOnWeaponUnitChanged OnWeaponUnitChanged;

protected:

	virtual void BeginPlay()override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_ProxyChanged();

private:

	// 等待加入“库存”的物品
	TMap<FGuid, TMap<FGameplayTag, int32>> SkillUnitApendingMap;

	// 自定义的“排列方式”
	TArray<TSharedPtr<FBasicProxy>> SceneToolsAry;

	TMap<IDType, TSharedPtr<FBasicProxy>> SceneMetaMap;

	TMap<FGameplayTag, TSharedPtr<FCoinProxy>> CoinUnitMap;

	// 默认的
	TSharedPtr<FCharacterProxy> CharacterProxySPtr = nullptr;

};