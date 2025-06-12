#include "HumanRegularProcessor.h"

#include "DrawDebugHelpers.h"
#include "Async/Async.h"
#include "Engine/LocalPlayer.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "ToolsLibrary.h"
#include "CharacterBase.h"
#include "ArticleBase.h"
#include "HumanCharacter_Player.h"
#include "PlacingBuildingAreaProcessor.h"
#include "ActionTrackVehiclePlace.h"
#include "ProxyProcessComponent.h"
#include "UIManagerSubSystem.h"
#include <DestroyProgress.h>
#include "InputProcessorSubSystemBase.h"
#include "HumanProcessor.h"
#include "HumanViewAlloctionSkillsProcessor.h"
#include "ViewGroupsProcessor.h"
#include "TeamMatesHelperComponentBase.h"
#include "HumanViewMapProcessor.h"

#include "GameplayTagsLibrary.h"
#include "ChallengeEntry.h"
#include "CharacterAbilitySystemComponent.h"
#include "GroupManagger.h"
#include "HumanCharacter_AI.h"
#include "HumanViewSetting.h"
#include "HumanViewTalentAllocation.h"
#include "InputProcessorSubSystem_Imp.h"
#include "ItemProxy_Character.h"
#include "TeamMatesHelperComponent.h"

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

	void FHumanRegularProcessor::SwitchWeapon()
	{
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();

		if (OnwerActorPtr)
		{
			OnwerActorPtr->GetCharacterAbilitySystemComponent()->SwitchWeapon();
		}
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
			OnAllocationChangedHandle.Reset();
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
						if (OnwerActorPtr->InteractionSceneActor(
						                                         Cast<ASceneActor>(OnwerActorPtr->LookAtSceneActorPtr),
						                                         OnwerActorPtr->LookAtSceneActorPtr
						                                        ))
						{
							return true;
						}
					}

					if (EventArgs.Key == GameOptionsPtr->InteractionWithSceneCharacter)
					{
						if (auto CharacterPtr = Cast<AHumanCharacter_AI>(OnwerActorPtr->LookAtSceneActorPtr))
						{
							if (OnwerActorPtr->InteractionSceneCharacter(CharacterPtr))
							{
								return true;
							}
						}
					}

					if (EventArgs.Key == GameOptionsPtr->InteractionWithChallengeEntry)
					{
						if (auto ChallengeEntryPtr = Cast<AChallengeEntry>(OnwerActorPtr->LookAtSceneActorPtr))
						{
							if (OnwerActorPtr->InteractionSceneActor(ChallengeEntryPtr, ChallengeEntryPtr))
							{
								return true;
							}
						}
					}
				}

				// 可激活的技能或消耗品插槽
				auto SkillIter = HandleKeysMap.Find(EventArgs.Key);
				if (SkillIter)
				{
					OnwerActorPtr->GetCharacterAbilitySystemComponent()->BreakMoveToAttackDistance();

					OnwerActorPtr->GetProxyProcessComponent()->ActiveAction(SkillIter->Socket);
					return true;
				}

				if (EventArgs.Key == GameOptionsPtr->SwitchWeapon)
				{
					SwitchWeapon();
					return true;
				}

				if (EventArgs.Key == GameOptionsPtr->ViewSetting)
				{
					UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<FHumanViewSetting>();
					return true;
				}

				if (EventArgs.Key == GameOptionsPtr->ViewGroupmateMenu)
				{
					UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<FViewGroupsProcessor>();
					return true;
				}

				if (EventArgs.Key == GameOptionsPtr->ViewAllocationMenu)
				{
					UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<
						FHumanViewAlloctionSkillsProcessor>();
					return true;
				}

				if (EventArgs.Key == GameOptionsPtr->ViewMap)
				{
					UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<FHumanViewMapProcessor>();
					return true;
				}

				if (EventArgs.Key == GameOptionsPtr->ViewTalentAllocation)
				{
					UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<FHumanViewTalentAllocation>();
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
						SkillIter->Socket.MatchesTag(UGameplayTagsLibrary::WeaponSocket) ||
						SkillIter->Socket.MatchesTag(UGameplayTagsLibrary::ActiveSocket)
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
		const FInputKeyEventArgs& EventArgs
		)
	{
		return Super::InputAxis(EventArgs);
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
