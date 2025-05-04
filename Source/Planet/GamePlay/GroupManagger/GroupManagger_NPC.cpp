#include "GroupManagger_NPC.h"

#include "HumanCharacter_AI.h"

inline void AGroupManagger_NPC::AddSpwanedCharacter(
	const TObjectPtr<AHumanCharacter_AI>& SpwanedCharacter
	)
{
	if (SpwanedCharacter)
	{
		SpwanedCharactersSet.Add(SpwanedCharacter);
		SpwanedCharacter->OnDestroyed.AddDynamic(this, &ThisClass::OnSpwanedCharacterDestroyed);
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
