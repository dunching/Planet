#include "Skill_Base.h"

#include "Engine/OverlapResult.h"
#include "Kismet/KismetMathLibrary.h"

#include "ItemProxy_Minimal.h"
#include "CharacterBase.h"
#include "AbilityTask_TimerHelper.h"
#include "AssetRefMap.h"
#include "AS_Character.h"
#include "PlanetWorldSettings.h"
#include "CharacterAbilitySystemComponent.h"
#include "CollisionDataStruct.h"
#include "GameplayTagsLibrary.h"
#include "Weapon_Base.h"
#include "InventoryComponent.h"
#include "KismetGravityLibrary.h"
#include "ModifyItemProxyStrategy.h"
#include "PlanetPlayerController.h"
#include "StateProcessorComponent.h"

UScriptStruct* FGameplayAbilityTargetData_RegisterParam_SkillBase::GetScriptStruct() const
{
	return FGameplayAbilityTargetData_RegisterParam_SkillBase::StaticStruct();
}

bool FGameplayAbilityTargetData_RegisterParam_SkillBase::NetSerialize(
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

FGameplayAbilityTargetData_RegisterParam_SkillBase* FGameplayAbilityTargetData_RegisterParam_SkillBase::Clone() const
{
	auto ResultPtr =
		new FGameplayAbilityTargetData_RegisterParam_SkillBase;

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
			                              FindProxy<FModifyItemProxyStrategy_Skill>(GameplayAbilityTargetPtr->ProxyID);
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

TArray<TObjectPtr<ACharacterBase>> USkill_Base::GetTargetsInDistanceByNearestCharacter(
	int32 MaxDistance,
	int32 UpForwardDistance,
	int32 DownDistance
	) const
{
	TArray<TObjectPtr<ACharacterBase>> Result;

	const auto TargetsAry = GetTargetsInDistance(MaxDistance, UpForwardDistance, DownDistance);

	const auto CharacterLocation = CharacterPtr->GetActorLocation();

	std::map<float, TObjectPtr<ACharacterBase>> Map;

	for (const auto& Iter : TargetsAry)
	{
		const auto Distance = FVector::Distance(Iter->GetActorLocation(), CharacterLocation);
		Map.emplace(Distance, Iter);
	}

	for (const auto& Iter : Map)
	{
		Result.Add(Iter.second);
	}

	return Result;
}

TArray<TObjectPtr<ACharacterBase>> USkill_Base::GetTargetsInDistanceByNearestCharacterViewDirection(
	int32 MaxDistance,
	int32 UpForwardDistance,
	int32 DownDistance
	) const
{
	TArray<TObjectPtr<ACharacterBase>> Result;

	const auto TargetsAry = GetTargetsInDistance(MaxDistance, UpForwardDistance, DownDistance);

	const auto CharacterLocation = CharacterPtr->GetActorLocation();

	// 初始角度
	const auto OriginRot = UKismetMathLibrary::MakeRotFromZX(
	                                                         -UKismetGravityLibrary::GetGravity(),
	                                                         CharacterPtr->GetActorForwardVector()
	                                                        );

	const auto Dir = OriginRot.Vector();

	std::map<float, TObjectPtr<ACharacterBase>, std::greater<>> Map;

	for (const auto& Iter : TargetsAry)
	{
		const auto TargetDir = (Iter->GetActorLocation() - CharacterLocation).
			GetSafeNormal();
		const auto DotProduct = FVector::DotProduct(TargetDir, Dir);
		Map.emplace(DotProduct, Iter);
	}

	for (const auto& Iter : Map)
	{
		Result.Add(Iter.second);
	}

	return Result;
}

FGameplayEffectSpecHandle USkill_Base::MakeDamageToTargetSpecHandle(
	EElementalType ElementalType,
	int32 Elemental_Damage,
	float Elemental_Damage_Magnification
	) const
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
		{
			const int32 BaseDamage = Elemental_Damage + (
				                         CharacterAttributes->GetWoodValue() * Elemental_Damage_Magnification);
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(
			                                               UGameplayTagsLibrary::GEData_ModifyItem_Damage_Wood,
			                                               BaseDamage
			                                              );
		}
		break;
	case EElementalType::kWater:
		{
			const int32 BaseDamage = Elemental_Damage + (
				                         CharacterAttributes->GetWaterValue() * Elemental_Damage_Magnification);
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(
			                                               UGameplayTagsLibrary::GEData_ModifyItem_Damage_Water,
			                                               BaseDamage
			                                              );
		}
		break;
	case EElementalType::kFire:
		{
			const int32 BaseDamage = Elemental_Damage + (
				                         CharacterAttributes->GetFireValue() * Elemental_Damage_Magnification);
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(
			                                               UGameplayTagsLibrary::GEData_ModifyItem_Damage_Fire,
			                                               BaseDamage
			                                              );
		}
		break;
	case EElementalType::kEarth:
		{
			const int32 BaseDamage = Elemental_Damage + (
				                         CharacterAttributes->GetEarthValue() * Elemental_Damage_Magnification);
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(
			                                               UGameplayTagsLibrary::GEData_ModifyItem_Damage_Earth,
			                                               BaseDamage
			                                              );
		}
		break;
	}

	return SpecHandle;
}

void USkill_Base::ApplyCostImp(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const TMap<FGameplayTag, int32>& CostMap
	) const
{
	auto AbilitySystemComponentPtr = Cast<UCharacterAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());
	if (!AbilitySystemComponentPtr)
	{
		return;
	}

	UGameplayEffect* CostGE = GetCostGameplayEffect();
	if (CostGE)
	{
		FGameplayEffectSpecHandle SpecHandle =
			MakeOutgoingGameplayEffectSpec(CostGE->GetClass(), GetAbilityLevel());
		SpecHandle.Data.Get()->AddDynamicAssetTag(SkillProxyPtr->GetProxyType());
		SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyType_BaseValue_Addtive);

		const auto CostsMap = AbilitySystemComponentPtr->GetCost(CostMap);
		for (const auto& Iter : CostsMap)
		{
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(
			                                               Iter.Key,
			                                               -Iter.Value
			                                              );
		}
		const auto CDGEHandle = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
}

TSet<TObjectPtr<ACharacterBase>> USkill_Base::GetTargetsInDistance(
	int32 MaxDistance,
	int32 UpForwardDistance,
	int32 DownDistance
	) const
{
	TSet<TObjectPtr<ACharacterBase>> Result;

	if (auto FocusCharacterPtr = HasFocusActor())
	{
		const auto CurrentDistance = FVector::Distance(
		                                               FocusCharacterPtr->GetActorLocation(),
		                                               CharacterPtr->GetActorLocation()
		                                              );
		if (CurrentDistance < MaxDistance)
		{
			Result.Add(FocusCharacterPtr);
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
				Result.Add(Iter.Get());
			}
		}
	}

	TArray<struct FHitResult> OutOverlaps;
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(Pawn_Object);

	FCollisionQueryParams Params;

	Params.AddIgnoredActor(CharacterPtr);

	if (GetWorld()->SweepMultiByObjectType(
	                                       OutOverlaps,
	                                       CharacterPtr->GetActorLocation() + (FVector::UpVector * UpForwardDistance),
	                                       CharacterPtr->GetActorLocation() + (FVector::DownVector * DownDistance),
	                                       FQuat::Identity,
	                                       ObjectQueryParams,
	                                       FCollisionShape::MakeSphere(MaxDistance),
	                                       Params
	                                      ))
	{
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

			Result.Add(TargetCharacterPtr);
		}
	}

	return Result;
}
