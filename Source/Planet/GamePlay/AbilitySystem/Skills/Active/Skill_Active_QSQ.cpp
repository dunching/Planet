#include "Skill_Active_QSQ.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Components/PrimitiveComponent.h"
#include "UObject/Class.h"
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilityTask_PlayMontage.h"
#include "AbilityTask_TimerHelper.h"
#include "CharacterAbilitySystemComponent.h"

#include "CharacterBase.h"
#include "GameplayCameraCommon.h"
#include "HumanCharacter_Player.h"
#include "Tool_PickAxe.h"
#include "ItemProxy_Skills.h"
#include "PlayerComponent.h"

struct FSkill_Active_QSQ : public TStructVariable<FSkill_Active_QSQ>
{
	const FName Hit = TEXT("Hit");

	const FName AttackEnd = TEXT("AttackEnd");

	const FName ChargeOn = TEXT("ChargeOn");

	const FName ChargeOff = TEXT("ChargeOff");
};

void USkill_Active_QSQ::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
	)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (SkillProxyPtr)
	{
		ItemProxy_DescriptionPtr = Cast<FItemProxy_DescriptionType>(
		                                                            DynamicCastSharedPtr<FActiveSkillProxy>(
			                                                             SkillProxyPtr
			                                                            )->GetTableRowProxy_ActiveSkillExtendInfo()
		                                                           );
	}
}

void USkill_Active_QSQ::ApplyCooldown(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo
	) const
{
	Super::ApplyCooldown(Handle, ActorInfo, ActivationInfo);

	ApplyCDImp(
	           Handle,
	           ActorInfo,
	           ActivationInfo,
	           SkillProxyPtr->GetProxyType(),
	           ItemProxy_DescriptionPtr->CD.PerLevelValue[0]
	          );
}

void USkill_Active_QSQ::ApplyCost(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo
	) const
{
	ApplyCostImp(Handle, ActorInfo, ActivationInfo, SkillProxyPtr->GetProxyType(), GetCostMap());
}

void USkill_Active_QSQ::PerformAction(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
	)
{
	Super::PerformAction(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AbilityTask_TimerHelperPtr = nullptr;
	ChargeTimePercent = 0.f;

	PlayMontage(
	            ItemProxy_DescriptionPtr->HumanMontage1.LoadSynchronous()
	           );
}

void USkill_Active_QSQ::OnStopContinuePerform()
{
	if (AbilityTask_TimerHelperPtr)
	{
		AbilityTask_TimerHelperPtr->SetFinished();
	}
	else
	{
	}
}

float USkill_Active_QSQ::GetRemainTime() const
{
	return ChargeTimePercent;
}

void USkill_Active_QSQ::PlayMontage(
	UAnimMontage* AnimMontagePtr
	)
{
	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority) ||
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_AutonomousProxy)
	)
	{
		const float InPlayRate = 1.f;

		AbilityTask_ASCPlayMontagePtr = UAbilityTask_ASCPlayMontage::CreatePlayMontageAndWaitProxy(
			 this,
			 TEXT(""),
			 AnimMontagePtr,
			 InPlayRate
			);

		AbilityTask_ASCPlayMontagePtr->Ability = this;
		AbilityTask_ASCPlayMontagePtr->SetAbilitySystemComponent(CharacterPtr->GetCharacterAbilitySystemComponent());
		AbilityTask_ASCPlayMontagePtr->OnCompleted.BindUObject(this, &ThisClass::OnPlayPrevMontageEnd);
		AbilityTask_ASCPlayMontagePtr->OnInterrupted.BindUObject(this, &ThisClass::OnPlayPrevMontageEnd);
		AbilityTask_ASCPlayMontagePtr->OnNotifyBegin.BindUObject(this, &ThisClass::OnNotifyBeginReceived);

		AbilityTask_ASCPlayMontagePtr->Ability = this;
		AbilityTask_ASCPlayMontagePtr->SetAbilitySystemComponent(CharacterPtr->GetCharacterAbilitySystemComponent());

		AbilityTask_ASCPlayMontagePtr->ReadyForActivation();
	}
}

void USkill_Active_QSQ::PlayMontage(
	UAnimMontage* AnimMontagePtr,
	const FName& SectionName,
	const float InPlayRate
	)
{
	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority) ||
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_AutonomousProxy)
	)
	{
		AbilityTask_ASCPlayMontagePtr->StopPlayingMontage();

		AbilityTask_ASCPlayMontagePtr = UAbilityTask_ASCPlayMontage::CreatePlayMontageAndWaitProxy(
			 this,
			 TEXT(""),
			 AnimMontagePtr,
			 InPlayRate,
			 SectionName
			);

		AbilityTask_ASCPlayMontagePtr->Ability = this;
		AbilityTask_ASCPlayMontagePtr->SetAbilitySystemComponent(CharacterPtr->GetCharacterAbilitySystemComponent());
		AbilityTask_ASCPlayMontagePtr->OnCompleted.BindUObject(this, &ThisClass::OnPlayMontageEnd);
		AbilityTask_ASCPlayMontagePtr->OnInterrupted.BindUObject(this, &ThisClass::OnPlayMontageEnd);
		AbilityTask_ASCPlayMontagePtr->OnNotifyBegin.BindUObject(this, &ThisClass::OnNotifyBeginReceived);

		AbilityTask_ASCPlayMontagePtr->Ability = this;
		AbilityTask_ASCPlayMontagePtr->SetAbilitySystemComponent(CharacterPtr->GetCharacterAbilitySystemComponent());

		AbilityTask_ASCPlayMontagePtr->ReadyForActivation();
	}
}

void USkill_Active_QSQ::OnNotifyBeginReceived(
	FName NotifyName
	)
{
	if (NotifyName == FSkill_Active_QSQ::Get().ChargeOn)
	{
		if (GetIsContinue())
		{
			if (
				(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_AutonomousProxy)
			)
			{
				auto PlayerCharacterPtr = Cast<AHumanCharacter_Player>(CharacterPtr);
				if (PlayerCharacterPtr)
				{
					PlayerCharacterPtr->GetPlayerComponent()->SetCameraType(ECameraType::kAction_WithShake);
				}
			}

			PlayMontage(
			            ItemProxy_DescriptionPtr->HumanMontage1.
			                                      LoadSynchronous(),
			            FSkill_Active_QSQ::Get().ChargeOn,
			            0.01f
			           );

			AbilityTask_TimerHelperPtr = UAbilityTask_TimerHelper::DelayTask(this);
			AbilityTask_TimerHelperPtr->SetDuration(ItemProxy_DescriptionPtr->ChargeMaxTime);
			AbilityTask_TimerHelperPtr->DurationDelegate.BindUObject(this, &ThisClass::DurationDelegate);
			AbilityTask_TimerHelperPtr->OnFinished.BindLambda(
			                                                  [this](
			                                                  auto
			                                                  )
			                                                  {
				                                                  if (
					                                                  (GetAbilitySystemComponentFromActorInfo()->
					                                                   GetOwnerRole() == ROLE_AutonomousProxy)
				                                                  )
				                                                  {
					                                                  auto PlayerCharacterPtr = Cast<
						                                                  AHumanCharacter_Player>(CharacterPtr);
					                                                  if (PlayerCharacterPtr)
					                                                  {
						                                                  PlayerCharacterPtr->GetPlayerComponent()->
							                                                  SetCameraType(ECameraType::kAction);
					                                                  }
				                                                  }

				                                                  PlayMontage(
				                                                              ItemProxy_DescriptionPtr->HumanMontage1.
				                                                              LoadSynchronous(),
				                                                              FSkill_Active_QSQ::Get().ChargeOff,
				                                                              1.f
				                                                             );
				                                                  return true;
			                                                  }
			                                                 );
			AbilityTask_TimerHelperPtr->ReadyForActivation();
		}
		else
		{
			PlayMontage(
			            ItemProxy_DescriptionPtr->HumanMontage1.
			                                      LoadSynchronous(),
			            FSkill_Active_QSQ::Get().ChargeOff,
			            1.f
			           );
		}
	}
}

void USkill_Active_QSQ::OnPlayPrevMontageEnd()
{
}

void USkill_Active_QSQ::OnPlayMontageEnd()
{
#if UE_EDITOR || UE_SERVER
	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority)
	)
	{
		K2_CancelAbility();
	}
#endif
}

void USkill_Active_QSQ::DurationDelegate(
	UAbilityTask_TimerHelper*,
	float CurrentTiem,
	float TotalTime
	)
{
	if (!GetIsContinue())
	{
		return;
	}

	if (FMath::IsNearlyZero(TotalTime))
	{
		return;
	}

	ChargeTimePercent = CurrentTiem / TotalTime;
}
