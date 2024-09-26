
#pragma once

#include "CoreMinimal.h"

#include "ProjectileBase.h"
#include "Skill_WeaponActive_Base.h"

#include "Skill_WeaponActive_Bow.generated.h"

class UPrimitiveComponent;
class UAnimMontage;

class AWeapon_Bow;
class ACharacterBase;
class UAbilityTask_PlayMontage;

UCLASS()
class PLANET_API ASkill_WeaponActive_Bow_Projectile : public AProjectileBase
{
	GENERATED_BODY()

public:

};

UCLASS()
class PLANET_API USkill_WeaponActive_Bow : public USkill_WeaponActive_Base
{
	GENERATED_BODY()

public:

	using FWeaponActorType = AWeapon_Bow;

	USkill_WeaponActive_Bow();

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
	);

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	);

	virtual void OnRemoveAbility(
		const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec
	) override;

protected:

	virtual void PerformAction(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	)override;

	void PlayMontage();

	UFUNCTION()
	void OnNotifyBeginReceived(FName NotifyName);
	
	UFUNCTION()
	void OnMontateComplete();

	void EmitProjectile();

	void MakeDamage(ACharacterBase * TargetCharacterPtr);

	void StartTasksLink();

	UFUNCTION()
	void OnProjectileBounce(
		UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex,
		bool bFromSweep, 
		const FHitResult& SweepResult
	);

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	UAnimMontage* HumanMontage = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	float Distance = 100.f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 Damage = 10;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<ASkill_WeaponActive_Bow_Projectile>Skill_WeaponActive_RangeTest_ProjectileClass;

	FWeaponActorType* WeaponPtr = nullptr;

};
