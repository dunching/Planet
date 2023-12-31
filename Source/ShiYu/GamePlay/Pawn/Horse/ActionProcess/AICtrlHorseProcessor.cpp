
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

#include "LogHelper/LogWriter.h"
#include "ThreadPoolHelper/ThreadLibrary.h"
#include "ToolsLibrary.h"
#include "Animation/AnimInstanceBase.h"
#include "GenerateType.h"
#include "PlayerState/ShiYuPlayerState.h"
#include "CharacterBase.h"
#include "SceneObj/Track/TrackVehicleBase.h"
#include "ArticleBase.h"
#include <SceneObj/Building/BuildingBase.h>
#include "SceneObj/CollisionDataStruct.h"
#include "HorseCharacter.h"
#include "PlacingBuildingAreaProcessor.h"
#include "ActionTrackPlace.h"
#include "ActionTrackVehiclePlace.h"
#include "PlacingWallProcessor.h"
#include "PlacingGroundProcessor.h"
#include "Pawn/Vehicle/4WheeledVehicle/4WheeledVehicle.h"
#include <Pawn/EquipmentSocksComponent.h>
#include "UI/Menu/EquipItems/EquipMenu.h"
#include <UI/Menu/Backpack/BackpackMenu.h>
#include "UI/UIManagerSubSystem.h"
#include "AnimInstanceHorse.h"
#include <Character/GravityMovementComponent.h>
#include <UI/Item/Hover/DestroyProgress/DestroyProgress.h>
#include "InputProcessorSubSystem.h"
#include "HorseViewBackpackProcessor.h"
#include "HorseProcessor.h"
#include "HumanProcessor.h"
#include "GravitySpringComponent.h"

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
		const auto Value = InputActionValue.Get<float>();
		if (FMath::IsNearlyZero(Value))
		{
			return;
		}

		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
		auto Rot = OnwerActorPtr->GetSpringArmComponent()->GetRelativeRotation();

        const auto Pitch = FMath::Clamp(Rot.Pitch + Value, -OnwerActorPtr->MaxLookDown, OnwerActorPtr->MaxLookUp);
		OnwerActorPtr->GetAnimationIns<UAnimInstanceHorse>()->SetPitch(Pitch);

		OnwerActorPtr->GetSpringArmComponent()->SetRelativeRotation(FRotator(Pitch, 0, 0));
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
		UInputProcessorSubSystem::GetInstance()->SwitchActionProcess<FHorseViewBackpackProcessor>();
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

			DrawDebugSphere(HumanCharaterPtr->GetWorld(), Result.ImpactPoint, 20, 10, FColor::Red, true);
		}
	}

	void FAICtrlHorseProcessor::MouseLeftReleased()
	{
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();

		OnwerActorPtr->GetEquipmentSocksComponent()->DoAction(EEquipmentActionType::kStopAction1);
	}

	void FAICtrlHorseProcessor::MouseLeftPressed()
	{
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();

		OnwerActorPtr->GetEquipmentSocksComponent()->DoAction(EEquipmentActionType::kStartAction1);
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
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();

		auto UIManagerPtr = UUIManagerSubSystem::GetInstance();
		if (UIManagerPtr)
		{
			auto EquipUIPtr = UIManagerPtr->DisplayEquipMenuUI();
			if (!EquipUIPtr)
			{
				return;
			}
			auto ItemPropertyBase = EquipUIPtr->GetItem(NumKey);
			if (ItemPropertyBase.Num > 0)
			{
				OnwerActorPtr->GetEquipmentSocksComponent()->UseItem(ItemPropertyBase);
			}
		}
	}

	void FAICtrlHorseProcessor::UpdateLookAtObject()
	{
		for (; !bNeedQuit;)
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
			ObjectQueryParams.AddObjectTypesToQuery(Building);

			FCollisionQueryParams Params;
			Params.bTraceComplex = false;

			bool bIsDestroyingBuilding = false;
			auto HumanCharaterPtr = GetOwnerActor<FOwnerPawnType>();

			ThreadLibrary::SyncExecuteInGameThread([&]()
				{
					if (HumanCharaterPtr->GetWorld()->LineTraceSingleByObjectType(
						Result,
						StartPt,
						StopPt,
						ObjectQueryParams,
						Params)
						)
					{
						if (bIsPressdE)
						{
							auto TempBuildingPtr = Cast<ABuildingBase>(Result.GetActor());
							if (BuildingPtr)
							{
								if (BuildingPtr == TempBuildingPtr)
								{
									DestroyBuildingProgress += 10;

									if (DestroyBuildingProgress < 100)
									{
										auto UIManagerPtr = UUIManagerSubSystem::GetInstance();
										if (UIManagerPtr)
										{
											auto DestroyProgressPtr = UIManagerPtr->DisplayDestroyProgress();

											if (DestroyProgressPtr)
											{
												DestroyProgressPtr->SetLookAtObject(BuildingPtr);
												DestroyProgressPtr->SetProgress(DestroyBuildingProgress);
												bIsDestroyingBuilding = true;
											}
										}
									}
									else
									{
										BuildingPtr->Destroy();
									}
								}
							}
							else
							{
								BuildingPtr = TempBuildingPtr;
								bIsDestroyingBuilding = true;
							}
						}
					}
				});
			{
				ON_SCOPE_EXIT
				{
					if (!bIsDestroyingBuilding)
					{
						ThreadLibrary::SyncExecuteInGameThread([&]()
							{
								auto UIManagerPtr = UUIManagerSubSystem::GetInstance();
								if (UIManagerPtr)
								{
									UIManagerPtr->RemoveDestroyProgress();
								}
							});
						BuildingPtr = nullptr;
						DestroyBuildingProgress = 0;
					}
				};
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}

		MarkHaveDone();
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