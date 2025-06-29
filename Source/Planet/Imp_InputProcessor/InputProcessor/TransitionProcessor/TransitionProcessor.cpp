#include "TransitionProcessor.h"

#include "Blueprint/WidgetBlueprintLibrary.h"

#include "HumanCharacter_Player.h"
#include "UIManagerSubSystem.h"

namespace HumanProcessor
{
	FTransitionProcessor::FTransitionProcessor(
		FOwnerPawnType* CharacterPtr
	) :
	  Super(CharacterPtr)
	{
	}

	void FTransitionProcessor::EnterAction()
	{
		Super::EnterAction();

		UUIManagerSubSystem::GetInstance()->SwitchLayout(ELayoutCommon::kTransitionLayout);

		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
		if (OnwerActorPtr)
		{
			SwitchShowCursor(false);
		}
	}
}