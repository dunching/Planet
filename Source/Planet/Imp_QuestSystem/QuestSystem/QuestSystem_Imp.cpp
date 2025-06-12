#include "QuestSystem_Imp.h"

#include "PlanetWorldSettings.h"
#include "Tools.h"
#include "QuestsActorBase.h"

TArray<TSubclassOf<AQuestChain_MainBase>> UQuestSubSystem_Imp::GetMainGuideThreadChaptersAry() const
{
	auto WorldSetting = Cast<APlanetWorldSettings>(GetWorldImp()->GetWorldSettings());

	return WorldSetting->MainGuideThreadChaptersAry;
}

TSubclassOf<AQuestChain_MainBase> UQuestSubSystem_Imp::GetToBeContinueGuideThread() const
{
	auto WorldSetting = Cast<APlanetWorldSettings>(GetWorldImp()->GetWorldSettings());

	return WorldSetting->ToBeContinueGuideThread;
}
