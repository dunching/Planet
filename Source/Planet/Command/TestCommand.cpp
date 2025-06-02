#include "TestCommand.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"
#include "Sound/SoundMix.h"
#include "Sound/SoundClass.h"

#include "AssetRefMap.h"
#include "CharacterAbilitySystemComponent.h"
#include "GameplayTagsLibrary.h"
#include "GroupManagger.h"
#include "GuideSubSystem.h"
#include "GuideThreadChallenge.h"
#include "HumanCharacter_Player.h"
#include "InventoryComponent.h"
#include "PlanetPlayerController.h"
#include "Planet_Tools.h"
#include "Tools.h"

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

void TestCommand::AddPlayerCharacterTestDataImp(
	AHumanCharacter* CharacterPtr
	)
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

void TestCommand::AddAICharacterTestDataImp(
	AHumanCharacter* CharacterPtr
	)
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
	UGuideSubSystem::GetInstance()->ActiveBrandGuideThread(UAssetRefMap::GetInstance()->GuideBranchThreadTestClass);
#endif
}

void TestCommand::ActiveBrandTest1()
{
#if WITH_EDITOR
	UGuideSubSystem::GetInstance()->ActiveBrandGuideThread(UAssetRefMap::GetInstance()->GuideBranchThreadTest1Class);
#endif
}

void TestCommand::ActiveBrandTest2()
{
#if WITH_EDITOR
	UGuideSubSystem::GetInstance()->ActiveBrandGuideThread(UAssetRefMap::GetInstance()->GuideBranchThreadTest2Class);
#endif
}

void TestCommand::ChallengeTest()
{
#if WITH_EDITOR
	UGuideSubSystem::GetInstance()->StartParallelGuideThread(
	                                                         UAssetRefMap::GetInstance()->GuideThreadChallengeActorClass
	                                                        );
#endif
}

void TestCommand::TestHasBeenFlyAway(
	const TArray<FString>& Args
	)
{
#if WITH_EDITOR
	if (!Args.IsValidIndex(0))
	{
		return;
	}

	auto Player = Cast<AHumanCharacter_Player>(UGameplayStatics::GetPlayerCharacter(GetWorldImp(), 0));
	if (Player)
	{
		int32 Height = 10.f;
		LexFromString(Height, *Args[0]);
		Player->GetCharacterAbilitySystemComponent()->HasBeenFlayAway(Height);
	}
#endif
}

void TestCommand::ReplyHP(
	const TArray<FString>& Args
	)
{
}

void TestCommand::IncreaseCD(
	const TArray<FString>& Args
	)
{
#if WITH_EDITOR
	if (!Args.IsValidIndex(0))
	{
		return;
	}

	auto Player = Cast<APlanetPlayerController>(UGameplayStatics::GetPlayerController(GetWorldImp(), 0));
	if (Player)
	{
		int32 CD = UKismetStringLibrary::Conv_StringToInt(Args[0]);
		Player->IncreaseCD(CD);
	}
#endif
}

void TestCommand::AddProxy(
	const TArray<FString>& Args
	)
{
#if WITH_EDITOR
	if (!Args.IsValidIndex(1))
	{
		return;
	}

	auto PCPtr = Cast<APlanetPlayerController>(UGameplayStatics::GetPlayerController(GetWorldImp(), 0));
	if (PCPtr)
	{
		PCPtr->AddProxy(
		                FGameplayTag::RequestGameplayTag(*Args[0]),
		                UKismetStringLibrary::Conv_StringToInt(Args[1])
		               );
	}
#endif
}

void TestCommand::RemoveProxy(
	const TArray<FString>& Args
	)
{
#if WITH_EDITOR
	if (!Args.IsValidIndex(1))
	{
		return;
	}

	auto PCPtr = Cast<APlanetPlayerController>(UGameplayStatics::GetPlayerController(GetWorldImp(), 0));
	if (PCPtr)
	{
		auto InventoryComponentPtr = PCPtr->GetGroupManagger()->GetInventoryComponent();
		if (InventoryComponentPtr)
		{
		}
	}
#endif
}

void TestCommand::SwitchPlayerInput(
	const TArray<FString>& Args
	)
{
#if WITH_EDITOR
	auto PCPtr = Cast<APlanetPlayerController>(UGameplayStatics::GetPlayerController(GetWorldImp(), 0));
	if (PCPtr)
	{
		PCPtr->SwitchPlayerInput(Args);
	}
#endif
}

void TestCommand::AddOrRemoveState(
	const TArray<FString>& Args
	)
{
#if WITH_EDITOR
	if (!Args.IsValidIndex(1))
	{
		return;
	}

	auto PCPtr = Cast<APlanetPlayerController>(UGameplayStatics::GetPlayerController(GetWorldImp(), 0));
	if (PCPtr)
	{
		PCPtr->AddOrRemoveState(
		                        FGameplayTag::RequestGameplayTag(*Args[0]),
		                        static_cast<bool>(UKismetStringLibrary::Conv_StringToInt(Args[1]))
		                       );
	}
#endif
}

void TestCommand::SetBGMVolume(
	const TArray<FString>& Args
	)
{
#if WITH_EDITOR
	if (!Args.IsValidIndex(0))
	{
		return;
	}
	USoundMix;
	USoundClass;
	float Volume = 0.0f;
	LexFromString(Volume, Args[0]);

	UGameplayStatics::SetSoundMixClassOverride(
	                                           GetWorldImp(),
	                                           UAssetRefMap::GetInstance()->SoundMixRef.LoadSynchronous(),
	                                           UAssetRefMap::GetInstance()->BGMSoundClassRef.LoadSynchronous(),
	                                           Volume
	                                          );
#endif
}

void TestCommand::AddExperience(
	const TArray<FString>& Args
	)
{
#if WITH_EDITOR
	if (!Args.IsValidIndex(0))
	{
		return;
	}

	auto PCPtr = Cast<APlanetPlayerController>(UGameplayStatics::GetPlayerController(GetWorldImp(), 0));
	if (PCPtr)
	{
		PCPtr->AddExperience(UKismetStringLibrary::Conv_StringToInt(Args[0]));
	}
#endif
}
