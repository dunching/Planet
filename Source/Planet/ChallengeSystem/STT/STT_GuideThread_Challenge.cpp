#include "STT_GuideThread_Challenge.h"

#include "Kismet/GameplayStatics.h"

#include "CharacterAbilitySystemComponent.h"
#include "GuideThreadChallenge.h"
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

	const auto CurrentLevel = InstanceData.ChallengeGuideTrheadActorPtr->GetCurrentLevel() + 1;
	InstanceData.ChallengeGuideTrheadActorPtr->SetCurrentLevel(CurrentLevel);

	InstanceData.GloabVariable_Challenge->Teleport = ETeleport::kNone;
	for (const auto& Iter : InstanceData.ChallengeGuideTrheadActorPtr->SpecifySpecialLevels)
	{
		if ((CurrentLevel % Iter.Key) == 0)
		{
			InstanceData.GloabVariable_Challenge->Teleport = Iter.Value;
			break;
		}
	}

	if (InstanceData.GloabVariable_Challenge->Teleport == ETeleport::kNone)
	{
		InstanceData.GloabVariable_Challenge->Teleport = InstanceData.ChallengeAry[
			FMath::RandRange(0, InstanceData.ChallengeAry.Num() - 1)
		];
	}

	auto PCPtr = Cast<APlanetPlayerController>(
	                                           UGameplayStatics::GetPlayerController(
		                                            InstanceData.GuideThreadActorPtr,
		                                            0
		                                           )
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
		UOpenWorldSubSystem::GetInstance()->CheckSwitchDataLayerComplete(InstanceData.GloabVariable_Challenge->Teleport)
		&&
		UOpenWorldSubSystem::GetInstance()->CheckTeleportPlayerComplete(InstanceData.GloabVariable_Challenge->Teleport)
	)
	{
		auto GuideThread_ChallengePtr = Cast<AGuideThread_Challenge>(InstanceData.GuideThreadActorPtr);
		if (GuideThread_ChallengePtr)
		{
			GuideThread_ChallengePtr->CurrentTeleport = InstanceData.GloabVariable_Challenge->Teleport;
		}
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
		InstanceData.RemainTime = InstanceData.DelayTime;

		return Super::EnterState(Context, Transition);
	}
}

EStateTreeRunStatus FSTT_GuideThread_Challenge_SpawnNPCs::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime
	) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (InstanceData.RemainTime > 0)
	{
		InstanceData.RemainTime -= DeltaTime;
	}
	
	if (InstanceData.RemainTime > 0)
	{
		if (InstanceData.GuideThreadActorPtr)
		{
			InstanceData.GuideThreadActorPtr->UpdateCurrentTaskNode(
			                                                        GetTaskNodeDescripton(
				                                                         Context
				                                                        )
			                                                       );
		}
	}
	else
	{
		if (InstanceData.GuideThreadActorPtr)
		{
			InstanceData.GuideThreadActorPtr->UpdateCurrentTaskNode(
																	GetTaskNodeDescripton(
																		 Context
																		)
																   );
		}

		if (InstanceData.CharacterIDAry.IsEmpty())
		{
			SpawnNPC(Context);
		}
		else
		{
			// 获取对应的Characers
			if (InstanceData.CharacterAry.Num() < InstanceData.CharacterIDAry.Num())
			{
				TArray<AActor*> OutActors;
				UGameplayStatics::GetAllActorsOfClass(
													  InstanceData.GuideThreadActorPtr,
													  AHumanCharacter_AI::StaticClass(),
													  OutActors
													 );
				for (auto Iter : OutActors)
				{
					auto CharacterPtr = Cast<AHumanCharacter_AI>(Iter);
					if (
						CharacterPtr &&
						InstanceData.CharacterIDAry.Contains(
															 CharacterPtr->GetCharacterAttributesComponent()->
																		   GetCharacterID()
															)
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
			// 等待消亡
			else
			{
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
					InstanceData.CharacterIDAry.Empty();
				
					InstanceData.RemainTime = InstanceData.DelayTime;
				}
				else
				{
					return EStateTreeRunStatus::Succeeded;
				}
			}
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
		                                           UGameplayStatics::GetPlayerController(
			                                            InstanceData.GuideThreadActorPtr,
			                                            0
			                                           )
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
			APlanetPlayerController>(UGameplayStatics::GetPlayerController(InstanceData.GuideThreadActorPtr, 0));
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

	if (InstanceData.RemainTime > 0)
	{
		TaskNodeDescript.Description =  FString::Printf(TEXT("%.0lf秒后生成下一波敌人"), InstanceData.RemainTime);
	}
	else
	{
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
				if (Iter.IsValid())
				{
					if (Iter->GetCharacterAbilitySystemComponent()->IsInDeath())
					{
						Num++;
					}
				}
				else
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
	}

	return TaskNodeDescript;
}
