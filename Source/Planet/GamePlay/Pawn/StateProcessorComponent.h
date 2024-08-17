#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <GameplayTagContainer.h>

#include "TalentUnit.h"
#include "GenerateType.h"

#include "StateProcessorComponent.generated.h"

UCLASS(BlueprintType, Blueprintable)
class UStateProcessorComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	static FName ComponentName;

protected:

	virtual void BeginPlay()override;

	void OnGameplayEffectTagCountChanged(const FGameplayTag Tag, int32 Count);

	FDelegateHandle OnGameplayEffectTagCountChangedHandle;

};