#include "AIComponent.h"

#include "Net/UnrealNetwork.h"

#include "CharacterBase.h"
#include "CharactersInfo.h"
#include "GameplayTagsLibrary.h"
#include "GroupManagger.h"
#include "HumanAIController.h"
#include "HumanCharacter_AI.h"
#include "InventoryComponent.h"
#include "ItemProxy_Skills.h"
#include "ItemProxy_Weapon.h"
#include "ModifyItemProxyStrategy.h"
#include "NiagaraComponent.h"
#include "PlanetGameViewportClient.h"
#include "TaskPromt.h"
#include "TeamMatesHelperComponent.h"

FName UAIComponent::ComponentName = TEXT("AIComponent");

UAIComponent::UAIComponent(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UAIComponent::BeginPlay()
{
	Super::BeginPlay();

	auto OnwerActorPtr = GetOwner<FOwnerType>();
	if (!OnwerActorPtr)
	{
		return;
	}

#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		// 这里按次序获取要追踪的位置
		// 		TArray<AActor*> OutActors;
		// 		OwnerPtr->GetAttachedActors(OutActors);
		// 
		// 		auto ParentPtr = OwnerPtr->GetAttachParentActor();
		// 		if (auto ColonyPtr = Cast<AGeneratorColony_ByTime>(ParentPtr))
		// 		{
		// 			const auto ChildAry = ColonyPtr->FormationComponentPtr->GetAttachChildren();
		// 			if (ChildAry.Num() >= OutActors.Num())
		// 			{
		// 				auto OwnerCharacterPtr = Cast<ACharacterBase>(OwnerPtr);
		// 				if (OwnerCharacterPtr)
		// 				{
		// 					OwnerCharacterPtr->GetController<AHumanAIController>()->PathFollowComponentPtr = ChildAry[OutActors.Num() - 1];
		// 				}
		// 			}
		// 		}
	}
#endif

	OnwerActorPtr->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}

void UAIComponent::OnGroupManaggerReady(
	AGroupManagger* NewGroupSharedInfoPtr
	)
{
#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		InitialAllocationsRowName();
		InitialAllocationsByProxy();

		FGuid Guid = FGuid::NewGuid();
		auto OnwerActorPtr = GetOwner<FOwnerType>();
		auto InventoryComponentPtr = OnwerActorPtr->GetInventoryComponent();
		for (const auto Iter : ProxyMap)
		{
			InventoryComponentPtr->AddProxy_Pending(Iter.Key, Iter.Value.Num, Guid);
		}

		InventoryComponentPtr->SyncPendingProxy(Guid);
	}
#endif

#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_Client)
	{
		auto OnwerActorPtr = GetOwner<FOwnerType>();
		OnwerActorPtr->GetProxyProcessComponent()->ActiveWeapon();
	}
#endif
}

void UAIComponent::AddTemporaryTaskNode(
	UTaskNode_Temporary* TaskNodePtr
	)
{
}

void UAIComponent::InitialAllocationsRowName()
{
#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
#if TESTAICHARACTERHOLDDATA
		// TestCommand::AddAICharacterTestDataImp(this);
#endif
		{
			auto OnwerActorPtr = GetOwner<FOwnerType>();
			auto CharacterProxy = OnwerActorPtr->GetCharacterProxy();
			auto OwnerCharacterProxy = OnwerActorPtr->GetGroupManagger()->GetTeamMatesHelperComponent()->
			                                          GetOwnerCharacterProxy();
			if (OwnerCharacterProxy)
			{
				if (OwnerCharacterProxy->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Character_Player))
				{
					return;
				}
				else if (OwnerCharacterProxy->GetProxyType().MatchesTag(
				                                                        UGameplayTagsLibrary::Proxy_Character_NPC_Assistional
				                                                       ))
				{
				}
			}

			auto InventoryComponentPtr = OnwerActorPtr->GetInventoryComponent();
			// 武器
			{
				{
					if (FirstWeaponSocketInfo.IsValid())
					{
						auto NewWeaponProxySPtr = InventoryComponentPtr->AddProxy<FModifyItemProxyStrategy_Weapon>(
							 FirstWeaponSocketInfo
							);
						if (NewWeaponProxySPtr)
						{
							FCharacterSocket SkillsSocketInfo;
							SkillsSocketInfo.Socket = UGameplayTagsLibrary::WeaponSocket_1;

							NewWeaponProxySPtr->SetAllocationCharacterProxy(
							                                                CharacterProxy,
							                                                SkillsSocketInfo.Socket
							                                               );

							SkillsSocketInfo.UpdateProxy(NewWeaponProxySPtr);

							CharacterProxy->UpdateSocket(SkillsSocketInfo);
						}
					}
				}
			}

			// 技能
			{
				if (ActiveSkillSet_1.IsValid())
				{
					auto SkillProxyPtr = InventoryComponentPtr->AddProxy<FModifyItemProxyStrategy_ActiveSkill>(
					                                                           ActiveSkillSet_1
					                                                          );
					if (SkillProxyPtr)
					{
						FCharacterSocket SkillsSocketInfo;

						SkillsSocketInfo.Socket = UGameplayTagsLibrary::ActiveSocket_1;

						SkillProxyPtr->SetAllocationCharacterProxy(
						                                           CharacterProxy,
						                                           SkillsSocketInfo.Socket
						                                          );

						SkillsSocketInfo.UpdateProxy(SkillProxyPtr);

						CharacterProxy->UpdateSocket(SkillsSocketInfo);
					}
				}

				OnwerActorPtr->GetProxyProcessComponent()->UpdateCanbeActiveSkills();
			}
		}
	}
#endif
}

void UAIComponent::InitialAllocationsByProxy()
{
	auto OnwerActorPtr = GetOwner<FOwnerType>();
	OnwerActorPtr->GetProxyProcessComponent()->ActiveWeapon();
}

void UAIComponent::StopDisplayTaskPromy()
{
	auto OnwerActorPtr = GetOwner<FOwnerType>();
	OnwerActorPtr->GetNiagaraComponent()->SetActive(false);
}

TMap<FGameplayTag, FProductsForSale> UAIComponent::GetSaleItemsInfo() const
{
	return ProxyMap;
}

void UAIComponent::SetIsCheerOn(
	bool IsCheerOn
	)
{
	bIsCheerOn = IsCheerOn;
}

bool UAIComponent::GetCheerOn() const
{
	return bIsCheerOn;
}

void UAIComponent::DisplayTaskPromy(
	TSubclassOf<UTaskPromt> TaskPromtClass
	)
{
	auto OnwerActorPtr = GetOwner<FOwnerType>();
	OnwerActorPtr->GetNiagaraComponent()->SetActive(true);
}
