#include "Skill_Active_Traction.h"

#include <Engine/OverlapResult.h>
#include "Components/CapsuleComponent.h"

#include "AbilityTask_ARM_RadialForce.h"
#include "CharacterBase.h"
#include "AbilityTask_TimerHelper.h"
#include "GameplayTagsLibrary.h"
#include "CharacterAbilitySystemComponent.h"
#include "CameraTrailHelper.h"
#include "AbilityTask_ControlCameraBySpline.h"
#include "CharacterAttibutes.h"
#include "CharacterAttributesComponent.h"
#include "KismetGravityLibrary.h"
#include "SPlineActor.h"
#include "CharacterStateInfo.h"
#include "StateProcessorComponent.h"
#include "AbilityTask_PlayMontage.h"
#include "TractionActor.h"

void UItemDecription_Skill_Active_Traction::SetUIStyle()
{
}

void USkill_Active_Traction::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (SkillProxyPtr)
	{
		ItemProxy_DescriptionPtr = Cast<FItemProxy_DescriptionType>(
			DynamicCastSharedPtr<FActiveSkillProxy>(SkillProxyPtr)->GetTableRowProxy_ActiveSkillExtendInfo()
		);
	}
}

void USkill_Active_Traction::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
#if UE_EDITOR || UE_SERVER
	if (GetAbilitySystemComponentFromActorInfo()->GetNetMode() == NM_DedicatedServer)
	{
		CommitAbility(Handle, ActorInfo, ActivationInfo);
	}
#endif

	if (TractionPoint)
	{
		TractionPoint->Destroy();
	}

	TractionPoint = nullptr;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USkill_Active_Traction::ApplyCooldown(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo
) const
{
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (CooldownGE)
	{
		FGameplayEffectSpecHandle SpecHandle =
			MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), GetAbilityLevel());
		SpecHandle.Data.Get()->AddDynamicAssetTag(SkillProxyPtr->GetProxyType());
		SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_CD);
		SpecHandle.Data.Get()->SetSetByCallerMagnitude(
			UGameplayTagsLibrary::GEData_Duration,
			ItemProxy_DescriptionPtr->CD.PerLevelValue[0]
		);

		const auto CDGEHandle = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
}

void USkill_Active_Traction::ApplyCost(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo
) const
{
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (CooldownGE)
	{
		FGameplayEffectSpecHandle SpecHandle =
			MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), GetAbilityLevel());
		SpecHandle.Data.Get()->AddDynamicAssetTag(SkillProxyPtr->GetProxyType());
		SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyType_BaseValue_Addtive);
		SpecHandle.Data.Get()->SetSetByCallerMagnitude(
			UGameplayTagsLibrary::GEData_ModifyItem_Mana,
			-ItemProxy_DescriptionPtr->Cost.PerLevelValue[0]
		);

		const auto CDGEHandle = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
}

void USkill_Active_Traction::PerformAction(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::PerformAction(Handle, ActorInfo, ActivationInfo, TriggerEventData);

#if UE_EDITOR || UE_SERVER
	if (GetAbilitySystemComponentFromActorInfo()->GetNetMode() == NM_DedicatedServer)
	{
		FActorSpawnParameters SpawnParameters;

		SpawnParameters.CustomPreSpawnInitalization = [this](
			auto ActorPtr
		)
			{
				Cast<ATractionPoint>(ActorPtr)->ItemProxy_DescriptionPtr = ItemProxy_DescriptionPtr;
			};

		SpawnParameters.Owner = CharacterPtr;

		auto TractionPointPtr = GetWorld()->SpawnActor<ATractionPoint>(
			CharacterPtr->GetActorLocation(),
			FRotator::ZeroRotator,
			SpawnParameters
		);
	}
#endif

	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority) ||
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_AutonomousProxy)
	)
	{
		auto DelayTaskPtr = UAbilityTask_TimerHelper::DelayTask(this);
		DelayTaskPtr->SetDuration(ItemProxy_DescriptionPtr->Duration.PerLevelValue[0]);
		DelayTaskPtr->DurationDelegate.BindUObject(this, &ThisClass::OnDuration);

		if (GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority)
		{
			DelayTaskPtr->OnFinished.BindLambda(
				[this](
				auto
			)
				{
					K2_CancelAbility();

					return true;
				}
			);
		}
		DelayTaskPtr->ReadyForActivation();
	}

	PlayMontage();
}

float USkill_Active_Traction::GetRemainTime() const
{
	return RemainTime;
}

void USkill_Active_Traction::PlayMontage()
{
	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority) ||
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_AutonomousProxy)
	)
	{
		const float InPlayRate = 1.f;

		auto TaskPtr = UAbilityTask_ASCPlayMontage::CreatePlayMontageAndWaitProxy(
			this,
			TEXT(""),
			HumanMontagePtr,
			InPlayRate,
			StartSection
		);

		TaskPtr->Ability = this;
		TaskPtr->SetAbilitySystemComponent(CharacterPtr->GetCharacterAbilitySystemComponent());

		TaskPtr->ReadyForActivation();
	}
}

void USkill_Active_Traction::OnDuration(
	UAbilityTask_TimerHelper*,
	float CurrentTiem,
	float TotalTime
)
{
	if (FMath::IsNearlyZero(TotalTime))
	{
		return;
	}
	
	RemainTime = (TotalTime - CurrentTiem) / TotalTime;
	if (RemainTime < 0.f)
	{
		RemainTime = 0.f;
	}
}
