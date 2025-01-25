
#pragma once

#include "CoreMinimal.h"

#include "ProjectileBase.h"
#include "Skill_WeaponActive_Base.h"

#include "Skill_WeaponActive_Bow.generated.h"

class UPrimitiveComponent;
class UNiagaraComponent;
class UAnimMontage;

class AWeapon_Bow;
class ACharacterBase;
class UAbilityTask_PlayMontage;
class ASkill_WeaponActive_Bow_Projectile;

USTRUCT()
struct FGameplayAbilityTargetData_Bow_RegisterParam :
	public FGameplayAbilityTargetData_SkillBase_RegisterParam
{
	GENERATED_USTRUCT_BODY()

	virtual UScriptStruct* GetScriptStruct() const override;

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)override;

	virtual FGameplayAbilityTargetData_Bow_RegisterParam* Clone()const override;

	bool bIsHomingTowards = false;

	bool bIsMultiple = false;

};

template<>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_Bow_RegisterParam> :
	public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_Bow_RegisterParam>
{
	enum
	{
		WithNetSerializer = true,
	};
};

UCLASS()
class PLANET_API USkill_WeaponActive_Bow : public USkill_WeaponActive_Base
{
	GENERATED_BODY()

public:

	using FRegisterParamType = FGameplayAbilityTargetData_Bow_RegisterParam;

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

	virtual	void UpdateRegisterParam(const FGameplayEventData& GameplayEventData)override;

	TSharedPtr<FRegisterParamType> RegisterParamSPtr = nullptr;

protected:

	virtual void PerformAction(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	)override;

	virtual void CheckInContinue(float InWaitInputTime)override;

	void PlayMontage();

	UFUNCTION()
	void OnNotifyBeginReceived(FName NotifyName);
	
	UFUNCTION()
	void OnMontateComplete();

	void EmitProjectile()const;

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
	UAnimMontage* BowMontage = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 Damage = 10;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 SweepWidth = 500;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<ASkill_WeaponActive_Bow_Projectile>Skill_WeaponActive_RangeTest_ProjectileClass;

	FWeaponActorType* WeaponPtr = nullptr;

};
