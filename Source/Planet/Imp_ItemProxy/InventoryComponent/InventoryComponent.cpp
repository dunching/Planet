#include "InventoryComponent.h"

#include <GameFramework/PlayerState.h>

#include "CharacterAttributesComponent.h"
#include "Net/UnrealNetwork.h"

#include "GameplayTagsLibrary.h"
#include "CharacterBase.h"
#include "PlanetPlayerState.h"
#include "DataTableCollection.h"
#include "ItemProxy_Container.h"
#include "PlanetControllerInterface.h"
#include "ItemProxy_Character.h"
#include "ModifyItemProxyStrategy.h"
#include "RewardsTD.h"

void UInventoryComponent::InitializeComponent()
{
	Super::InitializeComponent();

	//
	ModifyItemProxyStrategiesMap.Empty();
	{
		auto ModifyItemProxyStrategySPtr = MakeShared<FModifyItemProxyStrategy_Character>();
		ModifyItemProxyStrategiesMap.Add(
		                                 ModifyItemProxyStrategySPtr->GetCanOperationType(),
		                                 ModifyItemProxyStrategySPtr
		                                );
	}
	{
		auto ModifyItemProxyStrategySPtr = MakeShared<FModifyItemProxyStrategy_Weapon>();
		ModifyItemProxyStrategiesMap.Add(
		                                 ModifyItemProxyStrategySPtr->GetCanOperationType(),
		                                 ModifyItemProxyStrategySPtr
		                                );
	}
	{
		auto ModifyItemProxyStrategySPtr = MakeShared<FModifyItemProxyStrategy_WeaponSkill>();
		ModifyItemProxyStrategiesMap.Add(
		                                 ModifyItemProxyStrategySPtr->GetCanOperationType(),
		                                 ModifyItemProxyStrategySPtr
		                                );
	}
	{
		auto ModifyItemProxyStrategySPtr = MakeShared<FModifyItemProxyStrategy_ActiveSkill>();
		ModifyItemProxyStrategiesMap.Add(
		                                 ModifyItemProxyStrategySPtr->GetCanOperationType(),
		                                 ModifyItemProxyStrategySPtr
		                                );
	}
	{
		auto ModifyItemProxyStrategySPtr = MakeShared<FModifyItemProxyStrategy_PassveSkill>();
		ModifyItemProxyStrategiesMap.Add(
		                                 ModifyItemProxyStrategySPtr->GetCanOperationType(),
		                                 ModifyItemProxyStrategySPtr
		                                );
	}
	{
		auto ModifyItemProxyStrategySPtr = MakeShared<FModifyItemProxyStrategy_Coin>();
		ModifyItemProxyStrategiesMap.Add(
		                                 ModifyItemProxyStrategySPtr->GetCanOperationType(),
		                                 ModifyItemProxyStrategySPtr
		                                );
	}
	{
		auto ModifyItemProxyStrategySPtr = MakeShared<FModifyItemProxyStrategy_Consumable>();
		ModifyItemProxyStrategiesMap.Add(
		                                 ModifyItemProxyStrategySPtr->GetCanOperationType(),
		                                 ModifyItemProxyStrategySPtr
		                                );
	}
	{
		auto ModifyItemProxyStrategySPtr = MakeShared<FModifyItemProxyStrategy_MaterialProxy>();
		ModifyItemProxyStrategiesMap.Add(
		                                 ModifyItemProxyStrategySPtr->GetCanOperationType(),
		                                 ModifyItemProxyStrategySPtr
		                                );
	}
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();


#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		// 初始化一些需要的Proxy
		AddProxyNum(UGameplayTagsLibrary::Proxy_Coin_Regular, 0);
	}
#endif
}

void UInventoryComponent::OnSelfGroupManaggerReady(
	AGroupManagger* NewGroupSharedInfoPtr
	)
{
}

void UInventoryComponent::OnPlayerGroupManaggerReady(
	AGroupManagger* NewGroupSharedInfoPtr
	)
{
}

TSharedPtr<FCharacterProxy> UInventoryComponent::InitialOwnerCharacterProxy(
	ACharacterBase* OwnerCharacterPtr
	)
{
	Proxy_Container.SetInventoryComponentBase(this);

	if (!OwnerCharacterPtr)
	{
		return nullptr;
	}

	// 是否已存在？
	const auto ID = OwnerCharacterPtr->GetCharacterAttributesComponent()->GetCharacterID();
	if (auto ResultSPtr = FindProxy(ID))
	{
		// 使用指定的ID
		auto CharacterProxySPtr = DynamicCastSharedPtr<FCharacterProxy>(ResultSPtr);

		CharacterProxySPtr->ProxyCharacterPtr = OwnerCharacterPtr;
		CharacterProxySPtr->SetInventoryComponentBase(this);

		return CharacterProxySPtr;
	}
	else
	{
		// 使用指定的ID
		auto CharacterProxySPtr = MakeShared<FCharacterProxy>(
		                                                      OwnerCharacterPtr->GetCharacterAttributesComponent()->
		                                                      GetCharacterID()
		                                                     );

		CharacterProxySPtr->InitialProxy(OwnerCharacterPtr->GetCharacterAttributesComponent()->CharacterCategory);
		CharacterProxySPtr->ProxyCharacterPtr = OwnerCharacterPtr;
		CharacterProxySPtr->SetInventoryComponentBase(this);

		AddToContainer(CharacterProxySPtr);

		return CharacterProxySPtr;
	}
}

TArray<TSharedPtr<FCharacterProxy>> UInventoryComponent::GetCharacterProxyAry() const
{
	TArray<TSharedPtr<FCharacterProxy>> Result;

	for (auto Iter : ProxysAry)
	{
		if (Iter->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Character))
		{
			auto GroupmateProxyPtr = DynamicCastSharedPtr<FCharacterProxy>(Iter);
			check(GroupmateProxyPtr);
			if (GroupmateProxyPtr)
			{
				Result.Add(GroupmateProxyPtr);
			}
		}
	}

	return Result;
}

TSharedPtr<IProxy_Allocationble> UInventoryComponent::FindAllocationableProxy(
	const IDType& ID
	) const
{
	return DynamicCastSharedPtr<IProxy_Allocationble>(FindProxy(ID));
}

TSharedPtr<IProxy_Allocationble> UInventoryComponent::FindProxy_BySocket(
	const FCharacterSocket& Socket
	) const
{
	return FindAllocationableProxy(Socket.GetAllocationedProxyID());
}

#if UE_EDITOR || UE_CLIENT
void UInventoryComponent::SetAllocationCharacterProxy(
	const FGuid& Proxy_ID,
	const FGuid& CharacterProxy_ID,
	const FGameplayTag& InSocketTag
	)
{
	SetAllocationCharacterProxy_Server(Proxy_ID, CharacterProxy_ID, InSocketTag);
}

void UInventoryComponent::UpdateSocket(
	const FGuid& CharacterProxy_ID,
	const FCharacterSocket& Socket
	)
{
	UpdateSocket_Server(CharacterProxy_ID, Socket);
}

#endif

void UInventoryComponent::SetAllocationCharacterProxy_Server_Implementation(
	const FGuid& Proxy_ID,
	const FGuid& CharacterProxy_ID,
	const FGameplayTag& InSocketTag
	)
{
	auto ProxySPtr = FindAllocationableProxy(Proxy_ID);
	if (!ProxySPtr)
	{
		return;
	}

	if (auto CharacterProxySPtr = FindProxy<FModifyItemProxyStrategy_Character>(CharacterProxy_ID))
	{
		ProxySPtr->SetAllocationCharacterProxy(CharacterProxySPtr, InSocketTag);
	}
	else
	{
		ProxySPtr->ResetAllocationCharacterProxy();
	}
}

void UInventoryComponent::UpdateSocket_Server_Implementation(
	const FGuid& CharacterProxy_ID,
	const FCharacterSocket& Socket
	)
{
	auto CharacterProxySPtr = FindProxy<FModifyItemProxyStrategy_Character>(CharacterProxy_ID);
	if (!CharacterProxySPtr)
	{
		return;
	}

	CharacterProxySPtr->UpdateSocket(Socket);
}
