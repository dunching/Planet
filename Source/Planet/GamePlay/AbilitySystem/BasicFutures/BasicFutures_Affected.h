
#pragma once

#include "CoreMinimal.h"

#include "PlanetGameplayAbility.h"
#include "BasicFuturesBase.h"

#include "BasicFutures_Affected.generated.h"

class UAnimMontage;
class ACharacterBase;

USTRUCT()
struct FGameplayAbilityTargetData_Affected :
	public FGameplayAbilityTargetData
{
	GENERATED_USTRUCT_BODY()

	virtual UScriptStruct* GetScriptStruct() const override;

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	EAffectedDirection AffectedDirection = EAffectedDirection::kForward;

	int32 RepelDistance = -1;

	FVector RepelDirection = FVector::ZeroVector;

	ACharacterBase* TriggerCharacterPtr = nullptr;

};

template<>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_Affected> :
	public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_Affected>
{
	enum
	{
		WithNetSerializer = true,
	};
};

/**
 * 受击时的“僵直效果”
 */

UCLASS()
class PLANET_API UBasicFutures_Affected : public UBasicFuturesBase
{
	GENERATED_BODY()

public:

	using ActiveParamType = FGameplayAbilityTargetData_Affected;

	UBasicFutures_Affected();

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
	) override;

	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr,
		const FGameplayTagContainer* TargetTags = nullptr,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr
	) const override;

protected:

	// virtual void InitalDefaultTags()override;

	void Perform();

	void PlayMontage(UAnimMontage* CurMontagePtr, float Rate);

	void Move(UAnimMontage* CurMontagePtr, float Rate);

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	UAnimMontage* ForwardMontage = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	UAnimMontage* BackwardMontage = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	UAnimMontage* LeftMontage = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	UAnimMontage* RightMontage = nullptr;

	ACharacterBase* CharacterPtr = nullptr;

	const ActiveParamType* ActiveParamPtr = nullptr;

};
