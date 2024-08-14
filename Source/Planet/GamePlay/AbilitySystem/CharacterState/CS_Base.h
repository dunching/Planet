
#pragma once

#include "CoreMinimal.h"

#include <GameplayTagContainer.h>
#include "Skill_Base.h"

#include "CS_Base.generated.h"

class UTexture2D;

USTRUCT()
struct PLANET_API FStateInfo
{
	GENERATED_USTRUCT_BODY()

	FStateInfo();

	float Duration = -1.f;

	FGameplayTag Tag;

	TSoftObjectPtr<UTexture2D> DefaultIcon;

};

USTRUCT()
struct PLANET_API FGameplayAbilityTargetData_CS_Base : public FGameplayAbilityTargetData
{
	GENERATED_USTRUCT_BODY()

	FGameplayAbilityTargetData_CS_Base();

	FGameplayAbilityTargetData_CS_Base(
		const FGameplayTag& Tag
	);

	virtual FGameplayAbilityTargetData_CS_Base* Clone()const;

	// 会一次性修改多个状态码？
	FGameplayTag Tag;

private:

};

UCLASS()
class PLANET_API UCS_Base : public USkill_Base
{
	GENERATED_BODY()

public:

	TMap<FGameplayTag, FStateInfo> StateMap;

};
