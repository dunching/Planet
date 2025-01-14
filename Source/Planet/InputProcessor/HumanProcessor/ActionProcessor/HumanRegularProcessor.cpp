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
#include "ViewGroupsProcessor.h"
#include "TeamMatesHelperComponent.h"

#include "GameplayTagsLibrary.h"
#include "BasicFutures_Mount.h"
#include "BasicFutures_Dash.h"
#include "HumanViewRaffleMenu.h"
#include "CharacterAbilitySystemComponent.h"
#include "HumanCharacter_Player.h"
#include "ResourceBox.h"
#include "GroupSharedInfo.h"
#include "HumanCharacter_AI.h"
#include "ItemProxy_Character.h"

static TAutoConsoleVariable<int32> HumanRegularProcessor(
	TEXT("Skill.DrawDebug.HumanRegularProcessor"),
	0,
	TEXT("")
	TEXT(" default: 0"));

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
		Super::EnterAction();

		Async(
			EAsyncExecution::ThreadPool,
			std::bind(&ThisClass::UpdateLookAtObject, this)
		);

		SwitchCurrentWeapon();

		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
		if (OnwerActorPtr)
		{
			UUIManagerSubSystem::GetInstance()->DisplayActionLayout(true, OnwerActorPtr);

			OnAllocationChangedHandle = OnwerActorPtr->GetProxyProcessComponent()->OnCurrentWeaponChanged.AddCallback(
				[this]()
				{
					AddOrRemoveUseMenuItemEvent(true);
				});

			AddOrRemoveUseMenuItemEvent(true);
		}
	}

	void FHumanRegularProcessor::TickImp(float Delta)
	{
		Super::TickImp(Delta);

		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
		if (!OnwerActorPtr)
		{
			return;
		}

		OnwerActorPtr->UpdateSightActor();
	}

	void FHumanRegularProcessor::SwitchCurrentWeapon()
	{
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
		if (OnwerActorPtr)
		{
			OnwerActorPtr->GetProxyProcessComponent()->ActiveWeapon();
		}
	}

	void FHumanRegularProcessor::QuitAction()
	{
		AddOrRemoveUseMenuItemEvent(false);

		UUIManagerSubSystem::GetInstance()->DisplayActionLayout(false);

		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
		if (OnwerActorPtr)
		{
			OnwerActorPtr->GetProxyProcessComponent()->RetractputWeapon();
		}

		OnAllocationChangedHandle->UnBindCallback();

		Super::QuitAction();
	}

	void FHumanRegularProcessor::InputKey(const FInputKeyParams& Params)
	{
		if (Params.Event == EInputEvent::IE_Pressed)
		{
			auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();

			// 特殊处理一下
			if (OnwerActorPtr->LookAtSceneActorPtr && (Params.Key == EKeys::E))
			{
				return;
			}

			auto SkillIter = HandleKeysMap.Find(Params.Key);
			if (SkillIter)
			{
				OnwerActorPtr->GetCharacterAbilitySystemComponent()->BreakMoveToAttackDistance();

				OnwerActorPtr->GetProxyProcessComponent()->ActiveAction(SkillIter->Socket);
			}

			// 这里应该是特定的输入会打断 还是任意输入都会打断？
			else if (
				(Params.Key == EKeys::W) ||
				(Params.Key == EKeys::A) ||
				(Params.Key == EKeys::S) ||
				(Params.Key == EKeys::D)
			)
			{
				OnwerActorPtr->GetCharacterAbilitySystemComponent()->BreakMoveToAttackDistance();
			}
		}
		else
		{
			auto SkillIter = HandleKeysMap.Find(Params.Key);
			if (SkillIter)
			{
				auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
				if (
					SkillIter->Socket.MatchesTag(UGameplayTagsLibrary::WeaponSocket)
				)
				{
					OnwerActorPtr->GetProxyProcessComponent()->CancelAction(SkillIter->Socket);
				}
			}
		}
	}

	void FHumanRegularProcessor::F1KeyPressed()
	{
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
		if (OnwerActorPtr)
		{
			OnwerActorPtr->GetGroupSharedInfo()->GetTeamMatesHelperComponent()->SwitchTeammateOption(
				ETeammateOption::kFollow);
		}
	}

	void FHumanRegularProcessor::F2KeyPressed()
	{
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
		if (OnwerActorPtr)
		{
			OnwerActorPtr->GetGroupSharedInfo()->GetTeamMatesHelperComponent()->SwitchTeammateOption(
				ETeammateOption::kAssistance);
		}
	}

	void FHumanRegularProcessor::F10KeyPressed()
	{
		UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<HumanProcessor::FHumanViewRaffleMenuProcessor>();
	}

	void FHumanRegularProcessor::EKeyPressed()
	{
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();

		if (OnwerActorPtr->LookAtSceneActorPtr)
		{
			OnwerActorPtr->InteractionSceneActor(Cast<ASceneActor>(OnwerActorPtr->LookAtSceneActorPtr));
		}
	}

	void FHumanRegularProcessor::EKeyReleased()
	{
	}

	void FHumanRegularProcessor::FKeyPressed()
	{
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();

		if (OnwerActorPtr->LookAtSceneActorPtr)
		{
			if (auto CharacterPtr = Cast<AHumanCharacter_AI>(OnwerActorPtr->LookAtSceneActorPtr))
			{
				OnwerActorPtr->InteractionSceneCharacter(CharacterPtr);
			}
		}
	}

	void FHumanRegularProcessor::QKeyPressed()
	{
		auto PCPtr = Cast<APlanetPlayerController>(GetOwnerActor()->GetController());

		if (PCPtr->PlayerInput->IsPressed(EKeys::LeftAlt))
		{
			UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<HumanProcessor::FBuildingBaseProcessor>();
		}
	}

	void FHumanRegularProcessor::VKeyPressed()
	{
	}

	void FHumanRegularProcessor::BKeyPressed()
	{
		UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<FHumanViewAlloctionSkillsProcessor>();
	}

	void FHumanRegularProcessor::TabKeyPressed()
	{
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
		if (!OnwerActorPtr)
		{
			return;
		}

		auto PCPtr = OnwerActorPtr->GetController<APlanetPlayerController>();
		if (!PCPtr)
		{
			return;
		}

		auto PlayerCameraManagerPtr = UGameplayStatics::GetPlayerCameraManager(GetWorldImp(), 0);
		if (PlayerCameraManagerPtr)
		{
			FVector OutCamLoc;
			FRotator OutCamRot;
			PlayerCameraManagerPtr->GetCameraViewPoint(OutCamLoc, OutCamRot);

			FCollisionObjectQueryParams ObjectQueryParams;
			ObjectQueryParams.AddObjectTypesToQuery(Pawn_Object);

			FCollisionQueryParams Params;
			Params.AddIgnoredActor(OnwerActorPtr);

			FHitResult OutHit;
			if (GetWorldImp()->LineTraceSingleByObjectType(OutHit, OutCamLoc, OutCamLoc + (OutCamRot.Vector() * 1000),
			                                               ObjectQueryParams, Params))
			{
				if (PCPtr->GetFocusActor() == OutHit.GetActor())
				{
				}
				else
				{
					auto TargetCharacterPtr = Cast<AHumanCharacter>(OutHit.GetActor());
					if (TargetCharacterPtr)
					{
						PCPtr->SetFocus(TargetCharacterPtr);
						return;
					}
				}
			}
		}

		if (PCPtr->GetFocusActor())
		{
			PCPtr->ClearFocus();
		}
	}

	void FHumanRegularProcessor::LAltKeyPressed()
	{
		bIsPressdLeftAlt = true;
	}

	void FHumanRegularProcessor::LAltKeyReleased()
	{
		if (bIsPressdLeftAlt)
		{
			auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
			if (!OnwerActorPtr)
			{
				return;
			}

			auto PlayerPCPtr = OnwerActorPtr->GetController<APlayerController>();
			if (PlayerPCPtr)
			{
				if (PlayerPCPtr->bShowMouseCursor > 0)
				{
					PlayerPCPtr->bShowMouseCursor = 0;
					UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerPCPtr);
				}
				else
				{
					PlayerPCPtr->bShowMouseCursor = 1;
					UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PlayerPCPtr);
				}
			}
		}

		bIsPressdLeftAlt = false;
	}

	void FHumanRegularProcessor::GKeyPressed()
	{
		UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<FHumanViewTalentAllocation>();

		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
		if (!OnwerActorPtr)
		{
			return;
		}

		FMinimalViewInfo DesiredView;
		OnwerActorPtr->GetCameraComp()->GetCameraView(0, DesiredView);

		auto StartPt = DesiredView.Location;
		auto StopPt = DesiredView.Location + (DesiredView.Rotation.Vector() * 1000);

		FHitResult Result;

		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(VoxelWorld_Object);

		FCollisionQueryParams Params;
		Params.bTraceComplex = false;

		if (OnwerActorPtr->GetWorld()->LineTraceSingleByObjectType(
				Result,
				StartPt,
				StopPt,
				ObjectQueryParams,
				Params)
		)
		{
#ifdef WITH_EDITOR
			if (HumanRegularProcessor.GetValueOnGameThread())
			{
				DrawDebugSphere(OnwerActorPtr->GetWorld(), Result.ImpactPoint, 20, 10, FColor::Red, true);
			}
#endif
		}
	}

	void FHumanRegularProcessor::HKeyPressed()
	{
		UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<FViewGroupsProcessor>();
	}

	void FHumanRegularProcessor::XKeyPressed()
	{
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
		if (OnwerActorPtr)
		{
			OnwerActorPtr->GetProxyProcessComponent()->SwitchWeapon();
		}
	}

	void FHumanRegularProcessor::AddOrRemoveUseMenuItemEvent(bool bIsAdd)
	{
		HandleKeysMap.Empty();
		if (bIsAdd)
		{
			auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
			if (OnwerActorPtr)
			{
				const auto CanbeActivedInfoAry = OnwerActorPtr->GetProxyProcessComponent()->GetCanbeActiveSocket();
				for (const auto& Iter : CanbeActivedInfoAry)
				{
					HandleKeysMap.Add(Iter.Value, Iter.Key);
				}
			}
		}
	}

	void FHumanRegularProcessor::UpdateLookAtObject()
	{
		IncreaseAsyncTaskNum();
		ON_SCOPE_EXIT
		{
			ReduceAsyncTaskNum();
		};
	}
}
