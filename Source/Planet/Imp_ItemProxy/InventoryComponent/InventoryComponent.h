#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "GroupManaggerInterface.h"
#include "InventoryComponentBase.h"
#include "ItemProxy_Minimal.h"
#include "ItemProxy_Container.h"

#include "InventoryComponent.generated.h"

struct FSceneProxyContainer;
struct FBasicProxy;
struct FCharacterProxy;
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
class PLANET_API UInventoryComponent :
	public UInventoryComponentBase,
	public IGroupManaggerInterface
{
	GENERATED_BODY()

public:
	virtual void InitializeComponent() override;

	virtual void BeginPlay() override;

	virtual void OnSelfGroupManaggerReady(
		AGroupManagger* NewGroupSharedInfoPtr
		) override;

	virtual void OnPlayerGroupManaggerReady(
		AGroupManagger* NewGroupSharedInfoPtr
		) override;

	TSharedPtr<FCharacterProxy> InitialOwnerCharacterProxy(
		ACharacterBase* OwnerCharacterPtr
		);

	TArray<TSharedPtr<FCharacterProxy>> GetCharacterProxyAry() const;

	TSharedPtr<IProxy_Allocationble> FindAllocationableProxy(
		const IDType& ID
		) const;

	TSharedPtr<IProxy_Allocationble> FindProxy_BySocket(
		const FCharacterSocket& Socket
		) const;

#if UE_EDITOR || UE_CLIENT
	void SetAllocationCharacterProxy(
		const FGuid& Proxy_ID,
		const FGuid& CharacterProxy_ID,
		const FGameplayTag& InSocketTag
		);

	void UpdateSocket(
		const FGuid& CharacterProxy_ID,
		const FCharacterSocket& Socket
		);
#endif

	// 同步到服務器
	UFUNCTION(Server, Reliable)
	void SetAllocationCharacterProxy_Server(
		const FGuid& Proxy_ID,
		const FGuid& CharacterProxy_ID,
		const FGameplayTag& InSocketTag
		);

	// 同步到服務器
	UFUNCTION(Server, Reliable)
	void UpdateSocket_Server(
		const FGuid& CharacterProxy_ID,
		const FCharacterSocket& Socket
		);
};
