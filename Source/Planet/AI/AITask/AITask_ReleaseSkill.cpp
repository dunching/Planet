// Copyright Epic Games, Inc. All Rights Reserved.

#include "AITask_ReleaseSkill.h"

#include "UObject/Package.h"
#include "TimerManager.h"
#include "AISystem.h"
#include "AIController.h"
#include "VisualLogger/VisualLogger.h"
#include "AIResources.h"
#include "GameplayTasksComponent.h"

#include "ProxyProcessComponent.h"

#include "CharacterBase.h"
#include "Skill_Base.h"
#include "AssetRefMap.h"
#include "GameplayTagsLibrary.h"
#include "BaseFeatureComponent.h"

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
	if (CharacterPtr)
	{
		auto GASPtr = CharacterPtr->GetAbilitySystemComponent();

		FGameplayTagContainer GameplayTagContainer;
		GameplayTagContainer.AddTag(UGameplayTagsLibrary::State_ReleasingSkill_Continuous);
		GameplayTagContainer.AddTag(UGameplayTagsLibrary::State_MoveToAttaclArea);

		if (GASPtr->MatchesGameplayTagQuery(FGameplayTagQuery::MakeQuery_MatchAnyTags(GameplayTagContainer)))
		{
			// 
		}
		else
		{
			{
				const auto CanbeActivedInfo = CharacterPtr->GetProxyProcessComponent()->GetCanbeActiveSkills();
				for (const auto& Iter : CanbeActivedInfo)
				{
					if (
						Iter.Value.Socket.MatchesTag(UGameplayTagsLibrary::ActiveSocket)
						)
					{
						auto SkillProxySPtr = CharacterPtr->GetProxyProcessComponent()->FindActiveSkillBySocket(Iter.Value.Socket);
						auto GAInsPtr = Cast<USkill_Base>(SkillProxySPtr->GetGAInst());
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
							if (CharacterPtr->GetProxyProcessComponent()->ActiveAction(Iter.Value.Socket))
							{
								ReleasingSkillMap.Add(GAInsPtr, Iter.Value.Socket);
								return true;
							}
						}
					}
				}
			}

			// 未释放主动技能
			const auto CanbeActivedInfo = CharacterPtr->GetProxyProcessComponent()->GetCanbeActiveWeapon();
			for (const auto& Iter : CanbeActivedInfo)
			{
				if (
					Iter.Value.Socket.MatchesTag(UGameplayTagsLibrary::WeaponSocket)
					)
				{
					auto WeaponSPtr = CharacterPtr->GetProxyProcessComponent()->GetActivedWeapon();
					if (!WeaponSPtr)
					{
						continue;
					}
					auto GAInsPtr = Cast<USkill_Base>(WeaponSPtr->GetWeaponSkill()->GetGAInst());
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
						if (CharacterPtr->GetProxyProcessComponent()->ActiveAction(Iter.Value.Socket, true))
						{
							ReleasingSkillMap.Add(GAInsPtr, Iter.Value.Socket);
							return true;
						}
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
	CharacterPtr->GetBaseFeatureComponent()->BreakMoveToAttackDistance();

	for (const auto Iter : ReleasingSkillMap)
	{
		// 1.如果不取消这个回调，CancelAction会调用无效的成员函数（UE判断过了 不会崩溃 但是逻辑不对）
		auto GAInsPtr = Cast<USkill_Base>(Iter.Key);
		if (!GAInsPtr)
		{
			return;
		}

		// 2.
		CharacterPtr->GetProxyProcessComponent()->CancelAction(Iter.Value);
	}
	ReleasingSkillMap.Empty();
}

void UAITask_ReleaseSkill::OnOnGameplayAbilityEnded(UGameplayAbility* GAPtr)
{
	if (GAPtr && ReleasingSkillMap.Contains(GAPtr))
	{
		ReleasingSkillMap.Remove(GAPtr);
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
