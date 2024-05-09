
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

#include "LogHelper/LogWriter.h"
#include "ThreadPoolHelper/ThreadLibrary.h"
#include "ToolsLibrary.h"
#include "Animation/AnimInstanceBase.h"
#include "GenerateType.h"
#include "PlanetPlayerState.h"
#include "CharacterBase.h"
#include "ArticleBase.h"
#include "HumanCharacter.h"
#include "PlacingBuildingAreaProcessor.h"
#include "ActionTrackVehiclePlace.h"
#include "PlacingWallProcessor.h"
#include "PlacingGroundProcessor.h"
#include <Pawn/EquipmentElementComponent.h>
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
#include "PlanetGameplayAbility_Dash.h"
#include "AssetRefMap.h"
#include "BuildingBaseProcessor.h"
#include "CharacterAttributesComponent.h"
#include "PlanetPlayerController.h"
#include "HumanViewAlloctionSkillsProcessor.h"
#include "CollisionDataStruct.h"
#include "ToolFuture_PickAxe.h"
#include "Weapon_Base.h"
#include "Skill_Base.h"
#include "Skill_PickAxe.h"
#include "Skill_WeaponHandProtection.h"
#include "Weapon_HandProtection.h"
#include "Weapon_PickAxe.h"
#include "HumanViewTalentAllocation.h"
#include "HumanViewGroupManagger.h"

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

		AddOrRemoveUseMenuItemEvent(true);
	}

	void FHumanRegularProcessor::SwitchCurrentWeapon()
	{
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>(); 
		if (OnwerActorPtr)
		{
			UUIManagerSubSystem::GetInstance()->DisplayActionStateHUD(true, OnwerActorPtr);
			UUIManagerSubSystem::GetInstance()->DisplayTeamInfo(true);

			{
				auto WeaponUnitPtr = OnwerActorPtr->GetEquipmentItemsComponent()->GetMainWeaponUnit();
				if (WeaponUnitPtr && WeaponUnitPtr->GetSceneToolsType() == ESceneToolsType::kWeapon)
				{
					OnwerActorPtr->GetEquipmentItemsComponent()->ActiveWeapon(EWeaponSocket::kMain);
					return;
				}
			}
			{
				auto WeaponUnitPtr = OnwerActorPtr->GetEquipmentItemsComponent()->GetSecondaryWeaponUnit();
				if (WeaponUnitPtr && WeaponUnitPtr->GetSceneToolsType() == ESceneToolsType::kWeapon)
				{
					OnwerActorPtr->GetEquipmentItemsComponent()->ActiveWeapon(EWeaponSocket::kSecondary);
					return;
				}
			}
			OnwerActorPtr->SwitchAnimLink(EAnimLinkClassType::kUnarmed);
		}
	}

	void FHumanRegularProcessor::QuitAction()
	{
		AddOrRemoveUseMenuItemEvent(false);

		UUIManagerSubSystem::GetInstance()->DisplayActionStateHUD(false);
		UUIManagerSubSystem::GetInstance()->DisplayTeamInfo(false);

		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
		if (OnwerActorPtr)
		{
			OnwerActorPtr->GetEquipmentItemsComponent()->ActiveWeapon(EWeaponSocket::kNone);
		}

		Super::QuitAction();
	}

	void FHumanRegularProcessor::InputKey(const FInputKeyParams& Params)
	{
		if (Params.Event == EInputEvent::IE_Pressed)
		{
			auto SkillIter = HandleKeysMap.Find(Params.Key);
			if (SkillIter)
			{
				switch (SkillIter->SkillUnit->SkillType)
				{
				case ESkillType::kWeaponActive:
				{
					AWeapon_Base* WeaponPtr = nullptr;

					if (Params.Key == EKeys::LeftMouseButton)
					{
						auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
						WeaponPtr = OnwerActorPtr->GetEquipmentItemsComponent()->ActiveWeapon(EWeaponSocket::kMain);


					}
					else if (Params.Key == EKeys::RightMouseButton)
					{
						auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
						WeaponPtr = OnwerActorPtr->GetEquipmentItemsComponent()->ActiveWeapon(EWeaponSocket::kSecondary);
					}

					switch (SkillIter->SkillUnit->GetSceneElementType<ESkillUnitType>())
					{
					case ESkillUnitType::kHumanSkill_PickAxe_Attack1:
					{
						auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
						if (OnwerActorPtr)
						{
							FGameplayEventData Payload;
							auto GameplayAbilityTargetDashPtr = new FGameplayAbilityTargetData_Skill_PickAxe;
							GameplayAbilityTargetDashPtr->WeaponPtr = Cast<AWeapon_PickAxe>(WeaponPtr);
							Payload.TargetData.Add(GameplayAbilityTargetDashPtr);
							auto ASCPtr = OnwerActorPtr->GetAbilitySystemComponent();
							ASCPtr->TriggerAbilityFromGameplayEvent(
								SkillIter->Handle,
								ASCPtr->AbilityActorInfo.Get(),
								FGameplayTag(),
								&Payload,
								*ASCPtr
							);
						}
					}
					break;
					case ESkillUnitType::kHumanSkill_WeaponHandProtection_Attack1:
					{
						auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
						if (OnwerActorPtr)
						{
							FGameplayEventData Payload;
							auto GameplayAbilityTargetDashPtr = new FGameplayAbilityTargetData_Skill_WeaponHandProtection;
							GameplayAbilityTargetDashPtr->WeaponPtr = Cast<AWeapon_HandProtection>(WeaponPtr);
							Payload.TargetData.Add(GameplayAbilityTargetDashPtr);
							auto ASCPtr = OnwerActorPtr->GetAbilitySystemComponent();
							ASCPtr->TriggerAbilityFromGameplayEvent(
								SkillIter->Handle,
								ASCPtr->AbilityActorInfo.Get(),
								FGameplayTag(),
								&Payload,
								*ASCPtr
							);
						}
					}
					break;
					}
				}
				break;
				case ESkillType::kActive:
				{
					switch (SkillIter->SkillUnit->GetSceneElementType<ESkillUnitType>())
					{
					case ESkillUnitType::kHumanSkill_Displacement:
					{
						auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
						if (OnwerActorPtr)
						{
							auto ASCPtr = OnwerActorPtr->GetAbilitySystemComponent();
							ASCPtr->TryActivateAbility(SkillIter->Handle);
						}
					}
					break;
					}
				}
				break;
				}

			}
		}
		else
		{
			auto SkillIter = HandleKeysMap.Find(Params.Key);
			if (SkillIter)
			{
				auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
				if (OnwerActorPtr)
				{
					auto ASCPtr = OnwerActorPtr->GetAbilitySystemComponent();
					ASCPtr->CancelAbilityHandle(SkillIter->Handle);
				}
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
		UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<FHumanViewAlloctionSkillsProcessor>();
	}

	void FHumanRegularProcessor::BKeyPressed()
	{
		UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<FViewBackpackProcessor>();
	}

	void FHumanRegularProcessor::GKeyPressed()
	{
		UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<FHumanViewTalentAllocation>();

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

	void FHumanRegularProcessor::HKeyPressed()
	{
		UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<FHumanViewGroupManagger>();
	}

	void FHumanRegularProcessor::AddOrRemoveUseMenuItemEvent(bool bIsAdd)
	{
		HandleKeysMap.Empty();
		if (bIsAdd)
		{
			auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
			if (OnwerActorPtr)
			{
				auto ActiveSkillsAry = OnwerActorPtr->GetEquipmentItemsComponent()->GetSkills();
				for (const auto& Iter : ActiveSkillsAry)
				{
					switch (Iter.Value.SkillUnit->SkillType)
					{
					case ESkillType::kActive:
					case ESkillType::kWeaponActive:
					{
						HandleKeysMap.Add(Iter.Value.Key, Iter.Value);
					}
					break;
					}
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
