
#include "HorseRegularProcessor.h"

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
#include "AbilitySystemBlueprintLibrary.h"

#include "Character/GravityMovementComponent.h"

#include "LogHelper/LogWriter.h"
#include "ThreadPoolHelper/ThreadLibrary.h"
#include "ToolsLibrary.h"
#include "Animation/AnimInstanceBase.h"
#include "GenerateTypes.h"
#include "PlanetPlayerState.h"
#include "HorseCharacter.h"
#include "ArticleBase.h"
#include "HumanCharacter.h"
#include "PlacingBuildingAreaProcessor.h"
#include "ActionTrackVehiclePlace.h"
#include "PlacingWallProcessor.h"
#include "PlacingGroundProcessor.h"
#include "ProxyProcessComponent.h"
#include "ToolsMenu.h"
#include "BackpackMenu.h"
#include "UIManagerSubSystem.h"
#include <AnimInstanceHorse.h>
#include "DestroyProgress.h"
#include "InputProcessorSubSystemBase.h"
#include "HorseViewBackpackProcessor.h"
#include "HorseProcessor.h"
#include "HumanProcessor.h"
#include "HumanAnimInstance.h"
#include "CollisionDataStruct.h"
#include "BasicFutures_DisMount.h"

static TAutoConsoleVariable<int32> HorseRegularProcessor(
	TEXT("Skill.DrawDebug.HorseRegularProcessor"),
	0,
	TEXT("")
	TEXT(" default: 0"));

namespace HorseProcessor
{
	FHorseRegularProcessor::FHorseRegularProcessor(AHorseCharacter* CharacterPtr) :
		Super(CharacterPtr)
	{
	}

	FHorseRegularProcessor::~FHorseRegularProcessor()
	{
	}

	void FHorseRegularProcessor::EnterAction()
	{
		Super::EnterAction();
	}

	void FHorseRegularProcessor::QuitAction()
	{
		Super::QuitAction();
	}

	void FHorseRegularProcessor::BeginDestroy()
	{
		Super::BeginDestroy();
	}

}