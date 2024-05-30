// Copyright Epic Games, Inc. All Rights Reserved.

#include "AITask_ReleaseSkill.h"

#include "UObject/Package.h"
#include "TimerManager.h"
#include "AISystem.h"
#include "AIController.h"
#include "VisualLogger/VisualLogger.h"
#include "AIResources.h"
#include "GameplayTasksComponent.h"

#include "EquipmentElementComponent.h"
#include "CharacterBase.h"
#include "Skill_Base.h"
#include "AssetRefMap.h"

UAITask_ReleaseSkill::UAITask_ReleaseSkill(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsPausable = true;
}

void UAITask_ReleaseSkill::Activate()
{
	Super::Activate();

	ConditionalPerformMove();
}

void UAITask_ReleaseSkill::ConditionalPerformMove()
{
	PerformMove();
}

bool UAITask_ReleaseSkill::WasMoveSuccessful() const
{
	return ReleasingSkillMap.IsEmpty();
}

void UAITask_ReleaseSkill::SetUp(ACharacterBase* InChracterPtr)
{
	CharacterPtr = InChracterPtr;
}

void UAITask_ReleaseSkill::PerformMove()
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
		GameplayTagContainer.AddTag(UAssetRefMap::GetInstance()->GameplayTag1);

		if (GASPtr->MatchesGameplayTagQuery(FGameplayTagQuery::MakeQuery_MatchAnyTags(GameplayTagContainer)))
		{
		}
		else
		{
			auto CanbeActivedInfo = CharacterPtr->GetEquipmentItemsComponent()->GetCanbeActivedInfo();
			auto Skills = CharacterPtr->GetEquipmentItemsComponent()->GetSkills();
			{
				for (const auto& Iter : CanbeActivedInfo)
				{
					switch (Iter->Type)
					{
					case FCanbeActivedInfo::EType::kActiveSkill:
					{
						auto SkillIter = Skills.Find(Iter->SkillSocket);
						if (!SkillIter)
						{
							continue;
						}
						auto GameplayAbilitySpecPtr = GASPtr->FindAbilitySpecFromHandle((*SkillIter)->Handle);
						if (!GameplayAbilitySpecPtr)
						{
							continue;
						}
						auto GAInsPtr = Cast<USkill_Base>(GameplayAbilitySpecPtr->GetPrimaryInstance());
						if (!GAInsPtr)
						{
							continue;
						}

						auto bIsReady = GAInsPtr->CanActivateAbility(GAInsPtr->GetCurrentAbilitySpecHandle(), GAInsPtr->GetCurrentActorInfo());
						if (bIsReady)
						{
							ReleasingSkillMap.Add((*SkillIter)->Handle, Iter);
							ReleasingSkillDelegateMap.Add(
								(*SkillIter)->Handle,
								GAInsPtr->OnGameplayAbilityEnded.AddUObject(this, &ThisClass::OnOnGameplayAbilityEnded)
							);

							if (CharacterPtr->GetEquipmentItemsComponent()->ActiveSkill(Iter))
							{
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
					case FCanbeActivedInfo::EType::kWeaponActiveSkill:
					{
						auto SkillIter = Skills.Find(Iter->SkillSocket);
						if (!SkillIter)
						{
							continue;
						}
						auto GameplayAbilitySpecPtr = GASPtr->FindAbilitySpecFromHandle((*SkillIter)->Handle);
						if (!GameplayAbilitySpecPtr)
						{
							continue;
						}
						auto GAInsPtr = Cast<USkill_Base>(GameplayAbilitySpecPtr->GetPrimaryInstance());
						if (!GAInsPtr)
						{
							continue;
						}

						auto bIsReady = GAInsPtr->CanActivateAbility(GAInsPtr->GetCurrentAbilitySpecHandle(), GAInsPtr->GetCurrentActorInfo());
						if (bIsReady)
						{
							ReleasingSkillMap.Add((*SkillIter)->Handle, Iter);
							ReleasingSkillDelegateMap.Add(
								(*SkillIter)->Handle,
								GAInsPtr->OnGameplayAbilityEnded.AddUObject(this, &ThisClass::OnOnGameplayAbilityEnded)
							);

							if (CharacterPtr->GetEquipmentItemsComponent()->ActiveSkill(Iter))
							{
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
		// 1.如果不取消这个回调，CancelSkill会调用无效的成员函数（UE判断过了 不会崩溃 但是逻辑不对）
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
	//	CharacterPtr->GetEquipmentItemsComponent()->CancelSkill(Iter.Value);
	}

	Super::OnDestroy(bInOwnerFinished);
}
