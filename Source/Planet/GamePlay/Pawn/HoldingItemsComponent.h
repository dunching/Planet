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

	TSharedPtr<FBasicProxy> AddUnit(FGameplayTag UnitType, int32 Num);

	void AddUnit_Apending(FGameplayTag UnitType, int32 Num, FGuid Guid);

	void SyncApendingUnit(FGuid Guid);

	static FName ComponentName;

protected:

	virtual void BeginPlay()override;

private:

	// 将Client需要（显示）的数据从Server上同步过去
	UFUNCTION(Client, Reliable)
	void OnSkillUnitChanged(
		const FSkillProxy&Skill,
		bool bIsAdd
	);

	TMap<FGuid, TMap<FGameplayTag, int32>> SkillUnitApendingMap;

};