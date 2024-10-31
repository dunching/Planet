
#include "HumanEndangeredProcessor.h"

#include "DrawDebugHelpers.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Camera/CameraComponent.h"
#include "AIController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include <Kismet/GameplayStatics.h>
#include <Subsystems/SubsystemBlueprintLibrary.h>
#include "Async/Async.h"
#include "Components/CapsuleComponent.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "AbilitySystemBlueprintLibrary.h"
#include <Blueprint/WidgetBlueprintLibrary.h>

#include "LogHelper/LogWriter.h"
#include "ThreadPoolHelper/ThreadLibrary.h"
#include "ToolsLibrary.h"
#include "Animation/AnimInstanceBase.h"
#include "GenerateType.h"
#include "PlanetPlayerState.h"
#include "CharacterBase.h"
#include "ArticleBase.h"
#include "HumanCharacter_Player.h"
#include "PlacingBuildingAreaProcessor.h"
#include "ActionTrackVehiclePlace.h"
#include "PlacingWallProcessor.h"
#include "PlacingGroundProcessor.h"
#include "ProxyProcessComponent.h"
#include "ToolsMenu.h"
#include <BackpackMenu.h>
#include "UIManagerSubSystem.h"
#include <Character/GravityMovementComponent.h>
#include <DestroyProgress.h>
#include "InputProcessorSubSystem.h"
#include "HumanViewBackpackProcessor.h"
#include "HorseProcessor.h"
#include "HumanProcessor.h"
#include "Planet.h"
#include "PlanetGameInstance.h"
#include "HorseCharacter.h"
#include "PlanetGameplayAbility.h"
#include "AssetRefMap.h"
#include "BuildingBaseProcessor.h"
#include "CharacterAttributesComponent.h"
#include "PlanetPlayerController.h"
#include "HumanViewAlloctionSkillsProcessor.h"
#include "CollisionDataStruct.h"
#include "ToolFuture_PickAxe.h"
#include "Weapon_Base.h"
#include "Skill_Base.h"
#include "Skill_WeaponActive_PickAxe.h"
#include "Skill_WeaponActive_HandProtection.h"
#include "Weapon_HandProtection.h"
#include "Weapon_PickAxe.h"
#include "HumanViewTalentAllocation.h"
#include "HumanViewGroupManagger.h"
#include "GroupMnaggerComponent.h"

#include "GameplayTagsSubSystem.h"
#include "BasicFutures_Mount.h"
#include "BasicFutures_Dash.h"
#include "HumanViewRaffleMenu.h"
#include "BaseFeatureComponent.h"
#include "HumanCharacter_Player.h"

#include "ResourceBox.h"

static TAutoConsoleVariable<int32> HumanEndangeredProcessor(
	TEXT("Skill.DrawDebug.HumanEndangeredProcessor"),
	0,
	TEXT("")
	TEXT(" default: 0"));

namespace HumanProcessor
{
	FHumanEndangeredProcessor::FHumanEndangeredProcessor(FOwnerPawnType* CharacterPtr) :
		Super(CharacterPtr)
	{
	}

	FHumanEndangeredProcessor::~FHumanEndangeredProcessor()
	{
	}

	void FHumanEndangeredProcessor::EnterAction()
	{
		Super::EnterAction();

		UUIManagerSubSystem::GetInstance()->DisplayEndangeredState(true);
	}

	void FHumanEndangeredProcessor::QuitAction()
	{
		UUIManagerSubSystem::GetInstance()->DisplayEndangeredState(false);

		Super::QuitAction();
	}

}