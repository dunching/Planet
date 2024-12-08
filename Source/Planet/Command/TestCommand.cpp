
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
#include "ItemProxy_Minimal.h"
#include "Skill_Base.h"
#include "Talent_FASI.h"
#include "TalentAllocationComponent.h"
#include "CharacterBase.h"
#include "CollisionDataStruct.h"
#include "GroupMnaggerComponent.h"
#include "CharacterAttributesComponent.h"
#include "CharacterAttibutes.h"
#include "PlanetControllerInterface.h"
#include "BaseFeatureComponent.h"
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
//		auto& HoldItemComponent = CharacterPtr->GetHoldingItemsComponent()->GetSceneProxyContainer();
// 		HoldItemComponent.AddProxy(EToolProxyType::kPickAxe);
// 
// 		HoldItemComponent.AddProxy(EWeaponProxyType::kPickAxe);
// 		HoldItemComponent.AddProxy(EWeaponProxyType::kWeaponHandProtection);
// 		HoldItemComponent.AddProxy(EWeaponProxyType::kRangeTest);
// 
// 		HoldItemComponent.AddProxy(ESkillProxyType::kHumanSkill_Passive_ZMJZ);
// 		HoldItemComponent.AddProxy(ESkillProxyType::kHumanSkill_Active_Displacement);
// 		HoldItemComponent.AddProxy(ESkillProxyType::kHumanSkill_Talent_NuQi);
// 		HoldItemComponent.AddProxy(ESkillProxyType::kHumanSkill_Active_GroupTherapy);
// 		HoldItemComponent.AddProxy(ESkillProxyType::kHumanSkill_Active_ContinuousGroupTherapy);
// 		HoldItemComponent.AddProxy(ESkillProxyType::kHumanSkill_Active_Tornado);
// 
// 		HoldItemComponent.AddProxy(EConsumableProxyType::kGeneric_HP, 1);
// 		HoldItemComponent.AddProxy(EConsumableProxyType::kGeneric_PP, 1);
	}
}

void TestCommand::AddAICharacterTestDataImp(AHumanCharacter* CharacterPtr)
{
	{
//		auto& HoldItemComponent = CharacterPtr->GetHoldingItemsComponent()->GetSceneProxyContainer();
// 		HoldItemComponent.AddProxy(EToolProxyType::kPickAxe);
// 
// 		HoldItemComponent.AddProxy(EWeaponProxyType::kPickAxe);
// 		HoldItemComponent.AddProxy(EWeaponProxyType::kWeaponHandProtection);
// 		HoldItemComponent.AddProxy(EWeaponProxyType::kRangeTest);
// 
// 		HoldItemComponent.AddProxy(ESkillProxyType::kHumanSkill_Passive_ZMJZ);
// 		HoldItemComponent.AddProxy(ESkillProxyType::kHumanSkill_Active_Displacement);
// 		HoldItemComponent.AddProxy(ESkillProxyType::kHumanSkill_Talent_NuQi);
// 		HoldItemComponent.AddProxy(ESkillProxyType::kHumanSkill_Active_GroupTherapy);
// 		HoldItemComponent.AddProxy(ESkillProxyType::kHumanSkill_Active_ContinuousGroupTherapy);
	}
}
