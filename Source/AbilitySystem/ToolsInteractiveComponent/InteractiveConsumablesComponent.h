#pragma once

#include <functional>
#include <set>

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "InteractiveSkillComponent.h"
#include "GAEvent_Helper.h"

#include "InteractiveConsumablesComponent.generated.h"

UCLASS(BlueprintType, Blueprintable)
class ABILITYSYSTEM_API UInteractiveConsumablesComponent : public UInteractiveComponent
{
	GENERATED_BODY()

public:

	static FName ComponentName;

};