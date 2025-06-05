#include "PlanetAbilitySystemComponent.h"

#include "PlanetGameplayAbility.h"

FName UPlanetAbilitySystemComponent::ComponentName = TEXT("AbilitySystemComponent");

void UPlanetAbilitySystemComponent::TickComponent(
	float DeltaTime,
	enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction
)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPlanetAbilitySystemComponent::OnGiveAbility(
	FGameplayAbilitySpec& AbilitySpec
)
{
	if (GameplayEventDataMap.Find(AbilitySpec.InputID))
	{
		if (AbilitySpec.Ability)
		{
			AbilitySpec.GameplayEventData = MakeShared<FGameplayEventData>();

			*AbilitySpec.GameplayEventData = GameplayEventDataMap[AbilitySpec.InputID];
			GameplayEventDataMap.Remove(AbilitySpec.InputID);
		}
	}

	Super::OnGiveAbility(AbilitySpec);
}

void UPlanetAbilitySystemComponent::CurrentMontageStop(
	float OverrideBlendOutTime /*= -1.0f*/
)
{
	UAnimInstance* AnimInstance = AbilityActorInfo.IsValid() ? AbilityActorInfo->GetAnimInstance() : nullptr;
	UAnimMontage* MontageToStop = LocalAnimMontageInfo.AnimMontage;
	bool bShouldStopMontage = AnimInstance && MontageToStop && !AnimInstance->Montage_GetIsStopped(MontageToStop);

	if (bShouldStopMontage)
	{
		const float BlendOutTime = (OverrideBlendOutTime >= 0.0f ?
			                            OverrideBlendOutTime :
			                            MontageToStop->BlendOut.GetBlendTime());

		AnimInstance->Montage_Stop(BlendOutTime, MontageToStop);

		if (IsOwnerActorAuthoritative())
		{
			AnimMontage_UpdateReplicatedData();
		}
	}
}

UGameplayAbility* UPlanetAbilitySystemComponent::CreateNewInstanceOfAbility(
	FGameplayAbilitySpec& Spec,
	const UGameplayAbility* Ability
)
{
	check(Ability);
	check(Ability->HasAllFlags(RF_ClassDefaultObject));

	AActor* Owner = GetOwner();
	check(Owner);

	// Cast<UPlanetGameplayAbility>(Spec.Ability)->InitalDefaultTags();

	auto AbilityInstance = NewObject<UPlanetGameplayAbility>(Owner, Ability->GetClass());
	check(AbilityInstance);

	// Add it to one of our instance lists so that it doesn't GC.
	if (AbilityInstance->GetReplicationPolicy() != EGameplayAbilityReplicationPolicy::ReplicateNo)
	{
		Spec.ReplicatedInstances.Add(AbilityInstance);
		AddReplicatedInstancedAbility(AbilityInstance);
	}
	else
	{
		Spec.NonReplicatedInstances.Add(AbilityInstance);
	}

	return AbilityInstance;
}

FActiveGameplayEffectHandle UPlanetAbilitySystemComponent::ApplyGameplayEffectSpecToTarget(
	const FGameplayEffectSpec& Spec,
	UAbilitySystemComponent* Target,
	FPredictionKey PredictionKey
)
{
	return Super::ApplyGameplayEffectSpecToTarget(Spec, Target, PredictionKey);
}

void UPlanetAbilitySystemComponent::SetContinuePerform_Server_Implementation(
	FGameplayAbilitySpecHandle Handle,
	FGameplayAbilityActivationInfo ActivationInfo,
	bool bIsContinue
)
{
	if (bIsContinue)
	{
		FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(Handle);
		if (AbilitySpec && AbilitySpec->Ability && AbilitySpec->IsActive())
		{
			TArray<UGameplayAbility*> Instances = AbilitySpec->GetAbilityInstances();

			for (auto Instance : Instances)
			{
				if (Cast<UPlanetGameplayAbility>(Instance)->CanOncemorePerformAction())
				{
					ReplicatePerformAction(Handle, ActivationInfo);
				}
				else
				{
					ReplicateContinues(Handle, ActivationInfo, bIsContinue);
				}
			}
		}
	}
	else
	{
		ReplicateContinues(Handle, ActivationInfo, bIsContinue);
	}
}

void UPlanetAbilitySystemComponent::CurrentMontageStopImp_Implementation(
	float OverrideBlendOutTime
)
{
	CurrentMontageStop(OverrideBlendOutTime);
}

void UPlanetAbilitySystemComponent::AddLooseGameplayTag_2_Client_Implementation(
	const FGameplayTag& GameplayTag
)
{
	AddLooseGameplayTag(GameplayTag);
}

void UPlanetAbilitySystemComponent::RemoveLooseGameplayTag_2_Client_Implementation(
	const FGameplayTag& GameplayTag
)
{
	RemoveLooseGameplayTag(GameplayTag);
}

void UPlanetAbilitySystemComponent::ReplicateContinues_Implementation(
	FGameplayAbilitySpecHandle Handle,
	FGameplayAbilityActivationInfo ActivationInfo,
	bool bIsContinue
)
{
	FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(Handle);
	if (AbilitySpec && AbilitySpec->Ability && AbilitySpec->IsActive())
	{
		TArray<UGameplayAbility*> Instances = AbilitySpec->GetAbilityInstances();

		for (auto Instance : Instances)
		{
			Cast<UPlanetGameplayAbility>(Instance)->SetContinuePerform(bIsContinue);
		}
	}
}

void UPlanetAbilitySystemComponent::ReplicateEventData_Implementation(
	int32 InputID,
	const FGameplayEventData& GameplayEventData
)
{
	GameplayEventDataMap.Add(InputID, GameplayEventData);
}

void UPlanetAbilitySystemComponent::Replicate_UpdateGAParam_Implementation(
	FGameplayAbilitySpecHandle Handle,
	const FGameplayEventData& TriggerEventData
)
{
	FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(Handle);
	if (AbilitySpec && AbilitySpec->Ability)
	{
		if (AbilitySpec->Ability->GetInstancingPolicy() == EGameplayAbilityInstancingPolicy::InstancedPerActor)
		{
		}
		else
		{
		}
		TArray<UGameplayAbility*> Instances = AbilitySpec->GetAbilityInstances();

		for (auto Instance : Instances)
		{
			Cast<UPlanetGameplayAbility>(Instance)->UpdateRegisterParam(TriggerEventData);
		}
	}
}

void UPlanetAbilitySystemComponent::ReplicatePerformAction_Server_Implementation(
	FGameplayAbilitySpecHandle Handle,
	FGameplayAbilityActivationInfo ActivationInfo
)
{
	ReplicatePerformAction(Handle, ActivationInfo);
}

void UPlanetAbilitySystemComponent::ReplicatePerformAction_Implementation(
	FGameplayAbilitySpecHandle Handle,
	FGameplayAbilityActivationInfo ActivationInfo
)
{
	FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(Handle);
	if (AbilitySpec && AbilitySpec->Ability)
	{
		if (AbilitySpec->Ability->GetInstancingPolicy() == EGameplayAbilityInstancingPolicy::InstancedPerActor)
		{
		}
		else
		{
		}
		
		TArray<UGameplayAbility*> Instances = AbilitySpec->GetAbilityInstances();

		for (auto Instance : Instances)
		{
			Cast<UPlanetGameplayAbility>(Instance)->PerformAction(
				Handle,
				Instance->GetCurrentActorInfo(),
				Instance->GetCurrentActivationInfo(),
				nullptr
			);
		}
	}
}

bool UPlanetAbilitySystemComponent::K2_HasMatchingGameplayTag(
	FGameplayTag TagToCheck
) const
{
	return HasMatchingGameplayTag(TagToCheck);
}

bool UPlanetAbilitySystemComponent::K2_HasAnyMatchingGameplayTags(
	const FGameplayTagContainer& TagContainer
) const
{
	return HasAnyMatchingGameplayTags(TagContainer);
}

void UPlanetAbilitySystemComponent::ModifyActiveEffectDuration(
	FActiveGameplayEffectHandle Handle,
	float Duration
)
{
	FActiveGameplayEffect* Effect = ActiveGameplayEffects.GetActiveGameplayEffect(Handle);

	if (Effect)
	{
		//  bDurationLocked 为什么是true？
		// Effect->Spec.SetDuration(Duration, false);

		Effect->Spec.Duration = Duration;
		// Effect->Spec.bDurationLocked = bLockDuration;
		if (Duration > 0.f)
		{
			// We may have potential problems one day if a game is applying duration based gameplay effects from instantaneous effects
			// (E.g., every time fire damage is applied, a DOT is also applied). We may need to for Duration to always be captured.
			Effect->Spec.CapturedRelevantAttributes.AddCaptureDefinition(
				UAbilitySystemComponent::GetOutgoingDurationCapture()
			);
		}

		ActiveGameplayEffects.MarkItemDirty(*Effect);
	}
}
