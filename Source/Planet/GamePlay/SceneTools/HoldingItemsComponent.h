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

	FSceneUnitContainer & GetHoldItemProperty();

	UBasicUnit* AddUnit(FGameplayTag UnitType, int32 Num);

	void AddUnit_Apending(FGameplayTag UnitType, FGuid Guid);

	void SyncApendingUnit(FGuid Guid);

	static FName ComponentName;

protected:

	virtual void BeginPlay()override;

	UPROPERTY()
	FSceneUnitContainer HoldItemProperty;

private:

	TMap<FGuid, TMap<FGameplayTag, int32>> SkillUnitApendingMap;

};