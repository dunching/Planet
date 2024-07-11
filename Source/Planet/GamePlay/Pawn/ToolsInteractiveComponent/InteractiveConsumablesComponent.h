#pragma once

#include <functional>
#include <set>

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "InteractiveSkillComponent.h"
#include "GAEvent_Helper.h"

#include "InteractiveConsumablesComponent.generated.h"

class UConsumablesUnit;

struct FCanbeActivedInfo;

struct FConsumableSocketInfo
{
	FKey Key;

	FGameplayTag SkillSocket;

	UConsumablesUnit* UnitPtr = nullptr;
};

UCLASS(BlueprintType, Blueprintable)
class UInteractiveConsumablesComponent : public UInteractiveComponent
{
	GENERATED_BODY()

public:

	static FName ComponentName;

	void RegisterConsumable(
		const TMap <FGameplayTag, TSharedPtr<FConsumableSocketInfo>>& InToolInfoMap, bool bIsGenerationEvent = true
	);

	virtual TArray<TSharedPtr<FCanbeActivedInfo>> GetCanbeActiveAction()const override;

	virtual bool ActiveAction(
		const TSharedPtr<FCanbeActivedInfo>& CanbeActivedInfoSPtr, bool bIsAutomaticStop = false
	)override;

	virtual void CancelAction(const TSharedPtr<FCanbeActivedInfo>& CanbeActivedInfoSPtr)override;

	TSharedPtr<FConsumableSocketInfo> FindConsumable(const FGameplayTag& Tag);

protected:

	virtual void GenerationCanbeActiveEvent()override;

	TMap<FGameplayTag, TSharedPtr<FConsumableSocketInfo>>ToolsMap;

	TArray<TSharedPtr<FCanbeActivedInfo>>CanbeActiveToolsAry;

};