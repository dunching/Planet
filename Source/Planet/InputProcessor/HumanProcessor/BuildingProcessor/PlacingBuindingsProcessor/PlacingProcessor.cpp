
#include "PlacingProcessor.h"

#include "GameFramework/SpringArmComponent.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Async/Async.h"
#include "EnhancedInputSubsystems.h"

#include "Character/GravityMovementComponent.h"
//#include "VoxelSceneActor.h"

#include "HumanCharacter_Player.h"
#include "Animation/AnimInstanceBase.h"

#include "LogHelper/LogWriter.h"
#include "UIManagerSubSystem.h"
#include "ToolsMenu.h"
#include "InputProcessorSubSystem.h"
#include "HumanRegularProcessor.h"
#include "HorseProcessor.h"
#include "HumanProcessor.h"
#include "BuildSharedData.h"
#include "BuildingBase.h"
#include "CollisionDataStruct.h"
#include "ThreadLibrary.h"
#include "HumanCharacter_Player.h"

namespace HumanProcessor
{
	FPlacingProcessor::FPlacingProcessor(FOwnerPawnType* CharacterPtr) :
		Super(CharacterPtr)
	{
	}

	void FPlacingProcessor::EnterAction()
	{
		Super::EnterAction();

		UUIManagerSubSystem::GetInstance()->DisplayBuildingStateHUD(true);

		if (!BuildTargetPtr)
		{
			return;
		}
		BuildTargetPtr->GenerateCaptureObj();
		if (BuildTargetPtr && BuildTargetPtr->GetStateController())
		{
			BuildTargetPtr->GetStateController()->SwitchToNewState(EBuildingState::kPreviewPlacing);
		}

		SetBuildSharedDataCollision(true);

		GetWorldImp()->GetTimerManager().SetTimer(TimerHandle, std::bind(&FPlacingProcessor::OnPlaceItemPrev, this), 1.f / 60.f, true);

		Async(EAsyncExecution::ThreadPool, std::bind(&FPlacingProcessor::AsyncFindCaptureLoop, this));
	}

	void FPlacingProcessor::QuitAction()
	{
		HitsAry.Empty();
		HitsAryCV.notify_all();

		SetBuildSharedDataCollision(false);

		auto OnwerActorPtr = GetOwnerActor<AHumanCharacter>();
		if (OnwerActorPtr->GetWorld())
		{
			OnwerActorPtr->GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		}

		PRINTFUNC();
		Super::QuitAction();
	}

	void FPlacingProcessor::PressedNumKey(int32 NumKey)
	{
	}

	void FPlacingProcessor::SetHoldItemsData(const TSharedPtr<FSceneTool>& HoldItemsSPtr_)
	{
		HoldItemsSPtr = HoldItemsSPtr_;
	}

	void FPlacingProcessor::SetPlaceBuildItem(const FSceneTool& Item_)
	{
		//Item = Item_;
	}

	void FPlacingProcessor::GKeyPressed()
	{
		ExplicitCaptureIndex++;
	}

	void FPlacingProcessor::BeginDestroy()
	{
		if (BuildTargetPtr)
		{
			BuildTargetPtr->Destroy();
			BuildTargetPtr = nullptr;
		}

		Super::BeginDestroy();
	}

	void FPlacingProcessor::AddPitchInput(const FInputActionValue& InputActionValue)
	{
		const auto Value = InputActionValue.Get<float>();
		if (FMath::IsNearlyZero(Value))
		{
			return;
		}

		auto OnwerActorPtr = GetOwnerActor<AHumanCharacter>();
	}

	void FPlacingProcessor::AddYawInput(const FInputActionValue& InputActionValue)
	{
		const auto Value = InputActionValue.Get<float>();
		auto OnwerActorPtr = GetOwnerActor<AHumanCharacter>();
		OnwerActorPtr->AddActorLocalRotation(FRotator(0, Value, 0));
	}

	void FPlacingProcessor::MoveForward(const FInputActionValue& InputActionValue)
	{
		const auto Value = InputActionValue.Get<float>();
		auto OnwerActorPtr = GetOwnerActor<AHumanCharacter>();
		OnwerActorPtr->AddMovementInput(OnwerActorPtr->GetActorForwardVector(), Value);
	}

	void FPlacingProcessor::MoveRight(const FInputActionValue& InputActionValue)
	{
		const auto Value = InputActionValue.Get<float>();
		auto OnwerActorPtr = GetOwnerActor<AHumanCharacter>();
		OnwerActorPtr->AddMovementInput(OnwerActorPtr->GetActorRightVector(), Value);
	}

	void FPlacingProcessor::ESCKeyPressed()
	{
		UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<FHumanRegularProcessor>();
	}

	void FPlacingProcessor::QKeyPressed()
	{
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();

		if (OnwerActorPtr)
		{
			auto EnhancedInputLocalPlayerSubsystemPtr = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
				Cast<APlayerController>(GetOwnerActor()->GetController())->GetLocalPlayer()
			);
			if (EnhancedInputLocalPlayerSubsystemPtr->GetPlayerInput()->IsPressed(EKeys::LeftAlt))
			{
				UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<HumanProcessor::FHumanRegularProcessor>();
			}
		}
	}

	void FPlacingProcessor::OnPlaceItemPrev()
	{
		FMinimalViewInfo DesiredView;

		GetOwnerActor<FOwnerPawnType>()->GetCameraComp()->GetCameraView(0, DesiredView);

		auto StartPt = DesiredView.Location;
		auto StopPt = DesiredView.Location + (DesiredView.Rotation.Vector() * 1000);

		auto OnwerActorPtr = GetOwnerActor<AHumanCharacter>();

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(BuildTargetPtr);
		Params.AddIgnoredActor(BuildTargetPtr->BuildingCaptureDataPtr);
		Params.AddIgnoredActor(OnwerActorPtr);

		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(VoxelWorld_Object);
		ObjectQueryParams.AddObjectTypesToQuery(Building_Object);
		ObjectQueryParams.AddObjectTypesToQuery(CapturePoint_Object);

		TArray<FHitResult> OutHitsAry;
		if (GetWorldImp()->LineTraceMultiByObjectType(
			OutHitsAry,
			StartPt,
			StopPt,
			ObjectQueryParams,
			Params
		))
		{
		}
		OutHitsAry.Add(FHitResult(StartPt, StopPt));
		AsyncFindCaptureComponent(OutHitsAry);
	}

	void FPlacingProcessor::AsyncFindCaptureComponent(const TArray<FHitResult>& OutHitsAry)
	{
		{
			std::unique_lock<std::mutex>Lock(CopyAryMutex);
			HitsAry = OutHitsAry;
		}
		HitsAryCV.notify_all();
	}

	void FPlacingProcessor::AsyncFindCaptureLoop()
	{
		for (; !bIsRequestQuit;)
		{
			{
				std::unique_lock<std::mutex>Lock(HitsAryMutex);
				HitsAryCV.wait(Lock);
			}
			std::unique_lock<std::mutex>Lock(CopyAryMutex);
			const auto HitResult = HitsAry;
			Lock.unlock();
			if (HitResult.IsValidIndex(0))
			{
				AttachActor(FindCaptureImp(HitResult));
				continue;
			}
		}
	}

	void FPlacingProcessor::SetBuildSharedDataCollision(bool bIsEnable)
	{
		TArray<AActor*> OutActors;
		UGameplayStatics::GetAllActorsOfClass(GetOwnerActor(), ABuildingBase::StaticClass(), OutActors);
		if (bIsEnable)
		{
			for (auto Iter : OutActors)
			{
				auto CurBuildPtr = Cast<ABuildingBase>(Iter);
				if (CurBuildPtr && CurBuildPtr->BuildingCaptureDataPtr && (BuildTargetPtr != CurBuildPtr))
				{
					//		CurBuildPtr->BuildingCaptureDataPtr->StartOverlapCheck(std::get<EBuildingsType>(Item.ItemType));
				}
			}
		}
		else
		{
			for (auto Iter : OutActors)
			{
				auto CurBuildPtr = Cast<ABuildingBase>(Iter);
				if (CurBuildPtr && CurBuildPtr->BuildingCaptureDataPtr)
				{
					CurBuildPtr->BuildingCaptureDataPtr->StopOverlapCheck();
				}
			}
		}
	}

	FCaptureInfo FPlacingProcessor::FindCaptureImp(const TArray<FHitResult>& CurHitsAru)
	{
		FCaptureInfo Result;

		const auto Num = CurHitsAru.Num();

		if (Num > 0)
		{
			Result.HitResult = CurHitsAru[0];

			for (int32 Index = 0; Index < Num; Index++)
			{
				auto TargetPtr = Cast<ABuildingBase>(CurHitsAru[Index].GetActor());
				if (TargetPtr)
				{
					if (TargetPtr->BuildingCaptureDataPtr)
					{
						//		auto CaptureArySPtr = TargetPtr->BuildingCaptureDataPtr->GetCaptureAry(std::get<EBuildingsType>(Item.ItemType));
							//	if (!CaptureArySPtr)
						{
							return Result;
						}
						// 						const auto CaptureNum = CaptureArySPtr->CapturesInfoAry.Num();
						// 						if (CaptureNum > 0)
						// 						{
						// 							for (int32 NumIndex = 0; NumIndex < CaptureNum; NumIndex++)
						// 							{
						// 								Result.CaptureInfoSPtr = CaptureArySPtr->CapturesInfoAry[ExplicitCaptureIndex % CaptureNum];
						// 								return Result;
						// 							}
						// 							return Result;
						// 						}
					}
					else
					{
						auto ParentPtr = Cast<ABuildingCaptureData>(TargetPtr->GetParentActor());
						if (ParentPtr)
						{
							// 							auto MeshComponentAry = ParentPtr->GetCaptureAry(std::get<EBuildingsType>(Item.ItemType));
							// 							for (auto Iter : MeshComponentAry->CapturesInfoAry)
							// 							{
							// 								if (Iter && Iter->CapturePtPtr == TargetPtr)
							// 								{
							// 									Result.CaptureInfoSPtr = Iter;
							// 								}
							// 							}
						}
					}
				}
			}
		}

		return Result;
	}

	void FPlacingProcessor::AttachActor(const FCaptureInfo& CaptureInfo)
	{
		if (!BuildTargetPtr)
		{
			return;
		}

		ON_SCOPE_EXIT
		{
				ThreadLibrary::SyncExecuteInGameThread([&]()
					{
						if (CaptureInfo.HitResult.bBlockingHit)
						{
							auto VoerlapInfoAry = BuildTargetPtr->BuildingCaptureDataPtr->TerrainCheckComponnetPtr->GetOverlapInfos();

							bool bIsLessMaxAttachedCount = true;

							if (CaptureInfo.CaptureInfoSPtr && CaptureInfo.CaptureInfoSPtr->CapturePtPtr)
							{
								auto BuildingCaptureDataPtr = Cast<ABuildingCaptureData>(CaptureInfo.CaptureInfoSPtr->CapturePtPtr->GetParentActor());

								//								bIsLessMaxAttachedCount = BuildingCaptureDataPtr->AttachedBuldingCount < BuildingCaptureDataPtr->AttachedBuldingMaxCount;
															}
															bCanPlace = (VoerlapInfoAry.Num() == 0) && bIsLessMaxAttachedCount;
														}
														else
														{
															bCanPlace = false;
														}
														BuildTargetPtr->GetStateController()->SwitchToNewState(
															bCanPlace ? EBuildingState::kPreviewPlacing_Sucess : EBuildingState::kPreviewPlacing_Fail
														);
													});
		};

		if (CaptureInfo.CaptureInfoSPtr)
		{
		}
		else
		{
			FTransform Transform = FTransform::Identity;
			FVector Pt = FVector::ZeroVector;
			if (CaptureInfo.HitResult.bBlockingHit)
			{
				Pt = CaptureInfo.HitResult.ImpactPoint;
			}
			else
			{
				Pt = FMath::Lerp(CaptureInfo.HitResult.TraceStart, CaptureInfo.HitResult.TraceEnd, 1.0f);
			}
			Transform.SetTranslation(Pt);

			auto CharacterPtr = GetOwnerActor<ACharacterBase>();

			const auto GravityOrgin = CharacterPtr->GetGravityDirection();
			auto CharacterForward = CharacterPtr->GetActorForwardVector();

			const auto Rot = FRotationMatrix::MakeFromZX(GravityOrgin, CharacterForward).ToQuat();

			Transform.SetRotation(Rot);

			// 附着到地面
			ThreadLibrary::SyncExecuteInGameThread([&]()
				{
					BuildTargetPtr->SetActorTransform(Transform);
				});
		}
	}

	void FPlacingProcessor::MouseLeftPressed()
	{
		if (bCanPlace)
		{
			BuildTargetPtr->GetStateController()->SwitchToNewState(EBuildingState::kPlaced);
		}
		else
		{
			BuildTargetPtr->Destroy();
		}
		BuildTargetPtr = nullptr;

		UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<FHumanRegularProcessor>();
	}
}