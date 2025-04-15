
#pragma once

#include "CoreMinimal.h"

#include "ProjectileBase.h"
#include "Skill_WeaponActive_Base.h"

#include "Skill_WeaponActive_FoldingFan.generated.h"

class UPrimitiveComponent;
class UNiagaraComponent;
class UAnimMontage;

class AWeapon_FoldingFan;
class ACharacterBase;
class UAbilityTask_PlayMontage;
class UAbilityTask_ApplyRootMotion_FlyAway;
class UAbilityTask_ASCPlayMontage;
class ASkill_WeaponActive_FoldingFan_Projectile;

USTRUCT()
struct FGameplayAbilityTargetData_FoldingFan_RegisterParam :
	public FGameplayAbilityTargetData_SkillBase_RegisterParam
{
	GENERATED_USTRUCT_BODY()

	virtual UScriptStruct* GetScriptStruct() const override;

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)override;

	virtual FGameplayAbilityTargetData_FoldingFan_RegisterParam* Clone()const override;

	int32 IncreaseNum = 1;

};

template<>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_FoldingFan_RegisterParam> :
	public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_FoldingFan_RegisterParam>
{
	enum
	{
		WithNetSerializer = true,
	};
};

UCLASS()
class PLANET_API USkill_WeaponActive_FoldingFan : public USkill_WeaponActive_Base
{
	GENERATED_BODY()

public:

	using FRegisterParamType = FGameplayAbilityTargetData_FoldingFan_RegisterParam;

	using FWeaponActorType = AWeapon_FoldingFan;

	USkill_WeaponActive_FoldingFan();

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

	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr,
		const FGameplayTagContainer* TargetTags = nullptr,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr
	) const override;

	virtual bool CommitAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr
	)override;

	virtual void OnRemoveAbility(
		const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec
	) override;

	virtual bool GetNum(int32& Num)const override;

	TSharedPtr<FRegisterParamType> RegisterParamSPtr = nullptr;

protected:

	virtual void PerformAction(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	)override;

	virtual	void UpdateRegisterParam(const FGameplayEventData& GameplayEventData)override;

	void PlayMontage();

	void RootMotion();

	UFUNCTION()
	void OnNotifyBeginReceived(FName NotifyName);
	
	UFUNCTION()
	void OnMontateComplete();

	UFUNCTION()
	void OnMotionComplete();

	void EmitProjectile();

	void MakeDamage(ACharacterBase * TargetCharacterPtr, FWeaponActorType* WeaponProjectilePtr);

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
	
	UFUNCTION()
	void OnCurrentFanNumChanged();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	UAnimMontage* HumanMontage = nullptr;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 Damage = 10;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 Height = 80;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 ResingSpeed = 200;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 FallingSpeed = 100;

	int32 CurrentFanNum = 0;

	FWeaponActorType* WeaponPtr = nullptr;

	UAbilityTask_ASCPlayMontage* AbilityTask_PlayMontage_HumanPtr = nullptr;

	UAbilityTask_ApplyRootMotion_FlyAway* RootMotionPtr = nullptr;

};
