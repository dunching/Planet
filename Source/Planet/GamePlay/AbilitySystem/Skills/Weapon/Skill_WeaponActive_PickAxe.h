
#pragma once

#include "CoreMinimal.h"

#include "Skill_WeaponActive_Base.h"

#include "Skill_WeaponActive_PickAxe.generated.h"

class UAnimMontage;

class AWeapon_PickAxe;
class ACharacterBase;
class UAbilityTask_PlayMontage;

USTRUCT()
struct FGameplayAbilityTargetData_Axe_RegisterParam :
	public FGameplayAbilityTargetData_RegisterParam_SkillBase
{
	GENERATED_USTRUCT_BODY()

	virtual UScriptStruct* GetScriptStruct() const override;

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)override;

	virtual FGameplayAbilityTargetData_Axe_RegisterParam* Clone()const override;

};

template<>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_Axe_RegisterParam> :
	public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_Axe_RegisterParam>
{
	enum
	{
		WithNetSerializer = true,
	};
};

UCLASS()
class PLANET_API USkill_WeaponActive_PickAxe : public USkill_WeaponActive_Base
{
	GENERATED_BODY()

public:

	using FRegisterParamType = FGameplayAbilityTargetData_Axe_RegisterParam;

	using FWeaponActorType = AWeapon_PickAxe;

	using FItemProxy_DescriptionType = UItemProxy_Description_WeaponSkill;
	
	USkill_WeaponActive_PickAxe();

	virtual void OnAvatarSet(
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilitySpec& Spec
	) override;

	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr,
		const FGameplayTagContainer* TargetTags = nullptr,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr
	) const override;

	virtual void PreActivate(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
		const FGameplayEventData* TriggerEventData = nullptr
	) override;

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
	void OnMontageComplete();

	UFUNCTION()
	void OnMontageOnInterrupted();

	void MakeDamage();

	void StartTasksLink();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	UAnimMontage* PickAxeMontage = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	UAnimMontage* HumanMontage = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	float Distance = 100.f;

	FWeaponActorType* WeaponActorPtr = nullptr;

	int32 MontageNum = 0;

private:
	
	TObjectPtr<FItemProxy_DescriptionType> ItemProxy_DescriptionPtr = nullptr;

};
