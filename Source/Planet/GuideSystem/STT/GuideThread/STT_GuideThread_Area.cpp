#include "STT_GuideThread_Area.h"

#include "Kismet/GameplayStatics.h"

#include "HumanCharacter_Player.h"

#include "GuideActor.h"
#include "GuideSystemGameplayTask.h"
#include "GuideThread.h"
#include "GuideThreadGameplayTask.h"
#include "InventoryComponent.h"
#include "HumanCharacter_AI.h"
#include "OpenWorldSystem.h"
#include "PlanetPlayerController.h"
#include "Planet_Tools.h"
#include "AreaVolume.h"
#include "AssetRefMap.h"
#include "CharacterAbilitySystemComponent.h"
#include "CharacterAttributesComponent.h"
#include "GeneratorColony.h"
#include "OpenWorldDataLayer.h"
#include "PlanetChildActorComponent.h"
#include "SceneActor.h"
#include "STE_GuideThread.h"
#include "TargetPoint_Runtime.h"
#include "Engine/TargetPoint.h"

EStateTreeRunStatus FSTT_GuideThreadSpwanNPCColony::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);

	if (auto PAD = InstanceData.PAD.LoadSynchronous())
	{
		auto PCPtr = Cast<
			APlanetPlayerController>(
			UGameplayStatics::GetPlayerController(
				InstanceData.GuideActorPtr,
				0
			)
		);
		if (PCPtr)
		{
			PCPtr->ServerSpawnGeneratorActor(
				PAD->ActorPtr
			);
		}

		return Super::EnterState(
			Context,
			Transition
		);
	}

	return EStateTreeRunStatus::Failed;
}

EStateTreeRunStatus FSTT_GuideThreadSpwanNPCColony::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);

	if (InstanceData.PAD && InstanceData.PAD->ActorPtr.IsValid())
	{
		InstanceData.GloabVariable_Area->SpwanedCharacterAry.Empty();

		auto Components = InstanceData.PAD->ActorPtr->GetComponents();
		for (auto Iter : Components)
		{
			if (Iter && Iter->IsA(
				UPlanetChildActorComponent::StaticClass()
			))
			{
				auto PlanetChildActorComponentPtr = Cast<UPlanetChildActorComponent>(
					Iter
				);
				auto ChildActorPtr = Cast<ACharacterBase>(
					PlanetChildActorComponentPtr->GetChildActor()
				);
				if (ChildActorPtr)
				{
					InstanceData.GloabVariable_Area->SpwanedCharacterAry.Add(
						ChildActorPtr
					);

					InstanceData.GloabVariable_Area->TemporaryActorAry.Add(
						ChildActorPtr
					);
				}
				else
				{
					// 未同步到client
					return Super::Tick(
						Context,
						DeltaTime
					);
				}
			}
		}

		return EStateTreeRunStatus::Succeeded;
	}

	return Super::Tick(
		Context,
		DeltaTime
	);
}

EStateTreeRunStatus FSTT_GuideThreadWaitCharacterDie::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);

	if (InstanceData.GloabVariable_Area->SpwanedCharacterAry.IsEmpty())
	{
		return EStateTreeRunStatus::Failed;
	}

	return Super::EnterState(
		Context,
		Transition
	);
}

EStateTreeRunStatus FSTT_GuideThreadWaitCharacterDie::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);

	if (InstanceData.GloabVariable_Area->SpwanedCharacterAry.IsEmpty())
	{
		return EStateTreeRunStatus::Failed;
	}
	else
	{
		for (auto Iter : InstanceData.GloabVariable_Area->SpwanedCharacterAry)
		{
			if (Iter.IsValid())
			{
				const auto bIsDeath = Iter->GetCharacterAbilitySystemComponent()->IsInDeath();
				if (bIsDeath)
				{
				}
				else
				{
					return Super::Tick(
						Context,
						DeltaTime
					);
				}
			}
		}
	}

	return EStateTreeRunStatus::Succeeded;
}
