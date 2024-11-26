
#include "PlanetAbilitySystemComponent.h"

#include "GameOptions.h"
#include "PlanetGameplayAbility.h"
#include "Skill_Base.h"
#include "GameplayTagsSubSystem.h"

void UPlanetAbilitySystemComponent::TickComponent(
	float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction
)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPlanetAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
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

void UPlanetAbilitySystemComponent::CurrentMontageStop(float OverrideBlendOutTime /*= -1.0f*/)
{
	UAnimInstance* AnimInstance = AbilityActorInfo.IsValid() ? AbilityActorInfo->GetAnimInstance() : nullptr;
	UAnimMontage* MontageToStop = LocalAnimMontageInfo.AnimMontage;
	bool bShouldStopMontage = AnimInstance && MontageToStop && !AnimInstance->Montage_GetIsStopped(MontageToStop);

	if (HasMatchingGameplayTag(UGameplayTagsSubSystem::State_Buff_Stagnation))
	{
		AnimInstance->Montage_Pause(MontageToStop);

		if (IsOwnerActorAuthoritative())
		{
			AnimMontage_UpdateReplicatedData();
		}
	}
	else if (bShouldStopMontage)
	{
		const float BlendOutTime = (OverrideBlendOutTime >= 0.0f ? OverrideBlendOutTime : MontageToStop->BlendOut.GetBlendTime());

		AnimInstance->Montage_Stop(BlendOutTime, MontageToStop);

		if (IsOwnerActorAuthoritative())
		{
			AnimMontage_UpdateReplicatedData();
		}
	}
}

UGameplayAbility* UPlanetAbilitySystemComponent::CreateNewInstanceOfAbility(FGameplayAbilitySpec& Spec, const UGameplayAbility* Ability)
{
	check(Ability);
	check(Ability->HasAllFlags(RF_ClassDefaultObject));

	AActor* Owner = GetOwner();
	check(Owner);

	Cast<UPlanetGameplayAbility>(Spec.Ability)->InitalDefaultTags();

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

void UPlanetAbilitySystemComponent::CurrentMontageStopImp_Implementation(float OverrideBlendOutTime)
{
	CurrentMontageStop(OverrideBlendOutTime);
}

void UPlanetAbilitySystemComponent::AddLooseGameplayTag_2_Client_Implementation(const FGameplayTag& GameplayTag)
{
	AddLooseGameplayTag(GameplayTag);
}

void UPlanetAbilitySystemComponent::RemoveLooseGameplayTag_2_Client_Implementation(const FGameplayTag& GameplayTag)
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
		if (AbilitySpec->Ability->GetInstancingPolicy() == EGameplayAbilityInstancingPolicy::NonInstanced)
		{
		}
		else
		{
			TArray<UGameplayAbility*> Instances = AbilitySpec->GetAbilityInstances();

			for (auto Instance : Instances)
			{
				Cast<UPlanetGameplayAbility>(Instance)->SetContinuePerformImp(bIsContinue);
			}
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
		if (AbilitySpec->Ability->GetInstancingPolicy() == EGameplayAbilityInstancingPolicy::NonInstanced)
		{
		}
		else
		{
			TArray<UGameplayAbility*> Instances = AbilitySpec->GetAbilityInstances();

			for (auto Instance : Instances)
			{
				Cast<USkill_Base>(Instance)->UpdateRegisterParam(TriggerEventData);
			}
		}
	}
}

bool UPlanetAbilitySystemComponent::K2_HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	return HasMatchingGameplayTag(TagToCheck);
}

bool UPlanetAbilitySystemComponent::K2_HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return HasAnyMatchingGameplayTags(TagContainer);
}
