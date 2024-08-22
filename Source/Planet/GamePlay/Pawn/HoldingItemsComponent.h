#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "GenerateType.h"
#include <SceneElement.h>

#include "HoldingItemsComponent.generated.h"

struct FSceneUnitContainer;
class IPlanetControllerInterface;

UCLASS(BlueprintType, Blueprintable)
class UHoldingItemsComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	using FOwnerType = ACharacterBase;

	using FPawnType = ACharacterBase;

	UHoldingItemsComponent(const FObjectInitializer& ObjectInitializer);

	TSharedPtr<FSceneUnitContainer> GetSceneUnitContainer();

	UBasicUnit* AddUnit(FGameplayTag UnitType, int32 Num);

	void AddUnit_Apending(FGameplayTag UnitType, FGuid Guid);

	void SyncApendingUnit(FGuid Guid);

	static FName ComponentName;

protected:

	virtual void BeginPlay()override;

private:

	TMap<FGuid, TMap<FGameplayTag, int32>> SkillUnitApendingMap;

};