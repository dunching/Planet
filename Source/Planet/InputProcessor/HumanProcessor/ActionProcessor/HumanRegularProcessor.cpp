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
#include "ChallengeEntry.h"
#include "HumanViewRaffleMenu.h"
#include "CharacterAbilitySystemComponent.h"
#include "HumanCharacter_Player.h"
#include "ResourceBox.h"
#include "GroupManagger.h"
#include "HumanCharacter_AI.h"
#include "ItemProxy_Character.h"
#include "TextCollect.h"

static TAutoConsoleVariable<int32> HumanRegularProcessor(
	TEXT("Skill.DrawDebug.HumanRegularProcessor"),
	0,
	TEXT("")
	TEXT(" default: 0")
);

namespace HumanProcessor
{
	FHumanRegularProcessor::FHumanRegularProcessor(
		FOwnerPawnType* CharacterPtr
	) :
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
			SwitchShowCursor(false);

			UUIManagerSubSystem::GetInstance()->SwitchLayout(ELayoutCommon::kActionLayout);

			OnAllocationChangedHandle = OnwerActorPtr->GetProxyProcessComponent()->OnCurrentWeaponChanged.AddCallback(
				[this]()
				{
					AddOrRemoveUseMenuItemEvent(true);
				}
			);

			AddOrRemoveUseMenuItemEvent(true);
			const auto GameplayFeatureKeyMapAry = UGameOptions::GetInstance()->GetGameplayFeatureKeyMapAry();
			for (const auto& Iter : GameplayFeatureKeyMapAry)
			{
				// 忽略一些命令
				if (Iter.CMD == TextCollect::EntryActionProcessor)
				{
					const auto Key = Iter.Key;
				}

				GameplayFeatureKeyMapMap.Add(Iter.Key, Iter);
			}
		}
	}

	void FHumanRegularProcessor::TickImp(
		float Delta
	)
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
			OnwerActorPtr->GetGroupManagger()->GetTeamMatesHelperComponent()->TeammateCharacter_ActiveWeapon_Server();
		}
	}

	void FHumanRegularProcessor::QuitAction()
	{
		AddOrRemoveUseMenuItemEvent(false);

		// UUIManagerSubSystem::GetInstance()->DisplayActionLayout(false);

		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
		if (OnwerActorPtr)
		{
			OnwerActorPtr->GetProxyProcessComponent()->RetractputWeapon();
		}

		if (OnAllocationChangedHandle)
		{
			OnAllocationChangedHandle->UnBindCallback();
		}

		Super::QuitAction();
	}

	bool FHumanRegularProcessor::InputKey(
		const FInputKeyEventArgs& EventArgs
	)
	{
		switch (EventArgs.Event)
		{
		case IE_Pressed:
			{
				auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
				if (!OnwerActorPtr)
				{
					return false;
				}

				auto GameOptionsPtr = UGameOptions::GetInstance();

				if (EventArgs.Key == GameOptionsPtr->FocusTarget)
				{
					OnwerActorPtr->GetCharacterPlayerStateProcessorComponent()->FocusTarget();
					return true;
				}

				// 与场景里的对象交互，特殊处理一下
				if (OnwerActorPtr->LookAtSceneActorPtr)
				{
					if (EventArgs.Key == GameOptionsPtr->InteractionWithSceneActor)
					{
						OnwerActorPtr->InteractionSceneActor(Cast<ASceneActor>(OnwerActorPtr->LookAtSceneActorPtr));
						return true;
					}
					
					 if (EventArgs.Key == GameOptionsPtr->InteractionWithSceneCharacter)
					{
					 	if (auto CharacterPtr = Cast<AHumanCharacter_AI>(OnwerActorPtr->LookAtSceneActorPtr))
					 	{
					 		OnwerActorPtr->InteractionSceneCharacter(CharacterPtr);
					 		return true;
					 	}
					}
					
					 if (EventArgs.Key == GameOptionsPtr->InteractionWithChallengeEntry)
					 {
					 	if (auto ChallengeEntryPtr = Cast<AChallengeEntry>(OnwerActorPtr->LookAtSceneActorPtr))
					 	{
					 		OnwerActorPtr->InteractionSceneActor(ChallengeEntryPtr);
					 		return true;
					 	}
					}
				}

				auto GameplayFeatureKeyMapMapIter = GameplayFeatureKeyMapMap.Find(EventArgs.Key);
				if (GameplayFeatureKeyMapMapIter)
				{
					OnwerActorPtr->GetController<APlayerController>()->ConsoleCommand(
						GameplayFeatureKeyMapMapIter->CMD
					);
					return true;
				}

				auto SkillIter = HandleKeysMap.Find(EventArgs.Key);
				if (SkillIter)
				{
					OnwerActorPtr->GetCharacterAbilitySystemComponent()->BreakMoveToAttackDistance();

					OnwerActorPtr->GetProxyProcessComponent()->ActiveAction(SkillIter->Socket);
					return true;
				}

				// 这里应该是特定的输入会打断 还是任意输入都会打断？
				else if (
					(EventArgs.Key == EKeys::W) ||
					(EventArgs.Key == EKeys::A) ||
					(EventArgs.Key == EKeys::S) ||
					(EventArgs.Key == EKeys::D)
				)
				{
					OnwerActorPtr->GetCharacterAbilitySystemComponent()->BreakMoveToAttackDistance();
					return true;
				}
			}
			break;
		case IE_Released:
			{
				auto SkillIter = HandleKeysMap.Find(EventArgs.Key);
				if (SkillIter)
				{
					auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
					if (
						SkillIter->Socket.MatchesTag(UGameplayTagsLibrary::WeaponSocket)
					)
					{
						OnwerActorPtr->GetProxyProcessComponent()->CancelAction(SkillIter->Socket);
						return true;
					}
				}
			}
			break;
		}

		return Super::InputKey(EventArgs);
	}

	bool FHumanRegularProcessor::InputAxis(
		FViewport* Viewport,
		FInputDeviceId InputDevice,
		FKey Key,
		float Delta,
		float DeltaTime,
		int32 NumSamples,
		bool bGamepad
	)
	{
		return Super::InputAxis(Viewport, InputDevice, Key, Delta, DeltaTime, NumSamples, bGamepad);
	}

	void FHumanRegularProcessor::AddOrRemoveUseMenuItemEvent(
		bool bIsAdd
	)
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
