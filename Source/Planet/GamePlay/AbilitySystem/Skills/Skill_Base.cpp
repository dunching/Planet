#include "Skill_Base.h"

#include "Engine/OverlapResult.h"
#include "Kismet/KismetMathLibrary.h"

#include "ItemProxy_Minimal.h"
#include "CharacterBase.h"
#include "AbilityTask_TimerHelper.h"
#include "AssetRefMap.h"
#include "AS_Character.h"
#include "PlanetWorldSettings.h"
#include "ProxyProcessComponent.h"
#include "CharacterAbilitySystemComponent.h"
#include "CollisionDataStruct.h"
#include "GameplayTagsLibrary.h"
#include "Weapon_Base.h"
#include "InventoryComponent.h"
#include "KismetGravityLibrary.h"
#include "PlanetPlayerController.h"
#include "StateProcessorComponent.h"

UScriptStruct* FGameplayAbilityTargetData_SkillBase_RegisterParam::GetScriptStruct() const
{
	return FGameplayAbilityTargetData_SkillBase_RegisterParam::StaticStruct();
}

bool FGameplayAbilityTargetData_SkillBase_RegisterParam::NetSerialize(
	FArchive& Ar,
	class UPackageMap* Map,
	bool& bOutSuccess
	)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);

	if (Ar.IsSaving())
	{
	}
	else if (Ar.IsLoading())
	{
	}

	Ar << ProxyID;

	return true;
}

FGameplayAbilityTargetData_SkillBase_RegisterParam* FGameplayAbilityTargetData_SkillBase_RegisterParam::Clone() const
{
	auto ResultPtr =
		new FGameplayAbilityTargetData_SkillBase_RegisterParam;

	*ResultPtr = *this;

	return ResultPtr;
}

USkill_Base::USkill_Base() :
                           Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
}

void USkill_Base::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
	)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	// CDO
	CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());

	// 远程不能复制这个参数？
	if (Spec.GameplayEventData)
	{
		UpdateRegisterParam(*Spec.GameplayEventData);
	}
}

void USkill_Base::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	const FGameplayEventData* TriggerEventData /*= nullptr */
	)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);

	ResetPreviousStageActions();
}

void USkill_Base::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
	)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

bool USkill_Base::CommitAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
	)
{
	return Super::CommitAbility(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags);
}

bool USkill_Base::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags /*= nullptr*/,
	const FGameplayTagContainer* TargetTags /*= nullptr*/,
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
	) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void USkill_Base::CancelAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility
	)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void USkill_Base::OnRemoveAbility(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
	)
{
	Super::OnRemoveAbility(ActorInfo, Spec);
}

UGameplayEffect* USkill_Base::GetCooldownGameplayEffect() const
{
	return UAssetRefMap::GetInstance()->DurationGEClass.GetDefaultObject();
}

UGameplayEffect* USkill_Base::GetCostGameplayEffect() const
{
	return UAssetRefMap::GetInstance()->OnceGEClass.GetDefaultObject();
}

TArray<FActiveGameplayEffectHandle> USkill_Base::MyApplyGameplayEffectSpecToTarget(
	const FGameplayAbilitySpecHandle AbilityHandle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FGameplayEffectSpecHandle SpecHandle,
	const FGameplayAbilityTargetDataHandle& TargetData
	) const
{
	return ApplyGameplayEffectSpecToTarget(AbilityHandle, ActorInfo, ActivationInfo, SpecHandle, TargetData);
}

const TArray<FAbilityTriggerData>& USkill_Base::GetTriggers() const
{
	return AbilityTriggers;
}

void USkill_Base::UpdateRegisterParam(
	const FGameplayEventData& GameplayEventData
	)
{
	if (GameplayEventData.TargetData.IsValid(0))
	{
		auto GameplayAbilityTargetPtr = MakeSPtr_GameplayAbilityTargetData<FRegisterParamType>(
			 GameplayEventData.TargetData.Get(0)
			);
		if (GameplayAbilityTargetPtr)
		{
			SkillProxyPtr = CharacterPtr->GetInventoryComponent()->
			                              FindProxy_Skill(GameplayAbilityTargetPtr->ProxyID);
		}
	}
}

float USkill_Base::GetRemainTime() const
{
	return 0.f;
}

void USkill_Base::ResetPreviousStageActions()
{
	// 清除上一阶段遗留的内容
	for (int32 TaskIdx = ActiveTasks.Num() - 1; TaskIdx >= 0 && ActiveTasks.Num() > 0; --TaskIdx)
	{
		UGameplayTask* Task = ActiveTasks[TaskIdx];
		if (Task)
		{
			Task->TaskOwnerEnded();
		}
	}
	ActiveTasks.Reset();
	ResetListLock();
}

ACharacterBase* USkill_Base::HasFocusActor() const
{
	return CharacterPtr->GetFocusActor();
}

bool USkill_Base::CheckTargetInDistance(
	int32 InDistance
	) const
{
	if (CharacterPtr->IsPlayerControlled())
	{
		auto FocusCharactersAry = CharacterPtr->GetStateProcessorComponent()->GetTargetCharactersAry();
		if (FocusCharactersAry.IsValidIndex(0) && FocusCharactersAry[0].IsValid())
		{
			const auto Distance = FVector::Distance(
			                                        FocusCharactersAry[0]->GetActorLocation(),
			                                        CharacterPtr->GetActorLocation()
			                                       );
			return Distance < InDistance;
		}
	}
	else
	{
		auto TargetCharactersAry = CharacterPtr->GetStateProcessorComponent()->GetTargetCharactersAry();
		if (TargetCharactersAry.IsValidIndex(0))
		{
			const auto Distance = FVector::Distance(
			                                        TargetCharactersAry[0]->GetActorLocation(),
			                                        CharacterPtr->GetActorLocation()
			                                       );
			return Distance < InDistance;
		}
	}

	return false;
}

bool USkill_Base::CheckTargetIsEqualDistance(
	int32 InDistance
	) const
{
	const float Tolerance = 5.f;
	if (CharacterPtr->IsPlayerControlled())
	{
		auto FocusCharactersAry = CharacterPtr->GetStateProcessorComponent()->GetTargetCharactersAry();
		if (FocusCharactersAry.IsValidIndex(0))
		{
			auto TargetCharacterPtr = FocusCharactersAry[0];
			if (TargetCharacterPtr.IsValid())
			{
				const auto Distance = FVector::Distance(
				                                        TargetCharacterPtr->GetActorLocation(),
				                                        CharacterPtr->GetActorLocation()
				                                       );
				return FMath::IsNearlyEqual(Distance, InDistance, Tolerance);
			}
		}
	}
	else
	{
		auto ACPtr = CharacterPtr->GetController<AAIController>();
		auto TargetCharacterPtr = Cast<ACharacterBase>(ACPtr->GetFocusActor());
		if (TargetCharacterPtr)
		{
			const auto Distance = FVector::Distance(
			                                        TargetCharacterPtr->GetActorLocation(),
			                                        CharacterPtr->GetActorLocation()
			                                       );
			return FMath::IsNearlyEqual(Distance, InDistance, Tolerance);
		}
	}

	return false;
}

ACharacterBase* USkill_Base::GetTargetInDistance(
	int32 MaxDistance
	) const
{
	auto FocusCharacterPtr = HasFocusActor();

	if (FocusCharacterPtr)
	{
		const auto CurrentDistance = FVector::Distance(
		                                               FocusCharacterPtr->GetActorLocation(),
		                                               CharacterPtr->GetActorLocation()
		                                              );
		if (CurrentDistance < MaxDistance)
		{
			return FocusCharacterPtr;
		}
	}

	auto FocusCharactersAry = CharacterPtr->GetStateProcessorComponent()->GetTargetCharactersAry();
	for (auto Iter : FocusCharactersAry)
	{
		if (Iter.IsValid())
		{
			if (Iter->GetCharacterAbilitySystemComponent()->IsCantBeDamage())
			{
				continue;
			}
			const auto CurrentDistance = FVector::Distance(
			                                               Iter->GetActorLocation(),
			                                               CharacterPtr->GetActorLocation()
			                                              );
			if (CurrentDistance < MaxDistance)
			{
				return Iter.Get();
			}
		}
	}

	TArray<struct FOverlapResult> OutOverlaps;
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(Pawn_Object);

	FCollisionQueryParams Params;

	Params.AddIgnoredActor(CharacterPtr);

	if (GetWorld()->OverlapMultiByObjectType(
	                                         OutOverlaps,
	                                         CharacterPtr->GetActorLocation(),
	                                         FQuat::Identity,
	                                         ObjectQueryParams,
	                                         FCollisionShape::MakeSphere(MaxDistance),
	                                         Params
	                                        ))
	{
		// 初始角度
		const auto OriginRot = UKismetMathLibrary::MakeRotFromZX(
		                                                         -UKismetGravityLibrary::GetGravity(),
		                                                         CharacterPtr->GetActorForwardVector()
		                                                        );
		const auto Dir = OriginRot.Vector();

		// 身后的敌人
		float Dot = -1.f;

		for (const auto& Iter : OutOverlaps)
		{
			auto TargetCharacterPtr = Cast<ACharacterBase>(Iter.GetActor());
			if (!TargetCharacterPtr)
			{
				continue;
			}
			if (TargetCharacterPtr->IsGroupmate(CharacterPtr) || TargetCharacterPtr->
			                                                     GetCharacterAbilitySystemComponent()->IsCantBeDamage())
			{
				continue;
			}

			const auto CurrentDistance = FVector::Distance(
			                                               TargetCharacterPtr->GetActorLocation(),
			                                               CharacterPtr->GetActorLocation()
			                                              );
			if (CurrentDistance > MaxDistance)
			{
				continue;
			}

			const auto TargetDir = (TargetCharacterPtr->GetActorLocation() - CharacterPtr->GetActorLocation()).
				GetSafeNormal();
			const auto NewDot = FVector::DotProduct(TargetDir, Dir);
			if (NewDot > Dot)
			{
				Dot = NewDot;
				FocusCharacterPtr = TargetCharacterPtr;
			}
		}
	}
	return FocusCharacterPtr;
}

FGameplayEffectSpecHandle USkill_Base::MakeDamageToTarget(
	EElementalType ElementalType,
	int32 Elemental_Damage,
	float Elemental_Damage_Magnification
	)
{
	const auto& CharacterAttributes = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();

	FGameplayEffectSpecHandle SpecHandle =
		MakeOutgoingGameplayEffectSpec(UAssetRefMap::GetInstance()->OnceGEClass, GetAbilityLevel());
	SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyType_BaseValue_Addtive);
	SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_Damage);
	SpecHandle.Data.Get()->AddDynamicAssetTag(SkillProxyPtr->GetProxyType());

	switch (ElementalType)
	{
	case EElementalType::kMetal:
		{
			const int32 BaseDamage = Elemental_Damage + (
				                         CharacterAttributes->GetMetalValue() * Elemental_Damage_Magnification);
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(
			                                               UGameplayTagsLibrary::GEData_ModifyItem_Damage_Metal,
			                                               BaseDamage
			                                              );
		}
		break;
	case EElementalType::kWood:
		break;
	case EElementalType::kWater:
		break;
	case EElementalType::kFire:
		break;
	case EElementalType::kEarth:
		break;
	}

	return SpecHandle;
}
