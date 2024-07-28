
#include "TestCommand.h"

#include "Kismet/GameplayStatics.h"

#include "Engine/StreamableManager.h"
#include "MyUserWidget.h"
#include "Engine/AssetManager.h"
#include <Subsystems/SubsystemBlueprintLibrary.h>
#include "Components/SplineComponent.h"
#include "GameplayEffect.h"
#include "Kismet/KismetStringLibrary.h"

#include "HumanCharacter.h"
#include "GameInstance/PlanetGameInstance.h"
#include "HoldingItemsComponent.h"
#include "AssetRefMap.h"
#include "Planet.h"
#include "HumanCharacter.h"
#include "SPlineActor.h"
#include "SceneElement.h"
#include "Skill_Base.h"
#include "TalentUnit.h"
#include "TalentAllocationComponent.h"
#include "CharacterBase.h"
#include "CollisionDataStruct.h"
#include "GroupMnaggerComponent.h"
#include "CharacterAttributesComponent.h"
#include "CharacterAttibutes.h"
#include "HumanControllerInterface.h"
#include "InteractiveBaseGAComponent.h"
#include "HorseCharacter.h"

void TestCommand::AddCahracterTestData()
{
	auto CharacterPtr = Cast<AHumanCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorldImp(), 0));
	if (!CharacterPtr)
	{
		return;
	}
	AddPlayerCharacterTestDataImp(CharacterPtr);
}

void TestCommand::AddPlayerCharacterTestDataImp(AHumanCharacter* CharacterPtr)
{
	{
		auto& HoldItemComponent = CharacterPtr->GetHoldingItemsComponent()->GetHoldItemProperty();
// 		HoldItemComponent.AddUnit(EToolUnitType::kPickAxe);
// 
// 		HoldItemComponent.AddUnit(EWeaponUnitType::kPickAxe);
// 		HoldItemComponent.AddUnit(EWeaponUnitType::kWeaponHandProtection);
// 		HoldItemComponent.AddUnit(EWeaponUnitType::kRangeTest);
// 
// 		HoldItemComponent.AddUnit(ESkillUnitType::kHumanSkill_Passive_ZMJZ);
// 		HoldItemComponent.AddUnit(ESkillUnitType::kHumanSkill_Active_Displacement);
// 		HoldItemComponent.AddUnit(ESkillUnitType::kHumanSkill_Talent_NuQi);
// 		HoldItemComponent.AddUnit(ESkillUnitType::kHumanSkill_Active_GroupTherapy);
// 		HoldItemComponent.AddUnit(ESkillUnitType::kHumanSkill_Active_ContinuousGroupTherapy);
// 		HoldItemComponent.AddUnit(ESkillUnitType::kHumanSkill_Active_Tornado);
// 
// 		HoldItemComponent.AddUnit(EConsumableUnitType::kGeneric_HP, 1);
// 		HoldItemComponent.AddUnit(EConsumableUnitType::kGeneric_PP, 1);
	}
}

void TestCommand::AddAICharacterTestDataImp(AHumanCharacter* CharacterPtr)
{
	{
		auto& HoldItemComponent = CharacterPtr->GetHoldingItemsComponent()->GetHoldItemProperty();
// 		HoldItemComponent.AddUnit(EToolUnitType::kPickAxe);
// 
// 		HoldItemComponent.AddUnit(EWeaponUnitType::kPickAxe);
// 		HoldItemComponent.AddUnit(EWeaponUnitType::kWeaponHandProtection);
// 		HoldItemComponent.AddUnit(EWeaponUnitType::kRangeTest);
// 
// 		HoldItemComponent.AddUnit(ESkillUnitType::kHumanSkill_Passive_ZMJZ);
// 		HoldItemComponent.AddUnit(ESkillUnitType::kHumanSkill_Active_Displacement);
// 		HoldItemComponent.AddUnit(ESkillUnitType::kHumanSkill_Talent_NuQi);
// 		HoldItemComponent.AddUnit(ESkillUnitType::kHumanSkill_Active_GroupTherapy);
// 		HoldItemComponent.AddUnit(ESkillUnitType::kHumanSkill_Active_ContinuousGroupTherapy);
	}
}
