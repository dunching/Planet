
#include "Skill_WeaponHandProtection.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "NiagaraComponent.h"
#include "Components/PrimitiveComponent.h"
#include "UObject/Class.h"
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_Repeat.h"

#include "GAEvent.h"
#include "CharacterBase.h"
#include "EquipmentElementComponent.h"
#include "AbilityTask_PlayMontage.h"
#include "ToolFuture_PickAxe.h"
#include "Planet.h"
#include "CollisionDataStruct.h"
#include "CharacterAttributesComponent.h"
#include "AbilityTask_TimerHelper.h"
#include "Weapon_HandProtection.h"
#include "UIManagerSubSystem.h"
#include "ProgressTips.h"

namespace Skill_WeaponHandProtection
{
	const FName Hit = TEXT("Hit");

	const FName AttachEnd = TEXT("AttachEnd");
}

USkill_WeaponHandProtection::USkill_WeaponHandProtection() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	bRetriggerInstancedAbility = true;
}

void USkill_WeaponHandProtection::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
	if (CharacterPtr)
	{
	}
}

void USkill_WeaponHandProtection::PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData /*= nullptr */)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);

	RepeatType = ERepeatType::kInfinte;

	if (!bIsInInputRange)
	{
		CurrentIndex = 0;
	}

	if (InputRangeHelperPtr)
	{
		InputRangeHelperPtr->RemoveFromParent();
		InputRangeHelperPtr = nullptr;
	}

	if (TriggerEventData && TriggerEventData->TargetData.IsValid(0))
	{
		CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());

		auto GameplayAbilityTargetData_DashPtr = dynamic_cast<const FGameplayAbilityTargetData_Skill_WeaponHandProtection*>(TriggerEventData->TargetData.Get(0));
		if (GameplayAbilityTargetData_DashPtr)
		{
			WeaponPtr = GameplayAbilityTargetData_DashPtr->WeaponPtr;
			if (GameplayAbilityTargetData_DashPtr->WeaponPtr)
			{
				return;
			}
		}
	}

	check(0);
	K2_EndAbility();
}

bool USkill_WeaponHandProtection::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags /*= nullptr*/, const FGameplayTagContainer* TargetTags /*= nullptr*/, OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */) const
{
	switch (RepeatType)
	{
	case ERepeatType::kStop:
	{
		if (!bIsAttackEnd)
		{
			return false;
		}
	}
	break;
	}

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void USkill_WeaponHandProtection::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	if (IsMarkPendingKillOnAbilityEnd())
	{
		if (CharacterPtr)
		{
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USkill_WeaponHandProtection::ExcuteStepsLink()
{
	switch (CurrentIndex)
	{
	case 0:
	{
		FirstStep();

		CurrentIndex++;
	}
	break;
	case 1:
	{
		SecondStep();

		CurrentIndex++;
	};
	break;
	case 2:
	default:
	{
		ThirdStep();

		CurrentIndex = 0;
	}
	break;
	}
}

void USkill_WeaponHandProtection::ExcuteStopStep()
{
	IncrementListLock();

	InputRangeHelperPtr = UUIManagerSubSystem::GetInstance()->ViewProgressTips(true);
	InputRangeHelperPtr->SetWaitTime(InputRangeInSecond);

	bIsInInputRange = true;

	auto TaskPtr = UAbilityTask_TimerHelper::DelayTask(this);

	TaskPtr->SetDuration(InputRangeInSecond);

	TaskPtr->OnFinished.BindLambda([this](UAbilityTask_TimerHelper* TaskPtr) {
		bIsInInputRange = false;
		DecrementListLockOverride();

		if (InputRangeHelperPtr)
		{
			InputRangeHelperPtr->RemoveFromParent();
			InputRangeHelperPtr = nullptr;
		}
		});

	TaskPtr->ReadyForActivation();
}

void USkill_WeaponHandProtection::FirstStep()
{
	if (WeaponPtr && CharacterPtr)
	{
		PlayMontage();
	}
}

void USkill_WeaponHandProtection::SecondStep()
{
	if (WeaponPtr && CharacterPtr)
	{
		PlayMontage();
	}
}

void USkill_WeaponHandProtection::ThirdStep()
{
	if (WeaponPtr && CharacterPtr)
	{
		PlayMontage();
	}
}

void USkill_WeaponHandProtection::OnNotifyBeginReceived(FName NotifyName)
{
	if (NotifyName == Skill_WeaponHandProtection::Hit)
	{
		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(PawnECC);

		FCollisionShape  CollisionShape = FCollisionShape::MakeCapsule(45, 90);

		FCollisionQueryParams CapsuleParams;
		CapsuleParams.AddIgnoredActor(CharacterPtr);

		TArray<struct FHitResult> OutHits;
		if (GetWorldImp()->SweepMultiByObjectType(
			OutHits,
			CharacterPtr->GetActorLocation(),
			CharacterPtr->GetActorLocation() + (CharacterPtr->GetActorForwardVector() * Distance),
			FQuat::Identity,
			ObjectQueryParams,
			CollisionShape,
			CapsuleParams
		))
		{
			FGameplayAbilityTargetData_GAEvent* GAEventData = new FGameplayAbilityTargetData_GAEvent;

			FGameplayEventData Payload;
			Payload.TargetData.Add(GAEventData);

			GAEventData->TargetActorAry.Empty();
			GAEventData->TriggerCharacterPtr = CharacterPtr;
			GAEventData->ADDamage = Damage;

			for (auto Iter : OutHits)
			{
				auto TargetCharacterPtr = Cast<ACharacterBase>(Iter.GetActor());
				if (TargetCharacterPtr)
				{
					GAEventData->TargetActorAry.Add(TargetCharacterPtr);
				}
			}

			auto SkillsAry = CharacterPtr->GetEquipmentItemsComponent()->GetSkills();
			auto GASPtr = CharacterPtr->GetAbilitySystemComponent();
			for (const auto& Iter : SkillsAry)
			{
				if (Iter.Value.SkillUnit)
				{
					switch (Iter.Value.SkillUnit->SkillType)
					{
					case ESkillType::kActive:
					{
						auto GAPtr = GASPtr->FindAbilitySpecFromHandle(Iter.Value.Handle);
						if (!GAPtr)
						{
							continue;
						}
						auto GAInstPtr = Cast<USkill_Base>(GAPtr->GetPrimaryInstance());
						if (!GAInstPtr)
						{
							continue;
						}

						GAInstPtr->AddCooldownConsumeTime(1.f);
					}
					break;
					}
				}
			}

			SendEvent(Payload);
		}
	}
	else if (NotifyName == Skill_WeaponHandProtection::AttachEnd)
	{
		bIsAttackEnd = true;
		if (RepeatType != USkill_Base::ERepeatType::kStop)
		{
			DecrementToZeroListLock();
		}
	}
}

void USkill_WeaponHandProtection::PlayMontage()
{
	const auto GAPerformSpeed = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().GAPerformSpeed.GetCurrentValue();
	const float Rate = static_cast<float>(GAPerformSpeed) / 100;

	{
		// 
	}
	{
		UAnimMontage* HumanMontage = nullptr;

		switch (CurrentIndex)
		{
		case 0:
		{
			HumanMontage = HumanStep1Montage;
		}
		break;
		case 1:
		{
			HumanMontage = HumanStep2Montage;
		}
		break;
		case 2:
		{
			HumanMontage = HumanStep3Montage;
		}
		break;
		}

		auto AbilityTask_PlayMontage_HumanPtr = UAbilityTask_PlayMontage::CreatePlayMontageAndWaitProxy(
			this,
			TEXT(""),
			HumanMontage,
			CharacterPtr->GetMesh()->GetAnimInstance(),
			Rate
		);

		AbilityTask_PlayMontage_HumanPtr->Ability = this;
		AbilityTask_PlayMontage_HumanPtr->SetAbilitySystemComponent(CharacterPtr->GetAbilitySystemComponent());
		AbilityTask_PlayMontage_HumanPtr->OnCompleted.BindUObject(this, &ThisClass::DecrementListLockOverride);
		AbilityTask_PlayMontage_HumanPtr->OnInterrupted.BindUObject(this, &ThisClass::DecrementListLockOverride);

		AbilityTask_PlayMontage_HumanPtr->OnNotifyBegin.BindUObject(this, &ThisClass::OnNotifyBeginReceived);

		AbilityTask_PlayMontage_HumanPtr->ReadyForActivation();

		IncrementListLock();
	}
}
