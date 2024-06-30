#pragma once

#include <functional>
#include <set>

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "InteractiveSkillComponent.h"
#include "GAEvent_Helper.h"

#include "InteractiveToolComponent.generated.h"

struct FToolsSocketInfo
{
	FKey Key;

	FGameplayTag SkillSocket;

	UToolUnit* ToolUnitPtr = nullptr;
};

UCLASS(BlueprintType, Blueprintable)
class UInteractiveToolComponent : public UInteractiveComponent
{
	GENERATED_BODY()

public:

	static FName ComponentName;

	virtual TArray<TSharedPtr<FCanbeActivedInfo>> GetCanbeActiveAction()const override;

	virtual bool ActiveAction(
		const TSharedPtr<FCanbeActivedInfo>& CanbeActivedInfoSPtr, bool bIsAutomaticStop = false
	)override;

	virtual void CancelAction(const TSharedPtr<FCanbeActivedInfo>& CanbeActivedInfoSPtr)override;

#pragma region Tools
	const TArray<TSharedPtr<FCanbeActivedInfo>>& GetCanbeActivedTools()const;

	void RetractputTool();

	void RegisterTool(
		const TMap <FGameplayTag, TSharedPtr<FToolsSocketInfo>>& InToolInfoMap, bool bIsGenerationEvent = true
	);

	TSharedPtr<FToolsSocketInfo> FindTool(const FGameplayTag& Tag);

	const TMap<FGameplayTag, TSharedPtr<FToolsSocketInfo>>& GetTools()const;

	ATool_Base* GetCurrentTool()const;
#pragma endregion Tools

protected:

	virtual void GenerationCanbeActiveEvent()override;

	void CancelSkill_SwitchToTool(const TSharedPtr<FCanbeActivedInfo>& CanbeActivedInfoSPtr);

	bool ActiveSkill_SwitchToTool(
		const TSharedPtr<FCanbeActivedInfo>& CanbeActivedInfoSPtr, bool bIsAutomaticStop = false
	);

	void CancelSkill_ActiveTool(const TSharedPtr<FCanbeActivedInfo>& CanbeActivedInfoSPtr);

	bool ActiveSkill_ActiveTool(
		const TSharedPtr<FCanbeActivedInfo>& CanbeActivedInfoSPtr, bool bIsAutomaticStop = false
	);

	FGameplayTag PreviousTool = FGameplayTag::EmptyTag;

	ATool_Base* CurrentEquipmentPtr = nullptr;

	TMap<FGameplayTag, TSharedPtr<FToolsSocketInfo>>ToolsMap;

	TArray<TSharedPtr<FCanbeActivedInfo>>CanbeActiveToolsAry;

};