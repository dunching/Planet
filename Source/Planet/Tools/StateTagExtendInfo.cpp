
#include "StateTagExtendInfo.h"

#include <Kismet/GameplayStatics.h>

#include "GameInstance/PlanetGameInstance.h"
#include "Planet.h"
#include "PlanetWorldSettings.h"

UStateTagExtendInfoMap::UStateTagExtendInfoMap() :
	Super()
{

}

UStateTagExtendInfoMap* UStateTagExtendInfoMap::GetInstance()
{
	auto WorldSetting = Cast<APlanetWorldSettings>(GetWorldImp()->GetWorldSettings());
	return WorldSetting->GetStateTagExtendInfoInstance();
}
