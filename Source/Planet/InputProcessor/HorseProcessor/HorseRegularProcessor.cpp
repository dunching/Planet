
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
#include "GameplayTags/GameplayTagsSubSystem.h"
#include "AbilitySystemBlueprintLibrary.h"

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
#include "UnitProxyProcessComponent.h"
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

	void FHorseRegularProcessor::MoveForward(const FInputActionValue& InputActionValue)
    {
        const auto Value = InputActionValue.Get<float>();
        auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();

        if (OnwerActorPtr->Controller != nullptr)
        {
            const FRotator Rotation = OnwerActorPtr->Controller->GetControlRotation();

            const FVector ForwardDirection = 
				UKismetMathLibrary::MakeRotFromZX(-OnwerActorPtr->GetGravityDirection(), Rotation.Quaternion().GetForwardVector()).Vector();

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
	}

	void FHorseRegularProcessor::EKeyReleased()
	{
		bIsPressdE = false;

		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();

		if (OnwerActorPtr && OnwerActorPtr->RiderPtr)
		{
			FGameplayEventData Payload;
			auto GameplayAbilityTargetData_DashPtr = new FGameplayAbilityTargetData_DisMount;

			GameplayAbilityTargetData_DashPtr->HorseCharacterPtr = OnwerActorPtr;

			Payload.TargetData.Add(GameplayAbilityTargetData_DashPtr);

			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OnwerActorPtr->RiderPtr, UGameplayTagsSubSystem::GetInstance()->DisMount, Payload);
		}
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
		ObjectQueryParams.AddObjectTypesToQuery(VoxelWorld_Object);

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

#ifdef WITH_EDITOR
			if (HorseRegularProcessor.GetValueOnGameThread())
			{
				DrawDebugSphere(HumanCharaterPtr->GetWorld(), Result.ImpactPoint, 20, 10, FColor::Red, true);
			}
#endif

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
	}

	void FHorseRegularProcessor::LShiftKeyReleased()
	{
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