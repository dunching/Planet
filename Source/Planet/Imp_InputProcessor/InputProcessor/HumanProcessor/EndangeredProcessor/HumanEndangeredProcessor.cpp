
#include "HumanEndangeredProcessor.h"

#include "DrawDebugHelpers.h"
#include "Async/Async.h"
#include "Engine/LocalPlayer.h"

#include "ToolsLibrary.h"
#include "PlacingBuildingAreaProcessor.h"
#include "ActionTrackVehiclePlace.h"
#include "UIManagerSubSystem.h"
#include <DestroyProgress.h>
#include "HorseProcessor.h"

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

		SwitchShowCursor(true);
			
		UUIManagerSubSystem::GetInstance()->SwitchLayout(ELayoutCommon::kEndangeredLayout);
	}

	void FHumanEndangeredProcessor::QuitAction()
	{
		// UUIManagerSubSystem::GetInstance()->DisplayEndangeredLayout(false);

		Super::QuitAction();
	}

}