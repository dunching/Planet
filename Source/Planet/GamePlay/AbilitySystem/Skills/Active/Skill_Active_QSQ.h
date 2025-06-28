#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystemInstanceController.h"
#include "ProjectileBase.h"
#include "SceneProxyTable.h"

#include "Skill_Active_Base.h"
#include "ScopeValue.h"

#include "Skill_Active_QSQ.generated.h"

class ACameraTrailHelper;
class UAbilityTask_TimerHelper;
class UAbilityTask_ASCPlayMontage;
class ASPlineActor;
class ANiagaraActor;

UCLASS()
class PLANET_API UItemProxy_Description_ActiveSkill_QSQ : public UItemProxy_Description_ActiveSkill
{
	GENERATED_BODY()

public:
	/**
	 * 占位符：{HumanMontage1}
	 * 
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	TSoftObjectPtr<UAnimMontage> HumanMontage1 = nullptr;

	UPROPERTY(EditAnywhere, Category="Niagara", meta = (DisplayName = "Niagara System Asset"))
	TObjectPtr<UNiagaraSystem> Asset;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	float ChargeMaxTime = 3.5f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	float MinMagnification = 1.f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	float MaxMagnification = 3.5f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 RepelDistance = 500;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 Duration = 1;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 AttackDistance = 500;

	int32 UpOffset = 500;

	int32 DownOffset = 500;

	int32 LimitAngle = 30;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<ANiagaraActor> NiagaraActorClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	EElementalType ElementalType = EElementalType::kMetal;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 Elemental_Damage = 10;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	float Elemental_Damage_Magnification = .5f;
};

/**
 * 七伤拳！！！
 */
UCLASS()
class PLANET_API USkill_Active_QSQ : public USkill_Active_Base
{
	GENERATED_BODY()

public:
	using FItemProxy_DescriptionType = UItemProxy_Description_ActiveSkill_QSQ;

	virtual void OnAvatarSet(
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilitySpec& Spec
		) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
		) override;

	virtual void ApplyCooldown(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo
		) const override;

	virtual void ApplyCost(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo
		) const override;

	virtual void PerformAction(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
		) override;

	virtual void OnStopContinuePerform() override;

	virtual float GetRemainTime() const override;

	void PlayMontage(
		UAnimMontage* AnimMontagePtr
		);

	void PlayMontage(
		UAnimMontage* AnimMontagePtr,
		const FName& SectionName,
		const float InPlayRate
		);

	UFUNCTION()
	void OnNotifyBeginReceived(
		FName NotifyName
		);

	void OnPlayPrevMontageEnd();

	void OnPlayMontageEnd();

	void DurationDelegate(
		UAbilityTask_TimerHelper*,
		float CurrentIntervalTime,
		float IntervalTime
		);

	void DoDash(
		);

	UFUNCTION()
	void OnComponentHit(
		UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit
		);

	void MakeDamage(const TObjectPtr<ACharacterBase >& TargetCharacterPtr)const;
	
	/**
	 * < 0 跳过
	 */
	float ChargeTimePercent = 0.f;

	UPROPERTY(Transient)
	TObjectPtr<UAbilityTask_TimerHelper> AbilityTask_TimerHelperPtr = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UAbilityTask_ASCPlayMontage> AbilityTask_ASCPlayMontagePtr = nullptr;

	TObjectPtr<FItemProxy_DescriptionType> ItemProxy_DescriptionPtr = nullptr;

	TSet<ACharacterBase*> HasCollisionCharacters;
};
