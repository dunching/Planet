#pragma once

#include <functional>
#include <set>

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "InteractiveSkillComponent.h"
#include "GAEvent_Helper.h"

#include "InteractiveConsumablesComponent.generated.h"

class UConsumableUnit;
class USkill_Consumable_Generic;

struct FCanbeInteractionInfo;

struct FConsumableSocketInfo
{
	FKey Key;

	FGameplayTag SkillSocket;

	UConsumableUnit* UnitPtr = nullptr;
};

UCLASS(BlueprintType, Blueprintable)
class UInteractiveConsumablesComponent : public UInteractiveComponent
{
	GENERATED_BODY()

public:

	static FName ComponentName;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)override;

	void RegisterConsumable(
		const TMap <FGameplayTag, TSharedPtr<FConsumableSocketInfo>>& InToolInfoMap, bool bIsGenerationEvent = true
	);

	virtual bool ActiveAction(
		const TSharedPtr<FCanbeInteractionInfo>& CanbeActivedInfoSPtr, bool bIsAutomaticStop = false
	)override;

	virtual void CancelAction(const TSharedPtr<FCanbeInteractionInfo>& CanbeActivedInfoSPtr)override;

	TSharedPtr<FConsumableSocketInfo> FindConsumable(const FGameplayTag& SocketTag);

	void InitialBaseGAs();

protected:

	virtual void GenerationCanbeActiveEvent()override;

	TMap<FGameplayTag, TSharedPtr<FConsumableSocketInfo>>ConsumablesMap;
	
};