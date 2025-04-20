#include "Skill_Consumable_Generic.h"

#include "AbilitySystemComponent.h"
#include <Components/SkeletalMeshComponent.h>

#include "AbilityTask_PlayMontage.h"
#include "CharacterBase.h"
#include "AbilityTask_TimerHelper.h"

#include "Consumable_Test.h"
#include "CharacterAbilitySystemComponent.h"
#include "GameplayTagsLibrary.h"
#include "GroupManagger.h"
#include "SceneProxyTable.h"

USkill_Consumable_Generic::USkill_Consumable_Generic() :
	Super()
{
}

void USkill_Consumable_Generic::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
)
{
	Super::OnAvatarSet(ActorInfo, Spec);
}

void USkill_Consumable_Generic::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	const FGameplayEventData* TriggerEventData /*= nullptr */
)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
}

void USkill_Consumable_Generic::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

#if UE_EDITOR || UE_SERVER
	if (CharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		CommitAbility(Handle, ActorInfo, ActivationInfo);
	}
#endif
}

bool USkill_Consumable_Generic::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags /*= nullptr*/,
	const FGameplayTagContainer* TargetTags /*= nullptr*/,
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
) const
{
	if (ProxyPtr->Num <= 0)
	{
		return false;
	}

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

bool USkill_Consumable_Generic::CommitAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
)
{
#if UE_EDITOR || UE_SERVER
	if (CharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		ProxyPtr->Num--;
		ProxyPtr->Update2Client();
	}
#endif

	return Super::CommitAbility(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags);
}

void USkill_Consumable_Generic::ApplyCooldown(const FGameplayAbilitySpecHandle Handle,
                                              const FGameplayAbilityActorInfo* ActorInfo,
                                              const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (CooldownGE)
	{
		// 冷却
		{
			FGameplayEffectSpecHandle SpecHandle =
				MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), GetAbilityLevel());

			const auto CD = ProxyPtr->GetTableRowProxy_Consumable()->CD;
			SpecHandle.Data.Get()->SetDuration(CD, true);
			SpecHandle.Data.Get()->AddDynamicAssetTag(ProxyPtr->GetProxyType());
			SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_CD);
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(UGameplayTagsLibrary::GEData_Duration, CD);

			const auto CDGEHandle = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
		}

		// 公共冷却
		{
			auto AbilitySystemComponentPtr = CharacterPtr->GetGroupManagger()->GetAbilitySystemComponent();
			auto SkillCommonCooldownInfoMap = ProxyPtr->GetTableRowProxy_Consumable()->CommonCooldownInfoMap;
			for (const auto Iter : SkillCommonCooldownInfoMap)
			{
				auto CommonCooldownInfoPtr = GetTableRowProxy_CommonCooldownInfo(Iter);
				if (CommonCooldownInfoPtr)
				{
					FGameplayEffectSpecHandle SpecHandle =
						AbilitySystemComponentPtr->MakeOutgoingSpec(CooldownGE->GetClass(), GetAbilityLevel(),
						                                            AbilitySystemComponentPtr->MakeEffectContext());

					const auto CD = CommonCooldownInfoPtr->CoolDownTime;
					SpecHandle.Data.Get()->SetDuration(CD, true);
					SpecHandle.Data.Get()->AddDynamicAssetTag(Iter);
					SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_CD);
					SpecHandle.Data.Get()->SetSetByCallerMagnitude(UGameplayTagsLibrary::GEData_Duration, CD);
					AbilitySystemComponentPtr->ApplyGameplayEffectSpecToSelf(
						*SpecHandle.Data.Get(),
						AbilitySystemComponentPtr->GetPredictionKeyForNewAction()
					);
				}
			}
		}
	}
}

void USkill_Consumable_Generic::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USkill_Consumable_Generic::PerformAction(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::PerformAction(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (CharacterPtr)
	{
		SpawnActor();
		ExcuteTasks();
		PlayMontage();
	}
}

void USkill_Consumable_Generic::SpawnActor()
{
#if UE_EDITOR || UE_SERVER
	if (GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() > ROLE_SimulatedProxy)
	{
		FActorSpawnParameters ActorSpawnParameters;
		ActorSpawnParameters.Owner = CharacterPtr;
		ConsumableActorPtr = GetWorld()->SpawnActor<AConsumable_Test>(
			ProxyPtr->GetTableRowProxy_Consumable()->Consumable_Class, ActorSpawnParameters
		);

		if (ConsumableActorPtr)
		{
			ConsumableActorPtr->HasbeenInteracted(CharacterPtr);
		}
	}
#endif
}

void USkill_Consumable_Generic::ExcuteTasks()
{
#if UE_EDITOR || UE_SERVER
	if (CharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		if (CharacterPtr)
		{
			FGameplayEffectSpecHandle SpecHandle =
				MakeOutgoingGameplayEffectSpec(ConsumableGEClass, GetAbilityLevel());

			const auto Duration = ProxyPtr->GetTableRowProxy_Consumable()->Duration;
			SpecHandle.Data.Get()->SetDuration(Duration, true);
			SpecHandle.Data.Get()->AddDynamicAssetTag(ProxyPtr->GetProxyType());
			SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyType_BaseValue_Addtive);
			SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_Info);
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(UGameplayTagsLibrary::GEData_Duration, Duration);

			const auto ModifyPropertyMap = ProxyPtr->GetTableRowProxy_Consumable()->ModifyPropertyMap;
			for (const auto& Iter : ModifyPropertyMap)
			{
				SpecHandle.Data.Get()->SetSetByCallerMagnitude(
					Iter.Key,
					Iter.Value
				);
			}

			const auto CDGEHandle = ApplyGameplayEffectSpecToOwner(
				GetCurrentAbilitySpecHandle(),
				GetCurrentActorInfo(),
				GetCurrentActivationInfo(),
				SpecHandle
			);

			// auto GameplayAbilityTargetDataPtr = new FGameplayAbilityTargetData_PropertyModify(ProxyPtr);
			//
			// GameplayAbilityTargetDataPtr->TriggerCharacterPtr = CharacterPtr;
			// GameplayAbilityTargetDataPtr->TargetCharacterPtr = CharacterPtr;
			//
			// auto ICPtr = CharacterPtr->GetCharacterAbilitySystemComponent();
			// ICPtr->SendEventImp(GameplayAbilityTargetDataPtr);
		}
	}
#endif
}

void USkill_Consumable_Generic::PlayMontage()
{
#if UE_EDITOR || UE_SERVER
	if (CharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		auto HumanMontage = ProxyPtr->GetTableRowProxy_Consumable()->HumanMontage;
		if (HumanMontage)
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
#endif
}

void USkill_Consumable_Generic::OnPlayMontageEnd()
{
#if UE_EDITOR || UE_SERVER
	if (CharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		if (ConsumableActorPtr)
		{
			ConsumableActorPtr->Destroy();
			ConsumableActorPtr = nullptr;
		}

		K2_CancelAbility();
	}
#endif
}

void USkill_Consumable_Generic::EmitEffect()
{
}

void USkill_Consumable_Generic::OnGAEnd(UGameplayAbility* GAPtr)
{
}
