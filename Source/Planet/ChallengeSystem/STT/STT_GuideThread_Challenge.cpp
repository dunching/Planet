#include "STT_GuideThread_Challenge.h"

#include "Kismet/GameplayStatics.h"

#include "CharacterAbilitySystemComponent.h"
#include "HumanCharacter_AI.h"
#include "HumanCharacter_Player.h"
#include "PlanetPlayerController.h"
#include "PlayerGameplayTasks.h"
#include "STE_GuideThread_Challenge.h"
#include "Teleport.h"

EStateTreeRunStatus FSTT_GuideThreadEntryNextLevel::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	const auto Teleport = InstanceData.ChallengeAry[
		FMath::RandRange(0, InstanceData.ChallengeAry.Num() - 1)
	];;
	InstanceData.GloabVariable_Challenge->Teleport = Teleport;

	auto PCPtr = Cast<APlanetPlayerController>(
		UGameplayStatics::GetPlayerController(InstanceData.GuideActorPtr, 0)
	);

	PCPtr->GetGameplayTasksComponent()->EntryChallengeLevel(InstanceData.GloabVariable_Challenge->Teleport);

	return Super::EnterState(Context, Transition);
}

EStateTreeRunStatus FSTT_GuideThreadEntryNextLevel::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (
		UOpenWorldSubSystem::GetInstance()->CheckSwitchDataLayerComplete(InstanceData.GloabVariable_Challenge->Teleport) &&
		UOpenWorldSubSystem::GetInstance()->CheckTeleportPlayerComplete(InstanceData.GloabVariable_Challenge->Teleport)
	)
	{
		return EStateTreeRunStatus::Succeeded;
	}

	return Super::Tick(Context, DeltaTime);
}

EStateTreeRunStatus FSTT_GuideThread_Challenge_SpawnNPCs::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (InstanceData.PerWaveNum.IsEmpty())
	{
		return EStateTreeRunStatus::Failed;
	}
	else
	{
		SpawnNPC(Context);

		return Super::EnterState(Context, Transition);
	}
}

EStateTreeRunStatus FSTT_GuideThread_Challenge_SpawnNPCs::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	InstanceData.GuideActorPtr->UpdateCurrentTaskNode(
		GetTaskNodeDescripton(
			Context
		)
	);
	if (InstanceData.CharacterAry.Num() < InstanceData.CharacterIDAry.Num())
	{
		// 获取对应的Characers
		TArray<AActor*> OutActors;
		UGameplayStatics::GetAllActorsOfClass(InstanceData.GuideActorPtr, AHumanCharacter_AI::StaticClass(), OutActors);
		for (auto Iter : OutActors)
		{
			auto CharacterPtr = Cast<AHumanCharacter_AI>(Iter);
			if (
				CharacterPtr &&
				InstanceData.CharacterIDAry.Contains(CharacterPtr->GetCharacterAttributesComponent()->GetCharacterID())
			)
			{
				InstanceData.CharacterAry.Add(CharacterPtr);
			}
		}
		if (InstanceData.CharacterAry.Num() < InstanceData.CharacterIDAry.Num())
		{
			InstanceData.CharacterAry.Empty();
		}
		else
		{
			for (auto Iter : InstanceData.CharacterAry)
			{
				InstanceData.GloabVariable_Challenge->TemporaryActorAry.Add(Iter.Get());
			}
		}
	}
	else
	{
		// 等待消亡
		for (auto Iter : InstanceData.CharacterAry)
		{
			if (Iter.IsValid())
			{
				if (Iter->GetCharacterAbilitySystemComponent()->IsInDeath())
				{
				}
				else
				{
					return Super::Tick(Context, DeltaTime);
				}
			}
			else
			{
			}
		}

		if (InstanceData.WaveIndex < InstanceData.PerWaveNum.Num())
		{
			InstanceData.CharacterAry.Empty();
			SpawnNPC(Context);
		}
		else
		{
			return EStateTreeRunStatus::Succeeded;
		}
	}

	return Super::Tick(Context, DeltaTime);
}

void FSTT_GuideThread_Challenge_SpawnNPCs::ExitState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.GloabVariable_Challenge)
	{
		auto PCPtr = Cast<APlanetPlayerController>(
			UGameplayStatics::GetPlayerController(InstanceData.GuideActorPtr, 0)
		);
		if (PCPtr)
		{
			for (auto Iter : InstanceData.GloabVariable_Challenge->TemporaryActorAry)
			{
				if (Iter)
				{
					PCPtr->ServerDestroyActor(Iter);
				}
			}
			InstanceData.GloabVariable_Challenge->TemporaryActorAry.Empty();
		}
	}

	Super::ExitState(Context, Transition);
}

bool FSTT_GuideThread_Challenge_SpawnNPCs::SpawnNPC(
	FStateTreeExecutionContext& Context
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.WaveIndex < InstanceData.PerWaveNum.Num())
	{
		auto PCPtr = Cast<
			APlanetPlayerController>(UGameplayStatics::GetPlayerController(InstanceData.GuideActorPtr, 0));
		if (PCPtr)
		{
			const auto& NPCAry = InstanceData.PerWaveNum[InstanceData.WaveIndex].NPCAry;
			const auto Pts = UOpenWorldSubSystem::GetInstance()->GetChallengeSpawnPts(
				InstanceData.GloabVariable_Challenge->Teleport,
				NPCAry.Num()
			);

			if (Pts.Num() == NPCAry.Num())
			{
				InstanceData.CharacterIDAry.SetNum(Pts.Num());
				for (int32 Index = 0; Index < NPCAry.Num(); Index++)
				{
					InstanceData.CharacterIDAry[Index] = FGuid::NewGuid();
					PCPtr->ServerSpawnCharacter(
						NPCAry[Index],
						InstanceData.CharacterIDAry[Index],
						Pts[Index]
					);
				}

				InstanceData.WaveIndex++;
				return true;
			}
			else
			{
				checkNoEntry();
			}
		}
	}

	return false;
}

FTaskNodeDescript FSTT_GuideThread_Challenge_SpawnNPCs::GetTaskNodeDescripton(
	FStateTreeExecutionContext& Context
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);

	FTaskNodeDescript TaskNodeDescript;

	if (InstanceData.CharacterAry.Num() < InstanceData.CharacterIDAry.Num())
	{
		TaskNodeDescript.Description = FString::Printf(
			TEXT(
				"击败敌人（0/%d）,第%d/%d波敌人"
			),
			InstanceData.CharacterIDAry.Num(),
			InstanceData.WaveIndex,
			InstanceData.PerWaveNum.Num()
		);
	}
	else
	{
		int32 Num = 0;
		for (auto Iter : InstanceData.CharacterAry)
		{
			if (Iter.IsValid() && Iter->GetCharacterAbilitySystemComponent()->IsInDeath())
			{
				Num++;
			}
		}

		TaskNodeDescript.Description = FString::Printf(
			TEXT(
				"击败敌人（%d/%d）,第%d/%d波敌人"
			),
			Num,
			InstanceData.CharacterIDAry.Num(),
			InstanceData.WaveIndex,
			InstanceData.PerWaveNum.Num()
		);
	}

	return TaskNodeDescript;
}
