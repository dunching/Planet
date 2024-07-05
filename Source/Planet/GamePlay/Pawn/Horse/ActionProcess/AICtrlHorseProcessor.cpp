
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
#include "GenerateType.h"
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
#include "Pawn/Vehicle/4WheeledVehicle/4WheeledVehicle.h"
#include "InteractiveSkillComponent.h"
#include "ToolsMenu.h"
#include "BackpackMenu.h"
#include "UIManagerSubSystem.h"
#include "AnimInstanceHorse.h"
#include "DestroyProgress.h"
#include "InputProcessorSubSystem.h"
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

		Async(EAsyncExecution::Thread, std::bind(&FAICtrlHorseProcessor::UpdateLookAtObject, this));
	}

	void FAICtrlHorseProcessor::QuitAction()
	{
		FInputProcessor::QuitAction();
	}

	void FAICtrlHorseProcessor::BeginDestroy()
	{
		FInputProcessor::BeginDestroy();
	}

	void FAICtrlHorseProcessor::MoveForward(const FInputActionValue& InputActionValue)
	{
		const auto Value = InputActionValue.Get<float>();
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
		OnwerActorPtr->AddMovementInput(OnwerActorPtr->GetActorForwardVector(), Value);
	}

	void FAICtrlHorseProcessor::MoveRight(const FInputActionValue& InputActionValue)
	{
		const auto Value = InputActionValue.Get<float>();
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
		OnwerActorPtr->AddMovementInput(OnwerActorPtr->GetActorRightVector(), Value);
	}

	void FAICtrlHorseProcessor::MoveUp(const FInputActionValue& InputActionValue)
	{
	}

	void FAICtrlHorseProcessor::MouseWheel(const FInputActionValue& InputActionValue)
	{
	}

	void FAICtrlHorseProcessor::AddPitchInput(const FInputActionValue& InputActionValue)
	{
	}

	void FAICtrlHorseProcessor::AddYawInput(const FInputActionValue& InputActionValue)
	{
		const auto Value = InputActionValue.Get<float>();
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
		OnwerActorPtr->AddActorLocalRotation(FRotator(0, Value, 0));
	}

	void FAICtrlHorseProcessor::WKeyPressed()
	{
	}

	void FAICtrlHorseProcessor::WKeyReleased()
	{
	}

	void FAICtrlHorseProcessor::AKeyPressed()
	{
	}

	void FAICtrlHorseProcessor::AKeyReleased()
	{
	}

	void FAICtrlHorseProcessor::SKeyPressed()
	{
	}

	void FAICtrlHorseProcessor::SKeyReleased()
	{
	}

	void FAICtrlHorseProcessor::DKeyPressed()
	{
	}

	void FAICtrlHorseProcessor::DKeyReleased()
	{
	}

	void FAICtrlHorseProcessor::EKeyPressed()
	{
		bIsPressdE = true;
	}

	void FAICtrlHorseProcessor::EKeyReleased()
	{
		bIsPressdE = false;
	}

	void FAICtrlHorseProcessor::RKeyPressed()
	{
	}

	void FAICtrlHorseProcessor::TKeyPressed()
	{
	}

	void FAICtrlHorseProcessor::YKeyPressed()
	{
	}

	void FAICtrlHorseProcessor::BKeyPressed()
	{
		UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<FHorseViewBackpackProcessor>();
	}

	void FAICtrlHorseProcessor::FKeyPressed()
	{
	}

	void FAICtrlHorseProcessor::GKeyPressed()
	{
		auto PlayerCharacterPtr = GetOwnerActor<FOwnerPawnType>();
		if (!PlayerCharacterPtr)
		{
			return;
		}

		FMinimalViewInfo DesiredView;
		PlayerCharacterPtr->GetCameraComp()->GetCameraView(0, DesiredView);

		auto StartPt = DesiredView.Location;
		auto StopPt = DesiredView.Location + (DesiredView.Rotation.Vector() * 1000);

		FHitResult Result;

		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(VoxelWorld);

		FCollisionQueryParams Params;
		Params.bTraceComplex = false;

		auto HumanCharaterPtr = GetOwnerActor<FOwnerPawnType>();
		if (HumanCharaterPtr->GetWorld()->LineTraceSingleByObjectType(
			Result,
			StartPt,
			StopPt,
			ObjectQueryParams,
			Params)
			)
		{

		}
	}

	void FAICtrlHorseProcessor::MouseLeftReleased()
	{
	}

	void FAICtrlHorseProcessor::MouseLeftPressed()
	{
	}

	void FAICtrlHorseProcessor::MouseRightReleased()
	{
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
	}

	void FAICtrlHorseProcessor::MouseRightPressed()
	{
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
	}

	void FAICtrlHorseProcessor::PressedNumKey(int32 NumKey)
	{
	}

	void FAICtrlHorseProcessor::UpdateLookAtObject()
	{
	}

	void FAICtrlHorseProcessor::ESCKeyPressed()
	{
	}

	void FAICtrlHorseProcessor::LCtrlKeyPressed()
	{
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
	}

	void FAICtrlHorseProcessor::LCtrlKeyReleased()
	{
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
	}

	void FAICtrlHorseProcessor::LShiftKeyPressed()
	{
		//	GetOwnerActor<FOwnerPawnType>()->GetGravityMovementComponent()->SetMoveMaxSpeed(RunSpeed);
	}

	void FAICtrlHorseProcessor::LShiftKeyReleased()
	{
		//	GetOwnerActor<FOwnerPawnType>()->GetGravityMovementComponent()->SetMoveMaxSpeed(WalkSpeed);
	}

	void FAICtrlHorseProcessor::SpaceKeyPressed()
	{
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();

		if (OnwerActorPtr->CanJump())
		{
			OnwerActorPtr->Jump();
		}
	}

	void FAICtrlHorseProcessor::SpaceKeyReleased()
	{
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
	}
}