
#include "BasicFutures_Affected.h"

#include "CharacterBase.h"
#include "AbilityTask_PlayMontage.h"
#include "GameplayTagsSubSystem.h"
#include "Planet_Tools.h"

UBasicFutures_Affected::UBasicFutures_Affected() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	bRetriggerInstancedAbility = true;

}

void UBasicFutures_Affected::PostCDOContruct()
{
	Super::PostCDOContruct();

	if (GetWorldImp())
	{
		AbilityTags.AddTag(UGameplayTagsSubSystem::GetInstance()->Affected);

		// ���˶�ʱ������
		ActivationBlockedTags.AddTag(UGameplayTagsSubSystem::GetInstance()->RootMotion);

		// �ڡ����塱ʱ������
		ActivationBlockedTags.AddTag(UGameplayTagsSubSystem::GetInstance()->SuperArmor);

		FAbilityTriggerData AbilityTriggerData;

		AbilityTriggerData.TriggerTag = UGameplayTagsSubSystem::GetInstance()->Affected;
		AbilityTriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;

		AbilityTriggers.Add(AbilityTriggerData);
	}
}

void UBasicFutures_Affected::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
	if (CharacterPtr)
	{
	}
}

void UBasicFutures_Affected::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	const FGameplayEventData* TriggerEventData /*= nullptr */
)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);

	if (CharacterPtr)
	{
	}
}

void UBasicFutures_Affected::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

#ifdef WITH_EDITOR
#endif

	auto GameplayAbilityTargetDataPtr = dynamic_cast<const FGameplayAbilityTargetData_Affected*>(TriggerEventData->TargetData.Get(0));
	if (GameplayAbilityTargetDataPtr)
	{
		PerformAction(GameplayAbilityTargetDataPtr->AffectedDirection);
	}
}

bool UBasicFutures_Affected::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags,
	OUT FGameplayTagContainer* OptionalRelevantTags
) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UBasicFutures_Affected::PerformAction(EAffectedDirection AffectedDirection)
{
	UAnimMontage* CurMontagePtr = nullptr;

	const FRotator Rotation = CharacterPtr->Controller->GetControlRotation();
	FVector Direction = FVector::ZeroVector;

	switch (AffectedDirection)
	{
	case EAffectedDirection::kForward:
	{
		CurMontagePtr = ForwardMontage;
	}
	break;
	case EAffectedDirection::kBackward:
	{
		CurMontagePtr = BackwardMontage;
	}
	break;
	case EAffectedDirection::kLeft:
	{
		CurMontagePtr = LeftMontage;
	}
	break;
	case EAffectedDirection::kRight:
	{
		CurMontagePtr = RightMontage;
	}
	break;
	}

	const auto Rate = 1.f;
	PlayMontage(CurMontagePtr, Rate);
}

void UBasicFutures_Affected::PlayMontage(UAnimMontage* CurMontagePtr, float Rate)
{
	auto TaskPtr = UAbilityTask_ASCPlayMontage::CreatePlayMontageAndWaitProxy(
		this,
		TEXT(""),
		CurMontagePtr,
		CharacterPtr->GetMesh()->GetAnimInstance(),
		Rate
	);

	TaskPtr->Ability = this;
	TaskPtr->SetAbilitySystemComponent(CharacterPtr->GetAbilitySystemComponent());

	TaskPtr->OnCompleted.BindUObject(this, &ThisClass::K2_CancelAbility);
	TaskPtr->OnInterrupted.BindUObject(this, &ThisClass::K2_CancelAbility);

	TaskPtr->ReadyForActivation();
}