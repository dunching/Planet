
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

#include "Character/GravityMovementComponent.h"

#include "LogHelper/LogWriter.h"
#include "ThreadPoolHelper/ThreadLibrary.h"
#include "ToolsLibrary.h"
#include "Animation/AnimInstanceBase.h"
#include "GenerateType.h"
#include "PlanetPlayerState.h"
#include "HorseCharacter.h"
#include "ArticleBase.h"
#include "HumanCharacter.h"
#include "PlacingBuildingAreaProcessor.h"
#include "ActionTrackVehiclePlace.h"
#include "PlacingWallProcessor.h"
#include "PlacingGroundProcessor.h"
#include "Pawn/Vehicle/4WheeledVehicle/4WheeledVehicle.h"
#include <Pawn/EquipmentElementComponent.h>
#include "ToolsMenu.h"
#include "BackpackMenu.h"
#include "UIManagerSubSystem.h"
#include <AnimInstanceHorse.h>
#include "DestroyProgress.h"
#include "InputProcessorSubSystem.h"
#include "HorseViewBackpackProcessor.h"
#include "HorseProcessor.h"
#include "HumanProcessor.h"
#include "HumanAnimInstance.h"
#include "CollisionDataStruct.h"

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
		
		if (RiderPtr)
		{
			RiderPtr->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);

			auto AnimInstPtr = RiderPtr->GetAnimationIns<UHumanAnimInstance>();
			if (AnimInstPtr)
			{
				AnimInstPtr->RidingState_Anim = ERidingState_Anim::kRequestMount;
			}

			auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
			RiderPtr->AttachToActor(OnwerActorPtr, FAttachmentTransformRules::SnapToTargetIncludingScale);
		}
	}

	void FHorseRegularProcessor::QuitAction()
	{
		Super::QuitAction();
	}

	void FHorseRegularProcessor::BeginDestroy()
	{
		Super::BeginDestroy();
	}

	void FHorseRegularProcessor::MoveForward(const FInputActionValue& InputActionValue)
    {
        const auto Value = InputActionValue.Get<float>();
        auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();

        if (OnwerActorPtr->Controller != nullptr)
        {
            const FRotator Rotation = OnwerActorPtr->Controller->GetControlRotation();

            const FVector ForwardDirection = UKismetMathLibrary::MakeRotFromZX(-OnwerActorPtr->GetGravityDirection(), Rotation.Quaternion().GetForwardVector()).Vector();

            DrawDebugLine(GetWorldImp(), OnwerActorPtr->GetActorLocation(), OnwerActorPtr->GetActorLocation() + (100 * ForwardDirection), FColor::Red, false, 3);
            OnwerActorPtr->AddMovementInput(ForwardDirection, Value);
        }
	}

	void FHorseRegularProcessor::MoveRight(const FInputActionValue& InputActionValue)
    {
        const auto Value = InputActionValue.Get<float>();
        auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();

        if (OnwerActorPtr->Controller != nullptr)
        {
            const FRotator Rotation = OnwerActorPtr->Controller->GetControlRotation();

            const FVector RightDirection = Rotation.Quaternion().GetRightVector();

            OnwerActorPtr->AddMovementInput(RightDirection, Value);
        }
	}

	void FHorseRegularProcessor::MoveUp(const FInputActionValue& InputActionValue)
	{
	}

	void FHorseRegularProcessor::MouseWheel(const FInputActionValue& InputActionValue)
	{
	}

	void FHorseRegularProcessor::AddPitchInput(const FInputActionValue& InputActionValue)
    {
        const auto Value = InputActionValue.Get<float>();
        auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();

        OnwerActorPtr->AddControllerPitchInput(Value);
	}

	void FHorseRegularProcessor::AddYawInput(const FInputActionValue& InputActionValue)
    {
        const auto Value = InputActionValue.Get<float>();
        auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
        OnwerActorPtr->AddControllerYawInput(Value);
	}

	void FHorseRegularProcessor::WKeyPressed()
	{
	}

	void FHorseRegularProcessor::WKeyReleased()
	{
	}

	void FHorseRegularProcessor::AKeyPressed()
	{
	}

	void FHorseRegularProcessor::AKeyReleased()
	{
	}

	void FHorseRegularProcessor::SKeyPressed()
	{
	}

	void FHorseRegularProcessor::SKeyReleased()
	{
	}

	void FHorseRegularProcessor::DKeyPressed()
	{
	}

	void FHorseRegularProcessor::DKeyReleased()
	{
	}

	void FHorseRegularProcessor::EKeyPressed()
	{
		bIsPressdE = true;

		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
		auto AnimInstPtr = OnwerActorPtr->GetAnimationIns<UHumanAnimInstance>();
		if (AnimInstPtr)
		{
			AnimInstPtr->RidingState_Anim = ERidingState_Anim::kRequestDismount;
		}
	}

	void FHorseRegularProcessor::EKeyReleased()
	{
		bIsPressdE = false;
	}

	void FHorseRegularProcessor::RKeyPressed()
	{
	}

	void FHorseRegularProcessor::TKeyPressed()
	{
	}

	void FHorseRegularProcessor::YKeyPressed()
	{
	}

	void FHorseRegularProcessor::BKeyPressed()
	{
		UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<HorseProcessor::FHorseViewBackpackProcessor>();
	}

	void FHorseRegularProcessor::FKeyPressed()
	{
	}

	void FHorseRegularProcessor::GKeyPressed()
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

			DrawDebugSphere(HumanCharaterPtr->GetWorld(), Result.ImpactPoint, 20, 10, FColor::Red, true);
		}
	}

	void FHorseRegularProcessor::MouseLeftReleased()
	{
	}

	void FHorseRegularProcessor::MouseLeftPressed()
	{
	}

	void FHorseRegularProcessor::MouseRightReleased()
	{
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
	}

	void FHorseRegularProcessor::MouseRightPressed()
	{
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
	}

	void FHorseRegularProcessor::PressedNumKey(int32 NumKey)
	{
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();

		auto UIManagerPtr = UUIManagerSubSystem::GetInstance();
		if (UIManagerPtr)
		{
		}
	}

	void FHorseRegularProcessor::TickImp(float Delta)
	{
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
		auto AnimInstPtr = OnwerActorPtr->GetAnimationIns<UHumanAnimInstance>();
		if (AnimInstPtr)
		{
			if (AnimInstPtr->RidingState_Anim == ERidingState_Anim::kMounted)
			{
				auto PlayerCharacterPtr = GetOwnerActor<FOwnerPawnType>();
				if (!PlayerCharacterPtr)
				{
					return;
				}
				
				auto PlayerController = Cast<APlayerController>(PlayerCharacterPtr->GetController());
				if (!PlayerController)
				{
					return;
				}

				if (RiderPtr)
				{
					RiderPtr->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);
					PlayerController->Possess(RiderPtr);
				}
			}
		}
	}

	void FHorseRegularProcessor::ESCKeyPressed()
	{
	}

	void FHorseRegularProcessor::LCtrlKeyPressed()
	{
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
	}

	void FHorseRegularProcessor::LCtrlKeyReleased()
	{
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
	}

	void FHorseRegularProcessor::LShiftKeyPressed()
	{
		//	GetOwnerActor<FOwnerPawnType>()->GetGravityMovementComponent()->SetMoveMaxSpeed(RunSpeed);
	}

	void FHorseRegularProcessor::LShiftKeyReleased()
	{
		//	GetOwnerActor<FOwnerPawnType>()->GetGravityMovementComponent()->SetMoveMaxSpeed(WalkSpeed);
	}

	void FHorseRegularProcessor::SpaceKeyPressed()
	{
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();

		if (OnwerActorPtr->CanJump())
		{
			OnwerActorPtr->Jump();
		}
	}

	void FHorseRegularProcessor::SpaceKeyReleased()
	{
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
	}
}