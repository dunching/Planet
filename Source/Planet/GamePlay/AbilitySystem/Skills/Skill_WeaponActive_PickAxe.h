
#pragma once

#include "CoreMinimal.h"

#include "Skill_Base.h"

#include "Skill_WeaponActive_PickAxe.generated.h"

class UAnimMontage;

class AWeapon_PickAxe;
class ACharacterBase;
class UAbilityTask_PlayMontage;

struct FGameplayAbilityTargetData_PickAxe;

UCLASS()
class PLANET_API USkill_WeaponActive_PickAxe : public USkill_Base
{
	GENERATED_BODY()

public:

	USkill_WeaponActive_PickAxe();

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

	void MakeDamage();

	void StartTasksLink();

	bool bIsAttackEnd = true;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	UAnimMontage* PickAxeMontage = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	UAnimMontage* HumanMontage = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	float Distance = 100.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	int32 Damage = 10;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Weapon Property")
	int32 AD = 15;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Weapon Property")
	int32 AD_Penetration = 10;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Weapon Property")
	int32 AD_PercentPenetration = 5;

	AWeapon_PickAxe* EquipmentAxePtr = nullptr;

};

struct FGameplayAbilityTargetData_Skill_PickAxe : public FGameplayAbilityTargetData
{
	AWeapon_PickAxe* WeaponPtr = nullptr;
};
