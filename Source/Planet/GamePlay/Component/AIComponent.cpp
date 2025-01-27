
#include "AIComponent.h"

#include "Net/UnrealNetwork.h"

#include "GeneratorColony.h"
#include "CharacterBase.h"
#include "CharactersInfo.h"
#include "GameplayTagsLibrary.h"
#include "HumanAIController.h"
#include "HumanCharacter_AI.h"
#include "InventoryComponent.h"
#include "SceneProxyExtendInfo.h"
#include "TestCommand.h"

FName UAIComponent::ComponentName = TEXT("AIComponent");

UAIComponent::UAIComponent(const FObjectInitializer& ObjectInitializer):
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
// 		if (auto ColonyPtr = Cast<AGeneratorColony>(ParentPtr))
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

void UAIComponent::OnGroupSharedInfoReady(AGroupSharedInfo* NewGroupSharedInfoPtr)
{
#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		InitialAllocationsRowName();
		InitialAllocationsByProxy();
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

void UAIComponent::AddTemporaryTaskNode(UTaskNode_Temporary*TaskNodePtr)
{
	TemporaryTaskNodesAry.Add(TaskNodePtr);
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
			auto TableRowProxy_CharacterInfoPtr =
				USceneProxyExtendInfoMap::GetInstance()->GetTableRowProxy_AICharacter_Allocation(AI_Allocation_RowName);
			auto InventoryComponentPtr = OnwerActorPtr->GetInventoryComponent();
			if (TableRowProxy_CharacterInfoPtr)
			{
				// 武器
				{
					{
						if (TableRowProxy_CharacterInfoPtr->FirstWeaponSocketInfo.IsValid())
						{
							FCharacterSocket SkillsSocketInfo;
							SkillsSocketInfo.Socket = UGameplayTagsLibrary::WeaponSocket_1;
							
							auto NewWeaponProxySPtr = InventoryComponentPtr->AddProxy_Weapon(
								TableRowProxy_CharacterInfoPtr->FirstWeaponSocketInfo);
							if (NewWeaponProxySPtr)
							{
								NewWeaponProxySPtr->SetAllocationCharacterProxy(OnwerActorPtr->GetCharacterProxy(),SkillsSocketInfo.Socket);
								
								SkillsSocketInfo.UpdateProxy(NewWeaponProxySPtr);

								InventoryComponentPtr->UpdateSocket(OnwerActorPtr->GetCharacterProxy(), SkillsSocketInfo);
							}
						}
					}
				}

				// 技能
				{
					if (TableRowProxy_CharacterInfoPtr->ActiveSkillSet_1.IsValid())
					{
						FCharacterSocket SkillsSocketInfo;

						SkillsSocketInfo.Socket = UGameplayTagsLibrary::ActiveSocket_1;

						auto SkillProxyPtr = InventoryComponentPtr->AddProxy_Skill(
							TableRowProxy_CharacterInfoPtr->ActiveSkillSet_1);
						if (SkillProxyPtr)
						{
							SkillProxyPtr->SetAllocationCharacterProxy(OnwerActorPtr->GetCharacterProxy(),SkillsSocketInfo.Socket);
								
							SkillsSocketInfo.UpdateProxy(SkillProxyPtr);

							InventoryComponentPtr->UpdateSocket(OnwerActorPtr->GetCharacterProxy(), SkillsSocketInfo);
						}
					}

					OnwerActorPtr->GetProxyProcessComponent()->UpdateCanbeActiveSkills();
				}
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
