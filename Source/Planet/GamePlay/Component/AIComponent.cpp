
#include "AIComponent.h"

#include "Net/UnrealNetwork.h"

#include "CharacterBase.h"
#include "CharactersInfo.h"
#include "GameplayTagsLibrary.h"
#include "HumanAIController.h"
#include "HumanCharacter_AI.h"
#include "InventoryComponent.h"
#include "PlanetGameViewportClient.h"
#include "TaskPromt.h"

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

void UAIComponent::OnGroupManaggerReady(AGroupManagger* NewGroupSharedInfoPtr)
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
			auto InventoryComponentPtr = OnwerActorPtr->GetInventoryComponent();
			// 武器
			{
				{
					if (FirstWeaponSocketInfo.IsValid())
					{
						FCharacterSocket SkillsSocketInfo;
						SkillsSocketInfo.Socket = UGameplayTagsLibrary::WeaponSocket_1;
							
						auto NewWeaponProxySPtr = InventoryComponentPtr->AddProxy_Weapon(
							FirstWeaponSocketInfo);
						if (NewWeaponProxySPtr)
						{
							NewWeaponProxySPtr->SetAllocationCharacterProxy(OnwerActorPtr->GetCharacterProxy(),SkillsSocketInfo.Socket);
								
							SkillsSocketInfo.UpdateProxy(NewWeaponProxySPtr);
						}
					}
				}
			}

			// 技能
			{
				if (ActiveSkillSet_1.IsValid())
				{
					FCharacterSocket SkillsSocketInfo;

					SkillsSocketInfo.Socket = UGameplayTagsLibrary::ActiveSocket_1;

					auto SkillProxyPtr = InventoryComponentPtr->AddProxy_Skill(
						ActiveSkillSet_1);
					if (SkillProxyPtr)
					{
						SkillProxyPtr->SetAllocationCharacterProxy(OnwerActorPtr->GetCharacterProxy(),SkillsSocketInfo.Socket);
								
						SkillsSocketInfo.UpdateProxy(SkillProxyPtr);
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
#if UE_EDITOR || UE_CLIENT
	if (GetNetMode() == NM_Client)
	{
		auto ScreenLayer = UKismetGameLayerManagerLibrary::GetGameLayer<FHoverWidgetScreenLayer>(
			GetWorld(),
			TargetPointSharedLayerName
		);
		if (ScreenLayer)
		{
			ScreenLayer->RemoveHoverWidget(TaskPromtPtr);
			TaskPromtPtr = nullptr;
		}
	}
#endif
}

void UAIComponent::DisplayTaskPromy(
	 TSubclassOf<UTaskPromt> TaskPromtClass
)
{
#if UE_EDITOR || UE_CLIENT
	if (GetNetMode() == NM_Client)
	{
		auto ScreenLayer = UKismetGameLayerManagerLibrary::GetGameLayer<FHoverWidgetScreenLayer>(
			GetWorld(),
			TargetPointSharedLayerName
		);
		if (ScreenLayer)
		{
			if (TaskPromtPtr)
			{
				return;
			}
			
			// TaskPromtPtr = CreateWidget<UTaskPromt>(GetWorld(), TaskPromtClass);
			// if (TaskPromtPtr)
			// {
			// 	auto OnwerActorPtr = GetOwner<FOwnerType>();
			// 	TaskPromtPtr->TargetCharacterPtr = OnwerActorPtr;
			// 	ScreenLayer->AddHoverWidget(TaskPromtPtr);
			// }
		}
	}
#endif
}
