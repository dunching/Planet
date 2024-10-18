
#pragma once

#include "CoreMinimal.h"

#include <GameplayTagContainer.h>
#include "Skill_Base.h"

#include "CS_Base.generated.h"

class UTexture2D;
class UCS_Base;
class ACharacterBase;

struct FCharacterStateInfo;

/*
	一般为debuff
	给目标单位使用持续性的、脱手的效果

	如果对自己使用，则直接在GA里面使用UBaseFeatureComponent
*/
USTRUCT()
struct PLANET_API FGameplayAbilityTargetData_CS_Base : public FGameplayAbilityTargetData
{
	GENERATED_USTRUCT_BODY()

	friend UCS_Base;

	using FCharacterStateChanged = TCallbackHandleContainer<void(ECharacterStateType, UCS_Base*)>;

	FGameplayAbilityTargetData_CS_Base();

	FGameplayAbilityTargetData_CS_Base(
		const FGameplayTag& Tag
	);

	virtual FGameplayAbilityTargetData_CS_Base* Clone()const;

	// 会一次性修改多个状态码？
	FGameplayTag Tag;

	FCharacterStateChanged CharacterStateChanged;

	TSoftObjectPtr<UTexture2D> DefaultIcon;

protected:

};

UCLASS()
class PLANET_API UCS_Base : public UPlanetGameplayAbility
{
	GENERATED_BODY()

public:

	UCS_Base();

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

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	)override;

	virtual void UpdateDuration();

	TSharedPtr<FGameplayAbilityTargetData_CS_Base>GameplayAbilityTargetDataBaseSPtr;

protected:

	ACharacterBase* CharacterPtr = nullptr;

};
