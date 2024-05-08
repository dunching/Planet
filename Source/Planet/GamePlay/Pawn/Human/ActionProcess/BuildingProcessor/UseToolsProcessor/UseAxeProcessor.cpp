
#include "UseAxeProcessor.h"

HumanProcessor::FUseAxeProcessor::FUseAxeProcessor(FOwnerPawnType* CharacterPtr):
	Super(CharacterPtr)
{
}

void HumanProcessor::FUseAxeProcessor::EnterAction()
{
	Super::EnterAction();
}

void HumanProcessor::FUseAxeProcessor::QuitAction()
{
	Super::QuitAction();
}

void HumanProcessor::FUseAxeProcessor::BeginDestroy()
{
	Super::BeginDestroy();
}

