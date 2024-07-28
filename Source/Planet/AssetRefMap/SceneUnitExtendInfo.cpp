
#include "SceneUnitExtendInfo.h"

#include <Kismet/GameplayStatics.h>

#include "GameInstance/PlanetGameInstance.h"
#include "Planet.h"
#include "PlanetWorldSettings.h"

USceneUnitExtendInfoMap::USceneUnitExtendInfoMap() :
	Super()
{
}

void USceneUnitExtendInfoMap::PostCDOContruct()
{
	Super::PostCDOContruct();

	InitialData();

	if (DataTable)
	{
		FTableRowUnit SceneUnitExtendInfoBase;
		DataTable.Get()->AddRow(TEXT("{"), SceneUnitExtendInfoBase);
	}
}

USceneUnitExtendInfoMap* USceneUnitExtendInfoMap::GetInstance()
{
	auto WorldSetting = Cast<APlanetWorldSettings>(GetWorldImp()->GetWorldSettings());
	return WorldSetting->GetSceneUnitExtendInfoMap();
}

void USceneUnitExtendInfoMap::InitialData()
{
}

