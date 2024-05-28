
#pragma once

#include "CoreMinimal.h"

#include "Skill_Base.h"
#include "ProjectileBase.h"

#include "Skill_WeaponActive_RangeTest.generated.h"

class UPrimitiveComponent;
class UAnimMontage;

class AWeapon_RangeTest;
class ACharacterBase;
class UAbilityTask_PlayMontage;

struct FGameplayAbilityTargetData_PickAxe;

UCLASS()
class PLANET_API ASkill_WeaponActive_RangeTest_Projectile : public AProjectileBase
{
	GENERATED_BODY()

public:

};

UCLASS()
class PLANET_API USkill_WeaponActive_RangeTest : public USkill_Base
{
	GENERATED_BODY()

public:

	USkill_WeaponActive_RangeTest();

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

	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr,
		const FGameplayTagContainer* TargetTags = nullptr,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr
	) const override;

	virtual void OnRemoveAbility(
		const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec
	) override;

protected:

	virtual void ExcuteStepsLink()override;

	void PlayMontage();

	UFUNCTION()
	void OnNotifyBeginReceived(FName NotifyName);

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

	bool bIsAttackEnd = true;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	UAnimMontage* HumanMontage = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	float Distance = 100.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	int32 Damage = 10;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<ASkill_WeaponActive_RangeTest_Projectile>Skill_WeaponActive_RangeTest_ProjectileClass;

	AWeapon_RangeTest* EquipmentAxePtr = nullptr;

};

struct FGameplayAbilityTargetData_Skill_WeaponActive_RangeTest : public FGameplayAbilityTargetData
{
	AWeapon_RangeTest* WeaponPtr = nullptr;
};
