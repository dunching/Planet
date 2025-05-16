#include "Skill_Active_BYWD.h"

#include "Net/UnrealNetwork.h"

#include "AbilityTask_ApplyRootMotion_FlyAway.h"
#include "AbilityTask_PlayMontage.h"

#include "GameFramework/RootMotionSource.h"

#include "CharacterBase.h"
#include "CharacterStateInfo.h"
#include "StateProcessorComponent.h"
#include "AbilityTask_TimerHelper.h"
#include "AssetRefMap.h"
#include "CharacterAbilitySystemComponent.h"
#include "CharacterAttributesComponent.h"
#include "CollisionDataStruct.h"
#include "GameplayTagsLibrary.h"
#include "GroupManagger.h"
#include "HumanCharacter.h"
#include "Planet_Tools.h"
#include "TeamMatesHelperComponent.h"
#include "VectorTypes.h"

void USkill_Active_BYWD::OnAvatarSet(
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

void USkill_Active_BYWD::PerformAction(
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
		// CharacterStateInfoSPtr = MakeShared<FCharacterStateInfo>();
		// CharacterStateInfoSPtr->Tag = SkillProxyPtr->GetProxyType();
		// CharacterStateInfoSPtr->Duration = Duration;
		// CharacterStateInfoSPtr->DefaultIcon = SkillProxyPtr->GetIcon();
		// CharacterStateInfoSPtr->DataChanged();
		//
		// CharacterPtr->GetStateProcessorComponent()->AddStateDisplay(CharacterStateInfoSPtr);

		{
			auto TaskPtr = UAbilityTask_TimerHelper::DelayTask(this);
			TaskPtr->SetDuration(
			                     ItemProxy_DescriptionPtr->Duration.PerLevelValue[0],
			                     ItemProxy_DescriptionPtr->DamageFrequency.PerLevelValue[0]
			                    );
			TaskPtr->IntervalDelegate.BindUObject(this, &ThisClass::IntervalDelegate);
			TaskPtr->DurationDelegate.BindUObject(this, &ThisClass::OnDuration);
			TaskPtr->ReadyForActivation();
		}
	}
#endif

	PlayMontage();

	if (GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() > ROLE_SimulatedProxy)
	{
		{
			auto TaskPtr = UAbilityTask_ApplyRootMotion_FlyAway::NewTask(
			                                                             this,
			                                                             TEXT(""),
			                                                             ERootMotionAccumulateMode::Additive,
			                                                             ItemProxy_DescriptionPtr->Duration.
			                                                             PerLevelValue[0],
			                                                             ItemProxy_DescriptionPtr->Height.PerLevelValue[
				                                                             0],
			                                                             ItemProxy_DescriptionPtr->ResingSpeed,
			                                                             ItemProxy_DescriptionPtr->FallingSpeed
			                                                            );

#if UE_EDITOR || UE_SERVER
			if (GetAbilitySystemComponentFromActorInfo()->GetNetMode() == NM_DedicatedServer)
			{
				TaskPtr->OnFinished.BindLambda(
				                               [this]
				                               {
					                               K2_CancelAbility();
				                               }
				                              );
			}
#endif

			TaskPtr->ReadyForActivation();
		}
	}
}

void USkill_Active_BYWD::EndAbility(
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

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USkill_Active_BYWD::ApplyCooldown(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo
	) const
{
	Super::ApplyCooldown(Handle, ActorInfo, ActivationInfo);

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

void USkill_Active_BYWD::ApplyCost(
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

float USkill_Active_BYWD::GetRemainTime() const
{
	return RemainTime;
}

void USkill_Active_BYWD::PlayMontage()
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
			 ItemProxy_DescriptionPtr->HumanMontageRef.LoadSynchronous(),
			 InPlayRate
			);

		TaskPtr->Ability = this;
		TaskPtr->SetAbilitySystemComponent(GetAbilitySystemComponentFromActorInfo());

		TaskPtr->ReadyForActivation();
	}
}

void USkill_Active_BYWD::IntervalDelegate(
	UAbilityTask_TimerHelper*,
	float CurrentIntervalTime,
	float IntervalTime
	)
{
	if (CurrentIntervalTime < IntervalTime)
	{
		return;
	}

#if UE_EDITOR || UE_SERVER
	if (GetAbilitySystemComponentFromActorInfo()->GetNetMode() == NM_DedicatedServer)
	{
		MakeDamage();
	}
#endif
}

void USkill_Active_BYWD::OnDuration(
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

void USkill_Active_BYWD::MakeDamage()
{
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(Pawn_Object);

	FCollisionQueryParams CapsuleParams;
	CapsuleParams.AddIgnoredActor(CharacterPtr);

	auto GroupMnaggerComponent = Cast<AHumanCharacter>(CharacterPtr)->GetGroupManagger();
	if (GroupMnaggerComponent)
	{
		auto TeamsHelperSPtr = GroupMnaggerComponent->GetTeamMatesHelperComponent();
		if (TeamsHelperSPtr)
		{
			for (auto Iter : TeamsHelperSPtr->GetMembersSet())
			{
				CapsuleParams.AddIgnoredActor(Iter->GetCharacterActor().Get());
			}
		}
	}

	FCollisionShape CollisionShape = FCollisionShape::MakeSphere(
	                                                             ItemProxy_DescriptionPtr->DamageRadius.PerLevelValue[
		                                                             0]
	                                                            );

	TArray<struct FHitResult> OutHits;
	if (GetWorldImp()->SweepMultiByObjectType(
	                                          OutHits,
	                                          CharacterPtr->GetActorLocation(),
	                                          CharacterPtr->GetActorLocation() + (FVector::DownVector *
		                                          (ItemProxy_DescriptionPtr->Height.PerLevelValue[0] +
		                                           ItemProxy_DescriptionPtr->SweepOffset)),
	                                          // 
	                                          FQuat::Identity,
	                                          ObjectQueryParams,
	                                          CollisionShape,
	                                          CapsuleParams
	                                         ))
	{
		const auto& CharacterAttributes = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
		const int32 BaseDamage = ItemProxy_DescriptionPtr->DamageBase.PerLevelValue[
			0];

		TSet<ACharacterBase*> TargetCharacterSet;
		for (auto Iter : OutHits)
		{
			auto TargetCharacterPtr = Cast<ACharacterBase>(Iter.GetActor());
			if (TargetCharacterPtr)
			{
				if (CharacterPtr->IsGroupmate(TargetCharacterPtr))
				{
					continue;
				}
				if (TargetCharacterPtr->GetCharacterAbilitySystemComponent()->IsCantBeDamage())
				{
					continue;
				}
				TargetCharacterSet.Add(TargetCharacterPtr);
			}
		}

		FGameplayEffectSpecHandle SpecHandle =
			MakeOutgoingGameplayEffectSpec(UAssetRefMap::GetInstance()->OnceGEClass, GetAbilityLevel());
		SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyType_BaseValue_Addtive);
		SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_Damage);
		SpecHandle.Data.Get()->AddDynamicAssetTag(SkillProxyPtr->GetProxyType());

		SpecHandle.Data.Get()->SetSetByCallerMagnitude(
		                                               UGameplayTagsLibrary::GEData_ModifyItem_Damage_Metal,
		                                               BaseDamage
		                                              );

		TArray<TWeakObjectPtr<AActor>> Ary;
		for (auto Iter : TargetCharacterSet)
		{
			Ary.Add(Iter);
		}
		FGameplayAbilityTargetDataHandle TargetData;

		auto GameplayAbilityTargetData_ActorArrayPtr = new FGameplayAbilityTargetData_ActorArray;
		GameplayAbilityTargetData_ActorArrayPtr->SetActors(Ary);

		TargetData.Add(GameplayAbilityTargetData_ActorArrayPtr);
		const auto GEHandleAry = MyApplyGameplayEffectSpecToTarget(
		                                                           GetCurrentAbilitySpecHandle(),
		                                                           GetCurrentActorInfo(),
		                                                           GetCurrentActivationInfo(),
		                                                           SpecHandle,
		                                                           TargetData
		                                                          );
	}
}
