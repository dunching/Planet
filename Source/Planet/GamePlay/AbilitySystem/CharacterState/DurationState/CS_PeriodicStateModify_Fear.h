
#pragma once

#include "CoreMinimal.h"

#include <GameplayTagContainer.h>

#include "CS_PeriodicStateModify.h"

#include "CS_PeriodicStateModify_Fear.generated.h"

class UAbilityTask_TimerHelper;
class UTexture2D;
struct FConsumableProxy;
class UEffectItem;
class ASPlineActor;
class ATornado;

struct FStreamableHandle;

UCLASS()
class PLANET_API UCS_PeriodicStateModify_Fear : public UCS_PeriodicStateModify
{
	GENERATED_BODY()

public:

	UCS_PeriodicStateModify_Fear() {};

};
