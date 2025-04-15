
#include "TestCommand.h"

#include "AssetRefMap.h"
#include "CharacterAbilitySystemComponent.h"
#include "GuideSubSystem.h"
#include "GuideThreadChallenge.h"
#include "HumanCharacter_Player.h"
#include "Planet_Tools.h"
#include "Kismet/GameplayStatics.h"

// #include "Kismet/GameplayStatics.h"

// #include "HumanCharacter.h"
// #include "Planet.h"
// #include "CharacterBase.h"

void TestCommand::AddCahracterTestData()
{
	// auto CharacterPtr = Cast<AHumanCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorldImp(), 0));
	// if (!CharacterPtr)
	// {
	// 	return;
	// }
	// AddPlayerCharacterTestDataImp(CharacterPtr);
}

void TestCommand::AddPlayerCharacterTestDataImp(AHumanCharacter* CharacterPtr)
{
	{
//		auto& HoldItemComponent = CharacterPtr->GetInventoryComponent()->GetSceneProxyContainer();
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
//		auto& HoldItemComponent = CharacterPtr->GetInventoryComponent()->GetSceneProxyContainer();
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

void TestCommand::ActiveBrandTest()
{
#if WITH_EDITOR
	UGuideSubSystem::GetInstance()->ActiveTargetGuideThread(UAssetRefMap::GetInstance()->GuideBranchThreadTestClass, true);
#endif
}

void TestCommand::ActiveBrandTest1()
{
#if WITH_EDITOR
	UGuideSubSystem::GetInstance()->ActiveTargetGuideThread(UAssetRefMap::GetInstance()->GuideBranchThreadTest1Class, true);
#endif
}

void TestCommand::ActiveBrandTest2()
{
#if WITH_EDITOR
	UGuideSubSystem::GetInstance()->ActiveTargetGuideThread(UAssetRefMap::GetInstance()->GuideBranchThreadTest2Class, true);
#endif
}

void TestCommand::ChallengeTest()
{
#if WITH_EDITOR
	UGuideSubSystem::GetInstance()->ActiveTargetGuideThread(UAssetRefMap::GetInstance()->GuideThreadChallengeActorClass, true);
#endif
}

void TestCommand::TestHasBeenFlyAway(const TArray< FString >& Args)
{
#if WITH_EDITOR
	if (!Args.IsValidIndex(0))
	{
		return;
	}

	auto Player = Cast<AHumanCharacter_Player>(UGameplayStatics::GetPlayerCharacter(GetWorldImp(), 0));
	if (Player )
	{
		int32 Height = 10.f;
		LexFromString(Height, *Args[0]);
		Player ->GetCharacterAbilitySystemComponent()->HasBeenFlayAway(Height);
	}
#endif
}
