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

		if (GASPtr->MatchesGameplayTagQuery(FGameplayTagQuery::MakeQuery_MatchNoTags(GameplayTagContainer)))
		{
			auto SkillsMap = CharacterPtr->GetEquipmentItemsComponent()->GetSkills();
			for (const auto& Iter : SkillsMap)
			{
				if (Iter.Value->SkillUnit)
				{
					switch (Iter.Value->SkillUnit->SkillType)
					{
					case ESkillType::kActive:
					{
						auto GameplayAbilitySpecPtr = GASPtr->FindAbilitySpecFromHandle(Iter.Value->Handle);
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
							ReleasingSkillMap.Add(Iter.Value->Handle, Iter.Value);
							ReleasingSkillDelegateMap.Add(
								Iter.Value->Handle,
								GAInsPtr->OnGameplayAbilityEnded.AddUObject(this, &ThisClass::OnOnGameplayAbilityEnded)
							);

							//		CharacterPtr->GetEquipmentItemsComponent()->ActiveSkill(Iter.Value);
						}
					}
					break;
					}
				}
			}
			if (ReleasingSkillMap.IsEmpty())
			{
				for (const auto& Iter : SkillsMap)
				{
					if (Iter.Value->SkillUnit)
					{
						switch (Iter.Value->SkillUnit->SkillType)
						{
						case ESkillType::kWeaponActive:
						{
							auto GameplayAbilitySpecPtr = GASPtr->FindAbilitySpecFromHandle(Iter.Value->Handle);
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
								ReleasingSkillMap.Add(Iter.Value->Handle, Iter.Value);
								ReleasingSkillDelegateMap.Add(
									Iter.Value->Handle,
									GAInsPtr->OnGameplayAbilityEnded.AddUObject(this, &ThisClass::OnOnGameplayAbilityEnded)
								);

								//	CharacterPtr->GetEquipmentItemsComponent()->ActiveSkill(Iter.Value);
							}
						}
						break;
						}
					}
				}
			}
		}
	}

	return !ReleasingSkillMap.IsEmpty();
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
