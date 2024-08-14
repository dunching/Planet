
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

UCLASS()
class PLANET_API UCS_Base : public USkill_Base
{
	GENERATED_BODY()

public:

	TMap<FGameplayTag, FStateInfo> StateMap;

};
