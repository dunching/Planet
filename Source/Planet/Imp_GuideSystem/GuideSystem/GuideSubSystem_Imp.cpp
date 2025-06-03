#include "GuideSubSystem_Imp.h"

#include "PlanetWorldSettings.h"
#include "Tools.h"
#include "GuideThreadBase.h"
#include "GuideThread.h"

TArray<TSubclassOf<AGuideThread_MainBase>> UGuideSubSystem_Imp::GetMainGuideThreadChaptersAry() const
{
	auto WorldSetting = Cast<APlanetWorldSettings>(GetWorldImp()->GetWorldSettings());

	return WorldSetting->MainGuideThreadChaptersAry;
}

TSubclassOf<AGuideThread_MainBase> UGuideSubSystem_Imp::GetToBeContinueGuideThread() const
{
	auto WorldSetting = Cast<APlanetWorldSettings>(GetWorldImp()->GetWorldSettings());

	return WorldSetting->ToBeContinueGuideThread;
}
