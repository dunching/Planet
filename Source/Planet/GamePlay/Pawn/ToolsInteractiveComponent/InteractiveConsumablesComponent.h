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

struct FCanbeActivedInfo;

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

	void RegisterConsumable(
		const TMap <FGameplayTag, TSharedPtr<FConsumableSocketInfo>>& InToolInfoMap, bool bIsGenerationEvent = true
	);

	virtual TArray<TSharedPtr<FCanbeActivedInfo>> GetCanbeActiveAction()const override;

	virtual bool ActiveAction(
		const TSharedPtr<FCanbeActivedInfo>& CanbeActivedInfoSPtr, bool bIsAutomaticStop = false
	)override;

	virtual void CancelAction(const TSharedPtr<FCanbeActivedInfo>& CanbeActivedInfoSPtr)override;

	TSharedPtr<FConsumableSocketInfo> FindConsumable(const FGameplayTag& Tag);

	void InitialBaseGAs();

protected:

	virtual void GenerationCanbeActiveEvent()override;

#pragma region GAs
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TSubclassOf<USkill_Consumable_Generic>Skill_Consumable_GenericClass;

	FGameplayAbilitySpecHandle Skill_Consumable_GenericHandle;
#pragma endregion GAs

	TMap<FGameplayTag, TSharedPtr<FConsumableSocketInfo>>ToolsMap;

	TArray<TSharedPtr<FCanbeActivedInfo>>CanbeActiveToolsAry;

	FGameplayAbilitySpecHandle AbilitieHandle;

};