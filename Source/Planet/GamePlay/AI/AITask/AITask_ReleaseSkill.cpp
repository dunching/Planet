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
		for (const auto& iter : SkillsMap)
		{
			if (iter.Value.SkillUnit)
			{
				switch (iter.Value.SkillUnit->SkillType)
				{
				case ESkillType::kActive:
				{
					auto GASPtr = CharacterPtr->GetAbilitySystemComponent();
					auto GameplayAbilitySpecPtr = GASPtr->FindAbilitySpecFromHandle(iter.Value.Handle);
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

						GAInsPtr->OnGameplayAbilityEnded.AddUObject(this, &ThisClass::OnOnGameplayAbilityEnded);

						CharacterPtr->GetEquipmentItemsComponent()->ActiveSkill(iter.Value, EWeaponSocket::kMain);
						return;
					}
				}
				break;
				case ESkillType::kWeaponActive:
				{
					auto GASPtr = CharacterPtr->GetAbilitySystemComponent();
					auto GameplayAbilitySpecPtr = GASPtr->FindAbilitySpecFromHandle(iter.Value.Handle);
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

						GAInsPtr->OnGameplayAbilityEnded.AddUObject(this, &ThisClass::OnOnGameplayAbilityEnded);

						CharacterPtr->GetEquipmentItemsComponent()->ActiveSkill(iter.Value, EWeaponSocket::kMain);
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
	Super::OnDestroy(bInOwnerFinished);
}
