#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystemInstanceController.h"
#include "ProjectileBase.h"
#include "SceneProxyTable.h"

#include "Skill_Active_Base.h"
#include "ScopeValue.h"

#include "Skill_Active_XYFH.generated.h"

class ACameraTrailHelper;
class ASPlineActor;
class ANiagaraActor;

UCLASS()
class PLANET_API ASkill_Active_XYFH_Projectile : public AProjectileBase
{
	GENERATED_BODY()

public:

	ASkill_Active_XYFH_Projectile(const FObjectInitializer& ObjectInitializer);
	
protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Niagara")
	UNiagaraComponent* NiagaraComponent = nullptr;

};

UCLASS()
class PLANET_API UItemProxy_Description_ActiveSkill_XYFH : public UItemProxy_Description_ActiveSkill
{
	GENERATED_BODY()

public:
	/**
	 * 占位符：{HumanMontage1}
	 * 
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	TSoftObjectPtr<UAnimMontage> HumanMontage1 = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	FName Montage1SectionName;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	TSoftObjectPtr<UAnimMontage> HumanMontage2 = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	FName Montage2SectionName;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	TSoftObjectPtr<UAnimMontage> HumanMontage3 = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	FName Montage3SectionName;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	TSoftObjectPtr<UAnimMontage> HumanMontage4 = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	FName Montage4SectionName;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	TSoftObjectPtr<UAnimMontage> HumanMontage5 = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	FName Montage5SectionName;

	const int32 MaxIndex = 5;

	UPROPERTY(EditAnywhere, Category="Niagara", meta = (DisplayName = "Niagara System Asset"))
	TObjectPtr<UNiagaraSystem> Asset;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	float MoveDuration = .1f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	float WaitInputTime = 1.f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 AttackDistance = 800;
	
	int32 UpOffset = 500;

	int32 DownOffset = 500;

	int32 LimitAngle = 30;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<ASPlineActor> SPlineActorClass;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<ANiagaraActor> NiagaraActorClass;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<ASkill_Active_XYFH_Projectile>ProjectileClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	EElementalType ElementalType = EElementalType::kMetal;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 Elemental_Damage = 10;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	float Elemental_Damage_Magnification = .5f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 MaxTargetNum = 1;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 LastSecondNum = 8;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 Offset = 100;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 ForwardOffset = 60;

};

UCLASS()
class PLANET_API USkill_Active_XYFH : public USkill_Active_Base
{
	GENERATED_BODY()

public:
	using FItemProxy_DescriptionType = UItemProxy_Description_ActiveSkill_XYFH;

	USkill_Active_XYFH();

	virtual void OnAvatarSet(
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilitySpec& Spec
		) override;

	virtual void PreActivate(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
		const FGameplayEventData* TriggerEventData = nullptr
		) override;

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
		) override;

	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr,
		const FGameplayTagContainer* TargetTags = nullptr,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr
		) const override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
		) override;

protected:
	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
		) const override;

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

	virtual float GetRemainTime() const override;

	void MoveAlongSPlineTask(
		int32 StartPtIndex,
		int32 EndPtIndex,
		float Duration
		);

	void PlayMontage(
		UAnimMontage* AnimMontagePtr,
		const FName& SectionName
		);

	void OnPlayMontageEnd();

	void MoveAlongSPlineTaskComplete();

private:
	virtual bool CanOncemorePerformAction() const override;

	UFUNCTION()
	void OnNotifyBeginReceived(FName NotifyName);

	UFUNCTION()
	void OnProjectileBounce(
		UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex,
		bool bFromSweep, 
		const FHitResult& SweepResult
	);

	void DurationDelegate(
		UAbilityTask_TimerHelper*,
		float CurrentIntervalTime,
		float IntervalTime
		);

	void EmitProjectile()const;

	void MakeDamage(const TObjectPtr<ACharacterBase >& TargetCharacterPtr)const;
	
	/**
	 *	进入等待输入，如果按下了攻击键，则继续执行
	 */
	bool PerformIfContinue();

	int32 Index = 0;

	/**
	 * 攻击前摇结束，等待输入？
	 */
	bool WaitInput = true;

	UPROPERTY(Transient)
	TObjectPtr<UAbilityTask_TimerHelper> AbilityTask_TimerHelperPtr = nullptr;

	TObjectPtr<ASPlineActor> SPlineActorPtr = nullptr;

	TObjectPtr<FItemProxy_DescriptionType> ItemProxy_DescriptionPtr = nullptr;

	float RemainTime = 0.f;
	
};
