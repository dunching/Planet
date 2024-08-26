// Copyright Epic Games, Inc. All Rights Reserved.

#include "AITask_ReleaseSkill.h"

#include "UObject/Package.h"
#include "TimerManager.h"
#include "AISystem.h"
#include "AIController.h"
#include "VisualLogger/VisualLogger.h"
#include "AIResources.h"
#include "GameplayTasksComponent.h"

#include "InteractiveSkillComponent.h"
#include "InteractiveToolComponent.h"
#include "CharacterBase.h"
#include "Skill_Base.h"
#include "AssetRefMap.h"
#include "GameplayTagsSubSystem.h"
#include "InteractiveBaseGAComponent.h"

UAITask_ReleaseSkill::UAITask_ReleaseSkill(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsPausable = true;
}

void UAITask_ReleaseSkill::Activate()
{
	Super::Activate();

	ConditionalPerformTask();
}

void UAITask_ReleaseSkill::ConditionalPerformTask()
{
	TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &ThisClass::PerformTask), Frequency);
}

void UAITask_ReleaseSkill::SetUp(ACharacterBase* InChracterPtr)
{
	CharacterPtr = InChracterPtr;
}

bool UAITask_ReleaseSkill::PerformTask(float)
{
	if (bIsPauseRelease)
	{
		StopReleaseSkill();
		return true;
	}

	if (CharacterPtr)
	{
		auto GASPtr = CharacterPtr->GetAbilitySystemComponent();

		FGameplayTagContainer GameplayTagContainer;
		GameplayTagContainer.AddTag(UGameplayTagsSubSystem::GetInstance()->State_ReleasingSkill_Continuous);
		GameplayTagContainer.AddTag(UGameplayTagsSubSystem::GetInstance()->State_MoveToAttaclArea);

		if (GASPtr->MatchesGameplayTagQuery(FGameplayTagQuery::MakeQuery_MatchAnyTags(GameplayTagContainer)))
		{
			// 
		}
		else
		{
			auto CanbeActivedInfo = CharacterPtr->GetInteractiveSkillComponent()->GetCanbeActiveAction();
			{
				for (const auto& Iter : CanbeActivedInfo)
				{
					switch (Iter->Type)
					{
					case FCanbeInteractionInfo::EType::kActiveSkill:
					{
						auto Skills = CharacterPtr->GetInteractiveSkillComponent()->GetSkills();
						auto SkillIter = Skills.Find(Iter->Socket);
						if (!SkillIter)
						{
							continue;
						}
						auto GAInsPtr = Cast<USkill_Base>((*SkillIter)->SkillUnitPtr->GAInstPtr);
						if (!GAInsPtr)
						{
							continue;
						}

						auto bIsReady = GAInsPtr->CanActivateAbility(
							GAInsPtr->GetCurrentAbilitySpecHandle(),
							GAInsPtr->GetCurrentActorInfo()
						);
						if (bIsReady)
						{
							if (CharacterPtr->GetInteractiveSkillComponent()->ActiveAction(Iter))
							{
								ReleasingSkillMap.Add(GAInsPtr->GetCurrentAbilitySpecHandle(), Iter);
								return true;
							}
						}
					}
					break;
					}
				}
			}
			if (ReleasingSkillMap.IsEmpty())
			{
				for (const auto& Iter : CanbeActivedInfo)
				{
					switch (Iter->Type)
					{
					case FCanbeInteractionInfo::EType::kWeaponActiveSkill:
					{
						auto WeaponSPtr = CharacterPtr->GetInteractiveSkillComponent()->GetActivedWeapon();
						if (!WeaponSPtr)
						{
							continue;
						}
						auto GameplayAbilitySpecPtr = GASPtr->FindAbilitySpecFromHandle(WeaponSPtr->Handle);
						if (!GameplayAbilitySpecPtr)
						{
							continue;
						}
						auto GAInsPtr = Cast<USkill_Base>(GameplayAbilitySpecPtr->GetPrimaryInstance());
						if (!GAInsPtr)
						{
							continue;
						}

						auto bIsReady = GAInsPtr->CanActivateAbility(
							GAInsPtr->GetCurrentAbilitySpecHandle(),
							GAInsPtr->GetCurrentActorInfo()
						);
						if (bIsReady)
						{
							if (CharacterPtr->GetInteractiveSkillComponent()->ActiveAction(Iter, true))
							{
								ReleasingSkillMap.Add(WeaponSPtr->Handle, Iter);
								return true;
							}
						}
					}
					break;
					}
				}
			}
		}
	}
	return true;
}

void UAITask_ReleaseSkill::StopReleaseSkill()
{
	// 结束移动释放至范围内释放
	CharacterPtr->GetInteractiveBaseGAComponent()->BreakMoveToAttackDistance();

	for (const auto Iter : ReleasingSkillMap)
	{
		// 1.如果不取消这个回调，CancelAction会调用无效的成员函数（UE判断过了 不会崩溃 但是逻辑不对）
		auto GASPtr = CharacterPtr->GetAbilitySystemComponent();
		auto GameplayAbilitySpecPtr = GASPtr->FindAbilitySpecFromHandle(Iter.Key);
		if (!GameplayAbilitySpecPtr)
		{
			return;
		}
		auto GAInsPtr = Cast<USkill_Base>(GameplayAbilitySpecPtr->GetPrimaryInstance());
		if (!GAInsPtr)
		{
			return;
		}

		// 2.
		CharacterPtr->GetInteractiveSkillComponent()->CancelAction(Iter.Value);
	}
	ReleasingSkillMap.Empty();
}

void UAITask_ReleaseSkill::OnOnGameplayAbilityEnded(UGameplayAbility* GAPtr)
{
	if (GAPtr && ReleasingSkillMap.Contains(GAPtr->GetCurrentAbilitySpecHandle()))
	{
		ReleasingSkillMap.Remove(GAPtr->GetCurrentAbilitySpecHandle());
	}

	if (ReleasingSkillMap.IsEmpty())
	{
		EndTask();
	}
}

void UAITask_ReleaseSkill::OnDestroy(bool bInOwnerFinished)
{
	FTSTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);

	StopReleaseSkill();

	Super::OnDestroy(bInOwnerFinished);
}
