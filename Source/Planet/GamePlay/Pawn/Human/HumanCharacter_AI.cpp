#include "HumanCharacter_AI.h"

#include <Kismet/GameplayStatics.h>
#include "Net/UnrealNetwork.h"
#include "Components/WidgetComponent.h"

#include "AIComponent.h"
#include "CharacterTitle.h"
#include "CharacterBase.h"
#include "ItemProxy_Minimal.h"
#include "HumanCharacter.h"
#include "InventoryComponent.h"
#include "GameplayTagsLibrary.h"
#include "SceneProxyExtendInfo.h"
#include "CharactersInfo.h"
#include "HumanAIController.h"
#include "GroupManagger.h"
#include "AIControllerStateTreeAIComponent.h"
#include "CharacterAbilitySystemComponent.h"
#include "CharacterAttributesComponent.h"
#include "GroupManagger_NPC.h"
#include "GuideInteraction.h"
#include "HumanCharacter_Player.h"

// UGameplayTasksComponent* USceneCharacterAIInteractionComponent::GetGameplayTasksComponent(
// 	const UGameplayTask& Task) const
// {
// 	return GetOwner<AHumanCharacter_AI>()->GetCharacterAbilitySystemComponent();
// }

void USceneCharacterAIInteractionComponent::StartInteractionItem(
	const TSubclassOf<AGuideInteraction_Actor>& Item
)
{
	Super::StartInteractionItem(Item);

	auto OnwerActorPtr = GetOwner<FOwnerType>();
	if (!OnwerActorPtr)
	{
		return;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.CustomPreSpawnInitalization = [OnwerActorPtr, this](
		AActor* ActorPtr
	)
		{
			auto GuideInteractionActorPtr = Cast<AGuideInteraction_HumanCharacter_AI>(ActorPtr);
			if (GuideInteractionActorPtr)
			{
				GuideInteractionActorPtr->Character_NPC = OnwerActorPtr;
			}
		};

	GuideInteractionActorPtr = GetWorld()->SpawnActor<AGuideInteraction_HumanCharacter_AI>(
		Item,
		SpawnParameters
	);
}

void UCharacterAIAttributesComponent::SetCharacterID(
	const FGuid& InCharacterID
)
{
	Super::SetCharacterID(InCharacterID);

#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		auto OwnerCharacterPtr = GetOwner<AHumanCharacter_AI>();
	}
#endif
}

void UCharacterNPCStateProcessorComponent::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, TargetCharacter, COND_InitialOnly);
}

TArray<TWeakObjectPtr<ACharacterBase>> UCharacterNPCStateProcessorComponent::GetTargetCharactersAry() const
{
	TArray<TWeakObjectPtr<ACharacterBase>> Result;

	if (TargetCharacter.IsValid())
	{
		Result.Add(TargetCharacter);
	}

	return Result;
}

void UCharacterNPCStateProcessorComponent::SetTargetCharactersAry(
	const TWeakObjectPtr<ACharacterBase>& InTargetCharacter
)
{
	TargetCharacter = InTargetCharacter;
}

AHumanCharacter_AI::AHumanCharacter_AI(
	const FObjectInitializer& ObjectInitializer
) :
  Super(
	  ObjectInitializer.
	  SetDefaultSubobjectClass<USceneCharacterAIInteractionComponent>(
		  USceneCharacterAIInteractionComponent::ComponentName
	  ).
	  SetDefaultSubobjectClass<UCharacterNPCStateProcessorComponent>(
		  UCharacterNPCStateProcessorComponent::ComponentName
	  )
  )
{
	AIComponentPtr = CreateDefaultSubobject<UAIComponent>(UAIComponent::ComponentName);

	InteractionWidgetCompoentPtr = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	InteractionWidgetCompoentPtr->SetupAttachment(RootComponent);
}

void AHumanCharacter_AI::BeginPlay()
{
	Super::BeginPlay();

#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		// 组件自动调用条件不成功，原因未知   应该是AIST设置上下文的数据不正确，导致没开始执行
		// 👆
		// AI Comtroller下的需要显式调用
		// if (StateTreeAIComponentPtr && !StateTreeAIComponentPtr->IsRunning())
		auto AIControllerPtr = Cast<AHumanAIController>(GetController());
		if (AIControllerPtr)
		{
			if (auto AISTComponentPtr = AIControllerPtr->GetStateTreeAIComponent())
			{
				AISTComponentPtr->StartLogic();
			}
		}
	}
#endif
}

void AHumanCharacter_AI::PossessedBy(
	AController* NewController
)
{
	Super::PossessedBy(NewController);
}

void AHumanCharacter_AI::SpawnDefaultController()
{
	if (GroupManaggerPtr)
	{
	}
	else
	{
		GetAIComponent()->bIsSingle = true;

		// 单个的NPC
		FActorSpawnParameters SpawnParameters;

		SpawnParameters.Owner = this;
		SpawnParameters.CustomPreSpawnInitalization = [](
			AActor* ActorPtr
		)
			{
				auto GroupManaggerPtr = Cast<AGroupManagger>(ActorPtr);
				if (GroupManaggerPtr)
				{
					GroupManaggerPtr->GroupID = FGuid::NewGuid();
				}
			};

		GroupManaggerPtr = GetWorld()->SpawnActor<AGroupManagger_NPC>(
			AGroupManagger_NPC::StaticClass(),
			SpawnParameters
		);

		SetGroupSharedInfo(GroupManaggerPtr);
	}

	Super::SpawnDefaultController();
}

UCharacterNPCStateProcessorComponent* AHumanCharacter_AI::GetCharacterNPCStateProcessorComponent() const
{
	return Cast<UCharacterNPCStateProcessorComponent>(StateProcessorComponentPtr);
}

AGroupManagger_NPC* AHumanCharacter_AI::GetGroupManagger_NPC() const
{
	return Cast<AGroupManagger_NPC>(GetGroupManagger());
}

void AHumanCharacter_AI::HasBeenStartedLookAt(
	ACharacterBase* InCharacterPtr
)
{
	Super::HasBeenStartedLookAt(InCharacterPtr);

	HasBeenLookingAt(InCharacterPtr);
}

void AHumanCharacter_AI::HasBeenLookingAt(
	ACharacterBase* InCharacterPtr
)
{
	Super::HasBeenLookingAt(InCharacterPtr);

	if (
		InteractionWidgetCompoentPtr &&
		(FVector::Distance(InCharacterPtr->GetActorLocation(), GetActorLocation()) < SceneActorInteractionComponentPtr->
			Range)
	)
	{
		InteractionWidgetCompoentPtr->SetVisibility(true);
	}
	else
	{
		HasBeenEndedLookAt();
	}
}

void AHumanCharacter_AI::HasBeenEndedLookAt()
{
	if (InteractionWidgetCompoentPtr)
	{
		InteractionWidgetCompoentPtr->SetVisibility(false);
	}

	Super::HasBeenEndedLookAt();
}

void AHumanCharacter_AI::SetGroupSharedInfo(
	AGroupManagger* InGroupSharedInfoPtr
)
{
	GroupManaggerPtr = InGroupSharedInfoPtr;

	// if (auto ControllerPtr = GetController<AHumanAIController>())
	// {
	// 	ControllerPtr->SetGroupSharedInfo(InGroupSharedInfoPtr);
	// }

	// OnGroupManaggerReady(GroupManaggerPtr);
}

void AHumanCharacter_AI::SetCharacterID(
	const FGuid& InCharacterID
)
{
	GetCharacterAttributesComponent()->SetCharacterID(InCharacterID);
}

UAIComponent* AHumanCharacter_AI::GetAIComponent() const
{
	return AIComponentPtr;
}

void AHumanCharacter_AI::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AHumanCharacter_AI::OnRep_GroupSharedInfoChanged()
{
	Super::OnRep_GroupSharedInfoChanged();
}

// TSharedPtr<FCharacterProxy> AHumanCharacter_AI::GetCharacterProxy() const
// {
// 	return GetGroupManagger()->GetInventoryComponent()->FindProxy_Character(CharacterID);
// }

void AHumanCharacter_AI::OnGroupManaggerReady(
	AGroupManagger* NewGroupSharedInfoPtr
)
{
	Super::OnGroupManaggerReady(NewGroupSharedInfoPtr);

#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		if (GetParentActor())
		{
		}
		// 如果这个Character是单独的，则直接生成 
		else
		{
		}
		if (GetAIComponent()->bIsSingle && !GetAIComponent()->bIsTeammate)
		{
			GroupManaggerPtr->GetTeamMatesHelperComponent()->SwitchTeammateOption(
				AIComponentPtr->DefaultTeammateOption
			);
			GroupManaggerPtr->SetOwnerCharacterProxyPtr(this);
		}
	}
#endif
}
