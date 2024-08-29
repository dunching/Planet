
#pragma once

#include "CoreMinimal.h"
#include "AbilityTask_TimerHelper.h"
#include "ProjectileBase.h"

#include "Skill_Active_Base.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"

#include "Skill_Active_IceGun.generated.h"

class UAnimMontage;
class ACharacterBase;

struct FGameplayAbilityTargetData_IceGun : public FGameplayAbilityTargetData
{
	ACharacterBase* TargetCharacterPtr = nullptr;
};

UCLASS()
class PLANET_API USkill_Active_IceGun : public USkill_Active_Base
{
	GENERATED_BODY()

public:

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

	virtual void PerformAction(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	);

	void FindTarget();

	void PlayMontage();

	UFUNCTION()
	void OnHitCallback(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION()
	void OnNotifyBeginReceived(FName NotifyName);
	
	void OnProjectileBounce(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
							UPrimitiveComponent* OtherComp,
							int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void OnTimerHelperTick(UAbilityTask_TimerHelper* TaskPtr, float CurrentInterval, float Interval);
	void ExcuteTasks();
	void OnOverlap(AActor* OtherActor);
	
	UFUNCTION()
	bool ResetIceGun(UAbilityTask_TimerHelper* TaskPtr);
	USceneComponent* GetNearnestTarget(ACharacterBase* SelfCharacter, float SearchRadius);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	UAnimMontage* HumanMontage = nullptr;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	float Duration = 1.5f;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	float Distance = 800.f;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	float ToCharacterOffset = 100.f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	float Offset = 20.f;

	FVector StartPt = FVector::ZeroVector;

	FVector EndPt = FVector::ZeroVector;

	ASkill_IceGun_Projectile* IceGunPtr = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<ASkill_IceGun_Projectile> IceGunPtrClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSoftObjectPtr<USoundBase>HitSound;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSoftObjectPtr<UParticleSystem>HitParticle;
	
};

UCLASS()
class PLANET_API ASkill_IceGun_Projectile : public AActor
{
	GENERATED_BODY()

	ASkill_IceGun_Projectile(const FObjectInitializer& ObjectInitializer);
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UProjectileMovementComponent>  ProjectileMovementComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UCapsuleComponent> CapsuleComponentPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UParticleSystemComponent>  ParticleSystemComp;
	
	FTimerHandle TimerHandle; 
	void Reset();

	void Activate();
};
