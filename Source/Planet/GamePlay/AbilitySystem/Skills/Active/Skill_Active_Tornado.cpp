#include "Skill_Active_Tornado.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "NiagaraComponent.h"
#include "Components/PrimitiveComponent.h"
#include "UObject/Class.h"
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_Repeat.h"
#include "Components/SplineComponent.h"
#include <Components/CapsuleComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include "Kismet/KismetMathLibrary.h"
#include <Engine/OverlapResult.h>


#include "CharacterBase.h"
#include "ProxyProcessComponent.h"
#include "Tool_PickAxe.h"
#include "AbilityTask_PlayMontage.h"
#include "ToolFuture_PickAxe.h"
#include "Planet.h"
#include "CollisionDataStruct.h"
#include "AbilityTask_ApplyRootMotionBySPline.h"
#include "SPlineActor.h"
#include "AbilityTask_TimerHelper.h"
#include "Helper_RootMotionSource.h"
#include "Tornado.h"
#include "CharacterAbilitySystemComponent.h"
#include "GameplayTagsLibrary.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetStringLibrary.h"

struct FItemDecription_Skill_Active_Tornado : public TStructVariable<FItemDecription_Skill_Active_Tornado>
{
#pragma region UISocket
	const FName Title = TEXT("Title");

	const FName Text = TEXT("Text");

	const FName CD = TEXT("CD");

	const FName Cost = TEXT("Cost");
#pragma endregion

#pragma region UIDescription
	const FString Duration = TEXT("[Duration]");

	const FString Damage = TEXT("[Damage]");
#pragma endregion
};

void UItemDecription_Skill_Active_Tornado::SetUIStyle()
{
	if (!ProxySPtr)
	{
		return;
	}
	{
		auto WidgetPtr = Cast<UTextBlock>(GetWidgetFromName(FItemDecription_Skill_Active_Tornado::Get().Title));
		if (WidgetPtr)
		{
			WidgetPtr->SetText(FText::FromString(ProxySPtr->GetProxyName()));
		}
	}
	auto ItemProxy_DescriptionPtr = Cast<FItemProxy_DescriptionType>(ItemProxy_Description.LoadSynchronous());
	if (!ItemProxy_DescriptionPtr)
	{
		return;
	}
	{
		auto WidgetPtr = Cast<URichTextBlock>(GetWidgetFromName(FItemDecription_Skill_Active_Tornado::Get().CD));
		if (WidgetPtr)
		{
			WidgetPtr->SetText(
				FText::FromString(UKismetStringLibrary::Conv_IntToString(ItemProxy_DescriptionPtr->CD.PerLevelValue[0]))
			);
		}
	}
	{
		auto WidgetPtr = Cast<URichTextBlock>(GetWidgetFromName(FItemDecription_Skill_Active_Tornado::Get().Cost));
		if (WidgetPtr)
		{
			WidgetPtr->SetText(
				FText::FromString(
					UKismetStringLibrary::Conv_IntToString(ItemProxy_DescriptionPtr->Cost.PerLevelValue[0])
				)
			);
		}
	}
	if (!ItemProxy_DescriptionPtr->DecriptionText.IsEmpty())
	{
		FString Text = ItemProxy_DescriptionPtr->DecriptionText[0];

		Text = Text.Replace(
			*FItemDecription_Skill_Active_Tornado::Get().Duration,
			*UKismetStringLibrary::Conv_IntToString(ItemProxy_DescriptionPtr->Duration.PerLevelValue[0])
		);

		auto WidgetPtr = Cast<URichTextBlock>(GetWidgetFromName(FItemDecription_Skill_Active_Tornado::Get().Text));
		if (WidgetPtr)
		{
			WidgetPtr->SetText(FText::FromString(Text));
		}
	}
}

USkill_Active_Tornado::USkill_Active_Tornado() :
                                               Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void USkill_Active_Tornado::OnAvatarSet(
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

void USkill_Active_Tornado::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	const FGameplayEventData* TriggerEventData /*= nullptr */
)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
}

void USkill_Active_Tornado::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

bool USkill_Active_Tornado::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags /*= nullptr*/,
	const FGameplayTagContainer* TargetTags /*= nullptr*/,
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
) const
{
	if (Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return true;
	}
	return false;
}

void USkill_Active_Tornado::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool USkill_Active_Tornado::CommitAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FGameplayTagContainer* OptionalRelevantTags
)
{
	return Super::CommitAbility(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags);
}

void USkill_Active_Tornado::ApplyCooldown(
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

void USkill_Active_Tornado::ApplyCost(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo
) const
{
	UGameplayEffect* CostGE = GetCostGameplayEffect();
	if (CostGE)
	{
		FGameplayEffectSpecHandle SpecHandle =
			MakeOutgoingGameplayEffectSpec(CostGE->GetClass(), GetAbilityLevel());
		SpecHandle.Data.Get()->AddDynamicAssetTag(SkillProxyPtr->GetProxyType());
		SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyType_BaseValue_Addtive);
		SpecHandle.Data.Get()->SetSetByCallerMagnitude(
			UGameplayTagsLibrary::GEData_ModifyItem_Mana,
			-ItemProxy_DescriptionPtr->Cost.PerLevelValue[0]
		);

		const auto CDGEHandle = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
}

void USkill_Active_Tornado::OnGameplayTaskActivated(
	UGameplayTask& Task
)
{
	Super::OnGameplayTaskActivated(Task);
}

void USkill_Active_Tornado::PerformAction(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	if (CharacterPtr)
	{
#if UE_EDITOR || UE_SERVER
		if (GetAbilitySystemComponentFromActorInfo()->GetNetMode() == NM_DedicatedServer)
		{
			const auto Dir = UKismetMathLibrary::MakeRotFromZX(
				-CharacterPtr->GetGravityDirection(),
				CharacterPtr->GetControlRotation().Vector()
			);

			if (CharacterPtr->GetCharacterMovement()->CurrentFloor.IsWalkableFloor())
			{
				const auto Location = CharacterPtr->GetCharacterMovement()->CurrentFloor.HitResult.ImpactPoint;
				FActorSpawnParameters SpawnParameters;
				SpawnParameters.CustomPreSpawnInitalization = [this, &Dir](
					auto ActorPtr
				)
					{
						Cast<ATornado>(ActorPtr)->SetData(
							ItemProxy_DescriptionPtr,
							SkillProxyPtr,
							Dir.Vector()
						);
					};

				SpawnParameters.Owner = CharacterPtr;

				auto TornadoPtr = GetWorld()->SpawnActor<ATornado>(
					TornadoClass,
					Location,
					FRotator::ZeroRotator,
					SpawnParameters
				);
			}
		}
#endif
		if (
			(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority) ||
			(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_AutonomousProxy)
		)
		{
		}

		ExcuteTasks();
		PlayMontage();

#if UE_EDITOR || UE_SERVER
		if (GetAbilitySystemComponentFromActorInfo()->GetNetMode() == NM_DedicatedServer)
		{
			CommitAbility(Handle, ActorInfo, ActivationInfo);
		}
#endif
	}
}

void USkill_Active_Tornado::ExcuteTasks()
{
}

void USkill_Active_Tornado::PlayMontage()
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
			HumanMontage,
			InPlayRate
		);

		TaskPtr->Ability = this;
		TaskPtr->SetAbilitySystemComponent(CharacterPtr->GetCharacterAbilitySystemComponent());
		TaskPtr->OnCompleted.BindUObject(this, &ThisClass::OnPlayMontageEnd);
		TaskPtr->OnInterrupted.BindUObject(this, &ThisClass::OnPlayMontageEnd);

		TaskPtr->ReadyForActivation();
	}
}

void USkill_Active_Tornado::OnPlayMontageEnd()
{
#if UE_EDITOR || UE_SERVER
	if (GetAbilitySystemComponentFromActorInfo()->GetNetMode() == NM_DedicatedServer)
	{
		K2_CancelAbility();
	}
#endif
}

void USkill_Active_Tornado::OnTimerHelperTick(
	UAbilityTask_TimerHelper* TaskPtr,
	float CurrentInterval,
	float Interval
)
{
}
