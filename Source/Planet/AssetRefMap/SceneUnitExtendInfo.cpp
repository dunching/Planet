
#include "SceneUnitExtendInfo.h"

#include <Kismet/GameplayStatics.h>

#include "GameInstance/PlanetGameInstance.h"
#include "Planet.h"
#include "PlanetWorldSettings.h"

USceneUnitExtendInfoMap::USceneUnitExtendInfoMap() :
	Super()
{
	SkillUnitMap.Add(ESkillUnitType::kHumanSkill_Passive_ZMJZ);
}

void USceneUnitExtendInfoMap::PostCDOContruct()
{
	Super::PostCDOContruct();

	InitialData();
}

USceneUnitExtendInfoMap* USceneUnitExtendInfoMap::GetInstance()
{
	auto WorldSetting = Cast<APlanetWorldSettings>(GetWorldImp()->GetWorldSettings());
	return WorldSetting->GetSceneUnitExtendInfoMap();
}

void USceneUnitExtendInfoMap::InitialData()
{
#pragma region Skill
	{
		FSceneUnitExtendInfoBase SceneUnitExtendInfoBase;
		SceneUnitExtendInfoBase.Guid =
			FGuid(TEXT("{46DF00CA-5D51-46FA-A136-B0895B1D6812}"));

		SkillUnitMap.Add(ESkillUnitType::kHumanSkill_Passive_ZMJZ, SceneUnitExtendInfoBase);
	}
	{
		FSceneUnitExtendInfoBase SceneUnitExtendInfoBase;
		SceneUnitExtendInfoBase.Guid =
			FGuid(TEXT("{E1AF2E92-BBB6-4B5D-97D3-3D7F10957A20}"));

		SkillUnitMap.Add(ESkillUnitType::kHumanSkill_Active_Displacement, SceneUnitExtendInfoBase);
	}
	{
		FSceneUnitExtendInfoBase SceneUnitExtendInfoBase;
		SceneUnitExtendInfoBase.Guid =
			FGuid(TEXT("{30CB1E1B-80B1-476E-837F-3D2BE43831DF}"));

		SkillUnitMap.Add(ESkillUnitType::kHumanSkill_Active_GroupTherapy, SceneUnitExtendInfoBase);
	}
	{
		FSceneUnitExtendInfoBase SceneUnitExtendInfoBase;
		SceneUnitExtendInfoBase.Guid =
			FGuid(TEXT("{4C8652EC-DA76-43E0-85EB-6032E23E6833}"));

		SkillUnitMap.Add(ESkillUnitType::kHumanSkill_Active_ContinuousGroupTherapy, SceneUnitExtendInfoBase);
	}
	{
		FSceneUnitExtendInfoBase SceneUnitExtendInfoBase;
		SceneUnitExtendInfoBase.Guid =
			FGuid(TEXT("{B3B51225-735D-425B-8368-A3955D670FD5}"));

		SkillUnitMap.Add(ESkillUnitType::kHumanSkill_Active_Tornado, SceneUnitExtendInfoBase);
	}
	{
		FSceneUnitExtendInfoBase SceneUnitExtendInfoBase;
		SceneUnitExtendInfoBase.Guid =
			FGuid(TEXT("{2C712123-CFD0-4775-AF12-CC5C6521CE69}"));

		SkillUnitMap.Add(ESkillUnitType::kHumanSkill_Active_FlyAway, SceneUnitExtendInfoBase);
	}
#pragma endregion 

#pragma region Tools
#pragma endregion 

#pragma region Weapon
	{
		FSceneUnitExtendInfoBase SceneUnitExtendInfoBase;
		SceneUnitExtendInfoBase.Guid =
			FGuid(TEXT("{8705713D-8ADA-45D1-84DC-E1B63B9E0AAA}"));

		ToolUnitMap.Add(EToolUnitType::kPickAxe, SceneUnitExtendInfoBase);
	}
#pragma endregion 
}

