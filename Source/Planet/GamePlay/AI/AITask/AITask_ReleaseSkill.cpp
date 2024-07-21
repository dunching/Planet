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
	CurrentTaslHasReleaseNum = 0;

	PerformTask();
}

bool UAITask_ReleaseSkill::WasMoveSuccessful() const
{
	return CurrentTaslHasReleaseNum > 0;
}

void UAITask_ReleaseSkill::SetUp(ACharacterBase* InChracterPtr)
{
	CharacterPtr = InChracterPtr;
}

void UAITask_ReleaseSkill::PerformTask()
{
	if (ReleasingSKill())
	{

	}
	else
	{
		EndTask();
	}
}

void UAITask_ReleaseSkill::OnOnGameplayAbilityEnded(UGameplayAbility* GAPtr)
{
	if (GAPtr && ReleasingSkillMap.Contains(GAPtr->GetCurrentAbilitySpecHandle()))
	{
		ReleasingSkillMap.Remove(GAPtr->GetCurrentAbilitySpecHandle());
	}

	if (GAPtr && ReleasingSkillDelegateMap.Contains(GAPtr->GetCurrentAbilitySpecHandle()))
	{
		ReleasingSkillDelegateMap.Remove(GAPtr->GetCurrentAbilitySpecHandle());
	}

	if (ReleasingSkillMap.IsEmpty() && ReleasingSkillDelegateMap.IsEmpty())
	{
		EndTask();
	}
}

bool UAITask_ReleaseSkill::ReleasingSKill()
{
	if (CharacterPtr)
	{
		auto GASPtr = CharacterPtr->GetAbilitySystemComponent();

		FGameplayTagContainer GameplayTagContainer;
		GameplayTagContainer.AddTag(UGameplayTagsSubSystem::GetInstance()->GameplayTag1);

		if (GASPtr->MatchesGameplayTagQuery(FGameplayTagQuery::MakeQuery_MatchAnyTags(GameplayTagContainer)))
		{
		}
		else
		{
			auto CanbeActivedInfo = CharacterPtr->GetInteractiveSkillComponent()->GetCanbeActiveAction();
			{
				for (const auto& Iter : CanbeActivedInfo)
				{
					switch (Iter->Type)
					{
					case FCanbeActivedInfo::EType::kActiveSkill:
					{
						auto Skills = CharacterPtr->GetInteractiveSkillComponent()->GetSkills();
						auto SkillIter = Skills.Find(Iter->Socket);
						if (!SkillIter)
						{
							continue;
						}
						for (const auto SkillHandleIter : (*SkillIter)->HandleAry)
						{
							auto GameplayAbilitySpecPtr = GASPtr->FindAbilitySpecFromHandle(SkillHandleIter);
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
								if (CharacterPtr->GetInteractiveSkillComponent()->ActiveAction(Iter))
								{
									ReleasingSkillMap.Add(SkillHandleIter, Iter);
									ReleasingSkillDelegateMap.Add(
										SkillHandleIter,
										GAInsPtr->OnGameplayAbilityEnded.AddUObject(this, &ThisClass::OnOnGameplayAbilityEnded)
									);
									CurrentTaslHasReleaseNum++;
									return true;
								}
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
					case FCanbeActivedInfo::EType::kWeaponActiveSkill:
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
								ReleasingSkillDelegateMap.Add(
									WeaponSPtr->Handle,
									GAInsPtr->OnGameplayAbilityEnded.AddUObject(this, &ThisClass::OnOnGameplayAbilityEnded)
								);
								CurrentTaslHasReleaseNum++;
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

	return false;
}

void UAITask_ReleaseSkill::OnDestroy(bool bInOwnerFinished)
{
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

		GAInsPtr->OnGameplayAbilityEnded.Remove(ReleasingSkillDelegateMap[Iter.Key]);

		// 2.
		CharacterPtr->GetInteractiveSkillComponent()->CancelAction(Iter.Value);
	}

	Super::OnDestroy(bInOwnerFinished);
}
