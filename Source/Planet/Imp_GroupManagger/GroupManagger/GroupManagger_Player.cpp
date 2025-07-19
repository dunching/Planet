#include "GroupManagger_Player.h"

#include "AssetRefMap.h"
#include "HumanCharacter_AI.h"
#include "TeamMates_GenericType.h"
#include "AIComponent.h"
#include "DataTableCollection.h"
#include "GroupManagger_NPC.h"
#include "LogWriter.h"
#include "TeamMatesHelperComponent.h"
#include "GeneratorColony_ByInvoke.h"
#include "PlanetPlayerController.h"

void AGroupManagger_Player::CloneCharacter_Server_Implementation(
	const FGuid& ID,
	const FTransform& Transform,
	ETeammateOption TeammateOption
	)
{
	FActorSpawnParameters SpawnParameters;

	SpawnParameters.Owner = this;
	SpawnParameters.CustomPreSpawnInitalization = [this, ID, TeammateOption](
		auto ActorPtr
		)
		{
			auto AICharacterPtr = Cast<AHumanCharacter_AI>(ActorPtr);
			if (AICharacterPtr)
			{
				AICharacterPtr->GetAIComponent()->SetCustomCustomTeammateOption(TeammateOption);

				AICharacterPtr->GetCharacterAttributesComponent()->SetCharacterID(ID);

				AICharacterPtr->SetGroupSharedInfo(this);
			}
		};

	auto Result =
		GetWorld()->SpawnActor<AHumanCharacter_AI>(
		                                           UAssetRefMap::GetInstance()->CopyCharacterClass,
		                                           Transform,
		                                           SpawnParameters
		                                          );
	if (Result)
	{
	}
}

void AGroupManagger_Player::SpawnGeneratorActor_Server_Implementation(
	const TSoftObjectPtr<AGeneratorColony_ByInvoke>& GeneratorBasePtr
	)
{
	GeneratorBasePtr->SpawnGeneratorActor();
}

void AGroupManagger_Player::DestroyActor_Server_Implementation(
	AActor* ActorPtr
	)
{
	if (ActorPtr)
	{
		ActorPtr->Destroy();
	}
}

void AGroupManagger_Player::SpawnCharacterAry_Server_Implementation(
	const TArray<TSubclassOf<AHumanCharacter_AI>>& CharacterClassAry,
	const TArray<FGuid>& IDAry,
	const TArray<FTransform>& TransformAry,
	ETeammateOption TeammateOption
	)
{
	FActorSpawnParameters GroupManaggerSpawnParameters;

	GroupManaggerSpawnParameters.Owner = this;
	GroupManaggerSpawnParameters.CustomPreSpawnInitalization = [](
		AActor* ActorPtr
		)
		{
			PRINTINVOKEINFO();
			auto GroupManaggerPtr = Cast<AGroupManagger>(ActorPtr);
			if (GroupManaggerPtr)
			{
				GroupManaggerPtr->GroupID = FGuid::NewGuid();
			}
		};

	auto GroupManagger_NPCPtr = GetWorld()->SpawnActor<AGroupManagger_NPC>(
	                                                                       AGroupManagger_NPC::StaticClass(),
	                                                                       GroupManaggerSpawnParameters
	                                                                      );
	GroupManagger_NPCPtr->GetTeamMatesHelperComponent()->SwitchTeammateOption(
	                                                                          TeammateOption
	                                                                         );

	if ((CharacterClassAry.Num() == IDAry.Num()) && (IDAry.Num() == TransformAry.Num()))
	{
		for (int32 Index = 0; Index < CharacterClassAry.Num(); Index++)
		{
			FActorSpawnParameters SpawnParameters;

			SpawnParameters.CustomPreSpawnInitalization = [this, &IDAry, Index, GroupManagger_NPCPtr](
				auto ActorPtr
				)
				{
					auto AICharacterPtr = Cast<AHumanCharacter_AI>(ActorPtr);
					if (AICharacterPtr)
					{
						AICharacterPtr->SetGroupSharedInfo(GroupManagger_NPCPtr);
						AICharacterPtr->GetCharacterAttributesComponent()->SetCharacterID(IDAry[Index]);
					}
				};

			auto AICharacterPtr =
				GetWorld()->SpawnActor<AHumanCharacter_AI>(
				                                           CharacterClassAry[Index],
				                                           TransformAry[Index],
				                                           SpawnParameters
				                                          );
			if (AICharacterPtr)
			{
				GroupManagger_NPCPtr->GetTeamMatesHelperComponent()->UpdateTeammateConfig(
					 AICharacterPtr->GetCharacterProxy(),
					 Index
					);
				GroupManagger_NPCPtr->AddSpwanedCharacter(AICharacterPtr);
			}
		}
	}
}

void AGroupManagger_Player::SpawnCharacter_Server_Implementation(
	TSubclassOf<AHumanCharacter_AI> CharacterClass,
	const FGuid& ID,
	const FTransform& Transform,
	ETeammateOption TeammateOption
	)
{
	FActorSpawnParameters SpawnParameters;

	SpawnParameters.CustomPreSpawnInitalization = [this, ID](
		auto ActorPtr
		)
		{
			auto AICharacterPtr = Cast<AHumanCharacter_AI>(ActorPtr);
			if (AICharacterPtr)
			{
				AICharacterPtr->GetAIComponent()->bIsTeammate = true;
				AICharacterPtr->GetCharacterAttributesComponent()->SetCharacterID(ID);
				AICharacterPtr->SetGroupSharedInfo(this);
			}
		};

	auto Result =
		GetWorld()->SpawnActor<AHumanCharacter_AI>(
		                                           CharacterClass,
		                                           Transform,
		                                           SpawnParameters
		                                          );
	if (Result)
	{
		Result->GetGroupManagger_NPC()->GetTeamMatesHelperComponent()->SwitchTeammateOption(TeammateOption);
	}
}

inline void AGroupManagger_Player::SpawnCharacterByProxyType_Server_Implementation(
	const FGameplayTag& CharacterProxyType,
	const FTransform& Transform
	)
{
	auto ItemProxy_Description_CharacterPtr = UDataTableCollection::GetInstance()->GetTableRowProxyDescription<
		UItemProxy_Description_Character>(CharacterProxyType);

	if (ItemProxy_Description_CharacterPtr)
	{
		FActorSpawnParameters SpawnParameters;

		SpawnParameters.CustomPreSpawnInitalization = [this](
			auto ActorPtr
			)
			{
				auto AICharacterPtr = Cast<AHumanCharacter_AI>(ActorPtr);
				if (AICharacterPtr)
				{
				}
			};

		auto Result =
			GetWorld()->SpawnActor<AHumanCharacter_AI>(
			                                           ItemProxy_Description_CharacterPtr->CharacterClass,
			                                           Transform,
			                                           SpawnParameters
			                                          );
		if (Result)
		{
			// 似乎感知不到玩家？
			Result->GetGroupManagger_NPC()->GetTeamMatesHelperComponent()->AddKnowCharacter(
				 GetOwner<FOwnerType>()->GetPawn<ACharacterBase>()
				);
		}
	}
}
