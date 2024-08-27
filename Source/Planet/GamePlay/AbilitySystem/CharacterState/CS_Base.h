
#pragma once

#include "CoreMinimal.h"

#include <GameplayTagContainer.h>
#include "Skill_Base.h"

#include "CS_Base.generated.h"

class UTexture2D;
class UCS_Base;
class ACharacterBase;

USTRUCT()
struct PLANET_API FStateDisplayInfo
{
	GENERATED_USTRUCT_BODY()

	using FDataChanged = TCallbackHandleContainer<void()>;

	FStateDisplayInfo();

	// 总时间
	float Duration = -1.f;

	// 已过去的时间
	float TotalTime = 0.f;

	// 层数 <= 0 不显示
	int32 Num = 0;

	FString Text = TEXT("");

	FGameplayTag Tag;

	TSoftObjectPtr<UTexture2D> DefaultIcon;

	FDataChanged DataChanged;

};

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

	virtual TWeakPtr<FStateDisplayInfo> GetStateDisplayInfo()const;

	TSharedPtr<FGameplayAbilityTargetData_CS_Base>GameplayAbilityTargetDataBaseSPtr;

protected:

	virtual void InitialStateDisplayInfo();

	TSharedPtr<FStateDisplayInfo>StateDisplayInfoSPtr;

	ACharacterBase* CharacterPtr = nullptr;

};
