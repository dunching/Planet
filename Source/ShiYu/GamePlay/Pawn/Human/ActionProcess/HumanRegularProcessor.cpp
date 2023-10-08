
#include "HumanRegularProcessor.h"

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
#include "HumanCharacter.h"
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
#include "AnimInstanceHuman.h"
#include <Character/GravityMovementComponent.h>
#include <UI/Item/Hover/DestroyProgress/DestroyProgress.h>
#include "InputProcessorSubSystem.h"
#include "HumanViewBackpackProcessor.h"
#include "HorseProcessor.h"
#include "HumanProcessor.h"
#include "GravitySpringComponent.h"

namespace HumanProcessor
{
	FHumanRegularProcessor::FHumanRegularProcessor(FOwnerPawnType* CharacterPtr) :
		Super(CharacterPtr)
	{
	}

	FHumanRegularProcessor::~FHumanRegularProcessor()
	{
	}

	void FHumanRegularProcessor::EnterAction()
	{
		FInputProcessor::EnterAction();

		Async(EAsyncExecution::ThreadPool, std::bind(&FHumanRegularProcessor::UpdateLookAtObject, this));
	}

	void FHumanRegularProcessor::QuitAction()
	{
		FInputProcessor::QuitAction();
	}

	void FHumanRegularProcessor::BeginDestroy()
	{
		FInputProcessor::BeginDestroy();
	}

	void FHumanRegularProcessor::MoveForward(const FInputActionValue& InputActionValue)
	{
		const auto Value = InputActionValue.Get<float>();
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
		OnwerActorPtr->AddMovementInput(OnwerActorPtr->GetActorForwardVector(), Value);
	}

	void FHumanRegularProcessor::MoveRight(const FInputActionValue& InputActionValue)
	{
		const auto Value = InputActionValue.Get<float>();
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
		OnwerActorPtr->AddMovementInput(OnwerActorPtr->GetActorRightVector(), Value);
	}

	void FHumanRegularProcessor::MoveUp(const FInputActionValue& InputActionValue)
	{
	}

	void FHumanRegularProcessor::MouseWheel(const FInputActionValue& InputActionValue)
	{
	}

	void FHumanRegularProcessor::AddPitchInput(const FInputActionValue& InputActionValue)
	{
		const auto Value = InputActionValue.Get<float>();
		if (FMath::IsNearlyZero(Value))
		{
			return;
		}

		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
		auto Rot = OnwerActorPtr->GetSpringArmComponent()->GetRelativeRotation();

        const auto Pitch = FMath::Clamp(Rot.Pitch + Value, -OnwerActorPtr->MaxLookDown, OnwerActorPtr->MaxLookUp);
		OnwerActorPtr->GetAnimationIns<UAnimInstanceHuman>()->SetPitch(Pitch);

		OnwerActorPtr->GetSpringArmComponent()->SetRelativeRotation(FRotator(Pitch, 0, 0));
	}

	void FHumanRegularProcessor::AddYawInput(const FInputActionValue& InputActionValue)
	{
		const auto Value = InputActionValue.Get<float>();
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
		OnwerActorPtr->AddActorLocalRotation(FRotator(0, Value, 0));
	}

	void FHumanRegularProcessor::WKeyPressed()
	{
	}

	void FHumanRegularProcessor::WKeyReleased()
	{
	}

	void FHumanRegularProcessor::AKeyPressed()
	{
	}

	void FHumanRegularProcessor::AKeyReleased()
	{
	}

	void FHumanRegularProcessor::SKeyPressed()
	{
	}

	void FHumanRegularProcessor::SKeyReleased()
	{
	}

	void FHumanRegularProcessor::DKeyPressed()
	{
	}

	void FHumanRegularProcessor::DKeyReleased()
	{
	}

	void FHumanRegularProcessor::EKeyPressed()
	{
		bIsPressdE = true;
	}

	void FHumanRegularProcessor::EKeyReleased()
	{
		bIsPressdE = false;
	}

	void FHumanRegularProcessor::RKeyPressed()
	{
	}

	void FHumanRegularProcessor::TKeyPressed()
	{
	}

	void FHumanRegularProcessor::YKeyPressed()
	{
	}

	void FHumanRegularProcessor::BKeyPressed()
	{
		UInputProcessorSubSystem::GetInstance()->SwitchActionProcess<FViewBackpackProcessor>();
	}

	void FHumanRegularProcessor::FKeyPressed()
	{
	}

	void FHumanRegularProcessor::GKeyPressed()
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

	void FHumanRegularProcessor::MouseLeftReleased()
	{
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();

		OnwerActorPtr->GetEquipmentSocksComponent()->DoAction(EEquipmentActionType::kStopAction1);
	}

	void FHumanRegularProcessor::MouseLeftPressed()
	{
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();

		OnwerActorPtr->GetEquipmentSocksComponent()->DoAction(EEquipmentActionType::kStartAction1);
	}

	void FHumanRegularProcessor::MouseRightReleased()
	{
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
	}

	void FHumanRegularProcessor::MouseRightPressed()
	{
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
	}

	void FHumanRegularProcessor::PressedNumKey(int32 NumKey)
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

	void FHumanRegularProcessor::UpdateLookAtObject()
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

	void FHumanRegularProcessor::ESCKeyPressed()
	{
	}

	void FHumanRegularProcessor::LCtrlKeyPressed()
	{
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
	}

	void FHumanRegularProcessor::LCtrlKeyReleased()
	{
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
	}

	void FHumanRegularProcessor::LShiftKeyPressed()
	{
		//	GetOwnerActor<FOwnerPawnType>()->GetGravityMovementComponent()->SetMoveMaxSpeed(RunSpeed);
	}

	void FHumanRegularProcessor::LShiftKeyReleased()
	{
		//	GetOwnerActor<FOwnerPawnType>()->GetGravityMovementComponent()->SetMoveMaxSpeed(WalkSpeed);
	}

	void FHumanRegularProcessor::SpaceKeyPressed()
	{
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();

		if (OnwerActorPtr->CanJump())
		{
			OnwerActorPtr->Jump();
		}
	}

	void FHumanRegularProcessor::SpaceKeyReleased()
	{
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
	}
}