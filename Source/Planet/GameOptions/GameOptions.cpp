
#include "GameOptions.h"

#include <Kismet/GameplayStatics.h>

#include "GameInstance/PlanetGameInstance.h"
#include "StateTagExtendInfo.h"
#include "Planet.h"
#include "PlanetWorldSettings.h"

UGameOptions* UGameOptions::GetInstance()
{
	auto WorldSetting = Cast<APlanetWorldSettings>(GetWorldImp()->GetWorldSettings());
	return WorldSetting->GetGameOptions();
}
