
#include "AICtrlHorseProcessor.h"

#include "DrawDebugHelpers.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Camera/CameraComponent.h"
#include "AIController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include <Kismet/GameplayStatics.h>
#include <Subsystems/SubsystemBlueprintLibrary.h>
#include "Async/Async.h"

#include "Character/GravityMovementComponent.h"

#include "LogHelper/LogWriter.h"
#include "ThreadPoolHelper/ThreadLibrary.h"
#include "ToolsLibrary.h"
#include "Animation/AnimInstanceBase.h"
#include "GenerateTypes.h"
#include "PlanetPlayerState.h"
#include "CharacterBase.h"
#include "ArticleBase.h"
#include <BuildingBase.h>
#include "CollisionDataStruct.h"
#include "HorseCharacter.h"
#include "PlacingBuildingAreaProcessor.h"
#include "ActionTrackVehiclePlace.h"
#include "PlacingWallProcessor.h"
#include "PlacingGroundProcessor.h"
#include "ProxyProcessComponent.h"
#include "ToolsMenu.h"
#include "BackpackMenu.h"
#include "UIManagerSubSystem.h"
#include "AnimInstanceHorse.h"
#include "DestroyProgress.h"
#include "InputProcessorSubSystemBase.h"
#include "HorseViewBackpackProcessor.h"
#include "HorseProcessor.h"
#include "HumanProcessor.h"

namespace HorseProcessor
{
	FAICtrlHorseProcessor::FAICtrlHorseProcessor(AHorseCharacter* CharacterPtr) :
		Super(CharacterPtr)
	{
	}

	FAICtrlHorseProcessor::~FAICtrlHorseProcessor()
	{
	}

	void FAICtrlHorseProcessor::EnterAction()
	{
		FInputProcessor::EnterAction();
	}

	void FAICtrlHorseProcessor::QuitAction()
	{
		FInputProcessor::QuitAction();
	}

	void FAICtrlHorseProcessor::BeginDestroy()
	{
		FInputProcessor::BeginDestroy();
	}

}