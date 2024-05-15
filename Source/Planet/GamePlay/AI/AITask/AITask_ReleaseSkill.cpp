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
	return bIsReleasingSkill;
}

void UAITask_ReleaseSkill::SetUp(ACharacterBase* InChracterPtr)
{
	CharacterPtr = InChracterPtr;
}

void UAITask_ReleaseSkill::PerformMove()
{
	if (CharacterPtr)
	{
		auto SkillsMap = CharacterPtr->GetEquipmentItemsComponent()->GetSkills();
		for (const auto& Iter : SkillsMap)
		{
			if (Iter.Value.SkillUnit)
			{
				switch (Iter.Value.SkillUnit->SkillType)
				{
				case ESkillType::kActive:
				{
					auto GASPtr = CharacterPtr->GetAbilitySystemComponent();
					auto GameplayAbilitySpecPtr = GASPtr->FindAbilitySpecFromHandle(Iter.Value.Handle);
					if (!GameplayAbilitySpecPtr)
					{
						return;
					}
					auto GAInsPtr = Cast<USkill_Base>(GameplayAbilitySpecPtr->GetPrimaryInstance());
					if (!GAInsPtr)
					{
						return;
					}

					auto bIsReady = GAInsPtr->CanActivateAbility(GAInsPtr->GetCurrentAbilitySpecHandle(), GAInsPtr->GetCurrentActorInfo());
					if (bIsReady)
					{
						bIsReleasingSkill = true;
						CurrentActivedSkill = Iter.Value;

						OnOnGameplayAbilityEndedHandle = GAInsPtr->OnGameplayAbilityEnded.AddUObject(this, &ThisClass::OnOnGameplayAbilityEnded);

						CharacterPtr->GetEquipmentItemsComponent()->ActiveSkill(Iter.Value, EWeaponSocket::kMain);
						return;
					}
				}
				break;
				case ESkillType::kWeaponActive:
				{
					auto GASPtr = CharacterPtr->GetAbilitySystemComponent();
					auto GameplayAbilitySpecPtr = GASPtr->FindAbilitySpecFromHandle(Iter.Value.Handle);
					if (!GameplayAbilitySpecPtr)
					{
						return;
					}
					auto GAInsPtr = Cast<USkill_Base>(GameplayAbilitySpecPtr->GetPrimaryInstance());
					if (!GAInsPtr)
					{
						return;
					}

					auto bIsReady = GAInsPtr->CanActivateAbility(GAInsPtr->GetCurrentAbilitySpecHandle(), GAInsPtr->GetCurrentActorInfo());
					if (bIsReady)
					{
						bIsReleasingSkill = true;
						CurrentActivedSkill = Iter.Value;

						OnOnGameplayAbilityEndedHandle = GAInsPtr->OnGameplayAbilityEnded.AddUObject(this, &ThisClass::OnOnGameplayAbilityEnded);

						CharacterPtr->GetEquipmentItemsComponent()->ActiveSkill(Iter.Value, EWeaponSocket::kMain);
						return;
					}
				}
				break;
				}
			}
		}
	}
}

void UAITask_ReleaseSkill::OnOnGameplayAbilityEnded(UGameplayAbility* GAPtr)
{
	bIsReleasingSkill = false;

	EndTask();
}

void UAITask_ReleaseSkill::OnDestroy(bool bInOwnerFinished)
{
	// 1.如果不取消这个回调，CancelSkill会调用无效的成员函数（UE判断过了 不会崩溃 但是逻辑不对）
	auto GASPtr = CharacterPtr->GetAbilitySystemComponent();
	auto GameplayAbilitySpecPtr = GASPtr->FindAbilitySpecFromHandle(CurrentActivedSkill.Handle);
	if (!GameplayAbilitySpecPtr)
	{
		return;
	}
	auto GAInsPtr = Cast<USkill_Base>(GameplayAbilitySpecPtr->GetPrimaryInstance());
	if (!GAInsPtr)
	{
		return;
	}

	GAInsPtr->OnGameplayAbilityEnded.Remove(OnOnGameplayAbilityEndedHandle);

	// 2.
	CharacterPtr->GetEquipmentItemsComponent()->CancelSkill(CurrentActivedSkill);

	Super::OnDestroy(bInOwnerFinished);
}
