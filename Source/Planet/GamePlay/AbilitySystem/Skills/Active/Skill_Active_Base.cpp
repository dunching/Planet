
#include "Skill_Active_Base.h"

#include "SceneElement.h"
#include "CharacterBase.h"
#include "EquipmentElementComponent.h"
#include "AbilityTask_TimerHelper.h"
#include "PlanetWorldSettings.h"

void USkill_Active_Base::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());

	CooldownConsumeTime = CooldownTime - Cast<APlanetWorldSettings>(GetWorld()->GetWorldSettings())->ResetCooldownTime;
}

bool USkill_Active_Base::CommitAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
)
{
	CooldownConsumeTime = 0.f;

	return Super::CommitAbility(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags);
}

bool USkill_Active_Base::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags /*= nullptr*/,
	const FGameplayTagContainer* TargetTags /*= nullptr*/,
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
) const
{
	if (CooldownConsumeTime < CooldownTime)
	{
		return false;
	}

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void USkill_Active_Base::CancelAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility
)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void USkill_Active_Base::AddCooldownConsumeTime(float NewTime)
{
	CooldownConsumeTime += NewTime;
}

void USkill_Active_Base::Tick(float DeltaTime)
{
	CooldownConsumeTime += DeltaTime;
}

bool USkill_Active_Base::GetRemainingCooldown(
	float& RemainingCooldown, float& RemainingCooldownPercent
) const
{
	if (CooldownTime < 0.f)
	{
		const auto Remaining = CooldownTime - CooldownConsumeTime;

		if (Remaining <= 0.f)
		{
			RemainingCooldown = 0.f;

			RemainingCooldownPercent = 1.f;

			return true;
		}
		else
		{
			RemainingCooldown = Remaining;

			RemainingCooldownPercent = RemainingCooldown / Cast<APlanetWorldSettings>(GetWorld()->GetWorldSettings())->ResetCooldownTime;

			return false;
		}
	}

	const auto Remaining = CooldownTime - CooldownConsumeTime;

	if (Remaining <= 0.f)
	{
		RemainingCooldown = 0.f;

		RemainingCooldownPercent = 1.f;

		return true;
	}
	else
	{
		RemainingCooldown = Remaining;

		RemainingCooldownPercent = RemainingCooldown / CooldownTime;

		return false;
	}
}
