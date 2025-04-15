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
#include "CharacterAbilitySystemComponent.h"

UAITask_ReleaseSkill::UAITask_ReleaseSkill(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsPausable = true;
	bTickingTask = true;
}

void UAITask_ReleaseSkill::Activate()
{
	Super::Activate();

	ConditionalPerformTask();
}

void UAITask_ReleaseSkill::TickTask(
	float DeltaTime
)
{
	Super::TickTask(DeltaTime);
	if (CharacterPtr)
	{
		auto GASPtr = CharacterPtr->GetCharacterAbilitySystemComponent();

		FGameplayTagContainer GameplayTagContainer;
		GameplayTagContainer.AddTag(UGameplayTagsLibrary::State_ReleasingSkill_Continuous);
		GameplayTagContainer.AddTag(UGameplayTagsLibrary::State_MoveToLocation);

		if (GASPtr->MatchesGameplayTagQuery(FGameplayTagQuery::MakeQuery_MatchAnyTags(GameplayTagContainer)))
		{
			// 
		}
		else
		{
			const auto CanbeActivedInfo = CharacterPtr->GetProxyProcessComponent()->GetCanbeActiveSocket();
			{
				for (const auto& Iter : CanbeActivedInfo)
				{
					if (
						Iter.Key.Socket.MatchesTag(UGameplayTagsLibrary::ActiveSocket)
						)
					{
						auto SkillProxySPtr = CharacterPtr->GetProxyProcessComponent()->FindActiveSkillBySocket(Iter.Key.Socket);
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
							if (CharacterPtr->GetProxyProcessComponent()->ActiveAction(Iter.Key.Socket))
							{
								ReleasingSkillMap.Add(GAInsPtr, Iter.Key.Socket);
								return;
							}
						}
					}
				}
			}

			// 未释放主动技能
			for (const auto& Iter : CanbeActivedInfo)
			{
				if (
					Iter.Key.Socket.MatchesTag(UGameplayTagsLibrary::WeaponSocket)
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
						if (CharacterPtr->GetProxyProcessComponent()->ActiveAction(Iter.Key.Socket, true))
						{
							ReleasingSkillMap.Add(GAInsPtr, Iter.Key.Socket);
							return;
						}
					}
				}
			}
		}
	}
	return;
}

void UAITask_ReleaseSkill::ConditionalPerformTask()
{
}

void UAITask_ReleaseSkill::SetUp(ACharacterBase* InChracterPtr)
{
	CharacterPtr = InChracterPtr;
}

void UAITask_ReleaseSkill::StopReleaseSkill()
{
	// 结束移动释放至范围内释放
	CharacterPtr->GetCharacterAbilitySystemComponent()->BreakMoveToAttackDistance();

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
	StopReleaseSkill();

	Super::OnDestroy(bInOwnerFinished);
}
