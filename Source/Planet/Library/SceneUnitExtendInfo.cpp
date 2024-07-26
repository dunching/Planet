
#include "SceneUnitExtendInfo.h"

#include <Kismet/GameplayStatics.h>

#include "GameInstance/PlanetGameInstance.h"
#include "Planet.h"
#include "PlanetWorldSettings.h"

USceneUnitExtendInfoMap::USceneUnitExtendInfoMap() :
	Super()
{
	{
		TSharedPtr<FSceneUnitExtendInfoBase> SceneUnitExtendInfoBaseSPtr = MakeShared<FSceneUnitExtendInfoBase>();
		SceneUnitExtendInfoBaseSPtr->Guid = FGuid(TEXT("{46DF00CA-5D51-46FA-A136-B0895B1D6812}"));

		SkillUnitMap.Add(ESkillUnitType::kHumanSkill_Active_Displacement, SceneUnitExtendInfoBaseSPtr);
	}
}

USceneUnitExtendInfoMap* USceneUnitExtendInfoMap::GetInstance()
{
	auto WorldSetting = Cast<APlanetWorldSettings>(GetWorldImp()->GetWorldSettings());
	return WorldSetting->GetSceneUnitExtendInfoMap();
}

