
#pragma once

#include "CoreMinimal.h"

#include <GameplayTagContainer.h>

#include "Skill_Base.h"

#include "CS_RootMotion.generated.h"

class UAbilityTask_TimerHelper;
class UTexture2D;
class UConsumableUnit;
class UEffectItem;
class ASPlineActor;
class ATornado;

struct FStreamableHandle;

struct FGameplayAbilityTargetData_RootMotion;

USTRUCT()
struct PLANET_API FGameplayAbilityTargetData_RootMotion : public FGameplayAbilityTargetData
{
	GENERATED_USTRUCT_BODY()

	friend UCS_RootMotion;

	FGameplayAbilityTargetData_RootMotion();

	FGameplayAbilityTargetData_RootMotion(
		const FGameplayTag& Tag
	);

	FGameplayAbilityTargetData_RootMotion* Clone()const;

	// 会一次性修改多个状态码？
	FGameplayTag Tag;

	TWeakObjectPtr<ACharacterBase> TriggerCharacterPtr = nullptr;

	TWeakObjectPtr<ACharacterBase> TargetCharacterPtr = nullptr;

	TSoftObjectPtr<UTexture2D> DefaultIcon;

private:

};

UCLASS()
class PLANET_API UCS_RootMotion : public USkill_Base
{
	GENERATED_BODY()

public:

	UCS_RootMotion();

	virtual void UpdateDuration();

protected:

	virtual void PerformAction();

};
