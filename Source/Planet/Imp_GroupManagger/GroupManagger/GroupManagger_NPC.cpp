#include "GroupManagger_NPC.h"

#include "AIComponent.h"
#include "GameOptions.h"
#include "HumanCharacter_AI.h"

inline void AGroupManagger_NPC::AddSpwanedCharacter(
	const TObjectPtr<AHumanCharacter_AI>& SpwanedCharacter
	)
{
	if (SpwanedCharacter)
	{
		SpwanedCharactersSet.Add(SpwanedCharacter);

		auto DelagateHandle = SpwanedCharacter->GetProxyProcessComponent()->OnCloseCombatChanged.AddCallback(
			 std::bind(&ThisClass::OnCloseCombatChanged, this, std::placeholders::_1, std::placeholders::_2)
			);
		DelagateHandle->bIsAutoUnregister = false;
		OnCloseCombatChanged(SpwanedCharacter, SpwanedCharacter->GetProxyProcessComponent()->GetIsCloseCombat());

		SpwanedCharacter->OnDestroyed.AddDynamic(this, &ThisClass::OnSpwanedCharacterDestroyed);
	}
}

void AGroupManagger_NPC::OnCloseCombatChanged(
	ACharacterBase* CharacterPtr,
	bool bIsCloseCombat
	)
{
	auto AICharacterPtr = Cast<AHumanCharacter_AI>(CharacterPtr);
	if (!AICharacterPtr)
	{
		return;
	}

	if (bIsCloseCombat)
	{
		if (CloseCombatCharactersSet.Num() < UGameOptions::GetInstance()->MaxCloseCombat)
		{
			CloseCombatCharactersSet.Add(AICharacterPtr);

			AICharacterPtr->GetAIComponent()->SetIsCheerOn(false);
		}
		else
		{
			AICharacterPtr->GetAIComponent()->SetIsCheerOn(true);
		}
	}
	else
	{
		if (CloseCombatCharactersSet.Contains(AICharacterPtr))
		{
			CloseCombatCharactersSet.Remove(AICharacterPtr);
		}

		AICharacterPtr->GetAIComponent()->SetIsCheerOn(false);
	}
}

void AGroupManagger_NPC::OnSpwanedCharacterDestroyed(
	AActor* DestroyedActor
	)
{
	if (DestroyedActor)
	{
		auto SpwanedCharacter = Cast<AHumanCharacter_AI>(DestroyedActor);
		if (SpwanedCharacter)
		{
			if (SpwanedCharactersSet.Contains(SpwanedCharacter))
			{
				SpwanedCharactersSet.Remove(SpwanedCharacter);
			}
		}
	}

	if (SpwanedCharactersSet.IsEmpty())
	{
		Destroy();
	}
}
