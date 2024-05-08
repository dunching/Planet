#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "GenerateType.h"
#include <SceneElement.h>

#include "HoldingItemsComponent.generated.h"

UCLASS(BlueprintType, Blueprintable)
class UHoldingItemsComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UHoldingItemsComponent(const FObjectInitializer& ObjectInitializer);

	FSceneToolsContainer & GetHoldItemProperty();

	static FName ComponentName;

protected:

	virtual void BeginPlay()override;

	UPROPERTY()
	FSceneToolsContainer HoldItemPropertySPtr;

private:

};