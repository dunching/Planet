// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/HUD.h"

#include "UserWidget_Override.h"
#include "LayoutInterfacetion.h"
#include "TemplateHelper.h"

#include "RegularActionLayout.generated.h"

struct FOnAttributeChangeData;
struct FOnEffectedTargetCallback;

class AQuestChain;
class AQuestChainBase;
class UMainUILayout;
class URaffleMenu;
class UPawnStateActionHUD;
class UPawnStateConsumablesHUD;
class UPawnStateBuildingHUD;
class UItemProxyDragDropOperation;
class UItemProxyDragDropOperationWidget;
class UAllocationToolsMenu;
class UAllocationSkillsMenu;
class UBackpackMenu;
class UCharacterTitle;
class UEffectsList;
class UProgressTips;
class UTalentAllocation;
class UGroupManaggerMenu;
class UHUD_TeamInfo;
class UFocusIcon;
class UCharacterRisingTips;
class AHumanCharacter;
class AHorseCharacter;
class AGeneratorNPC;
class UFocusTitle;
class ACharacterBase;
class UUpgradePromt;
class UOverlay;

/**
 *
 */
UCLASS()
class PLANET_API URegularActionLayout :
	public UUserWidget_Override,
	public ILayoutInterfacetion
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	virtual void Enable() override;

	virtual void DisEnable() override;

	virtual ELayoutCommon GetLayoutType() const override final;

	virtual UOverlaySlot* DisplayWidget(
		const TSubclassOf<UUserWidget>& WidgetClass,
		const std::function<void(UUserWidget*)>& Initializer
		) override;

	virtual bool RemovedWidgets() override;
	
	virtual void RemoveWidget(const TSubclassOf<UUserWidget>& WidgetClass) override;
	
private:
	void OnStartGuide(
		AQuestChainBase* GuideThread
		);

	void OnStopGuide(
		AQuestChainBase* GuideThread
		);

	UFUNCTION()
	void OnQuitChallengeBtnClicked();

	// 锁定目标时,上方显示的状态栏
	void OnFocusCharacter(
		ACharacterBase* TargetCharacterPtr
		);

	// 出战队员列表
	void DisplayTeamInfo(
		bool bIsDisplay,
		AHumanCharacter* HumanCharacterPtr = nullptr
		);

	// 效果栏（buff、debuff）
	UEffectsList* InitialEffectsList(bool bIsDisplay);

	// 进度条/工具
	UProgressTips* ViewProgressTips(
		bool bIsViewMenus
		);

	void OnHPChanged(
		const FOnAttributeChangeData&
	
		);

	void OnFocusDestruct(
		UUserWidget* UIPtr
		);

protected:
	void OnEffectOhterCharacter(
		const FOnEffectedTargetCallback& ReceivedEventModifyDataCallback
		);

#pragma region MenusUI
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UCharacterRisingTips>FightingTipsClass;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUpgradePromt>UpgradePromtClass;
	
#pragma endregion MenusUI
	
	UPROPERTY(meta = (BindWidget))
	UOverlay* OtherWidgetsOverlay = nullptr;

	UPROPERTY(Transient)
	UFocusIcon* FocusIconPtr = nullptr;

	TObjectPtr<ACharacterBase> PreviousTargetCharacterPtr = nullptr;

	FDelegateHandle StartGuideDelegateHandle;

	FDelegateHandle StopGuideDelegateHandle;

	FDelegateHandle Max_HPChangedDelegateHandle;

	FDelegateHandle HPChangedDelegateHandle;

	TCallbackHandleContainer<void(
		ACharacterBase*
	
		)>::FCallbackHandleSPtr FocusCharacterDelegateSPtr;

	TCallbackHandleContainer<void(
		const FOnEffectedTargetCallback&
		)>::FCallbackHandleSPtr EffectOhterCharacterCallbackDelegate;

	TCallbackHandleContainer<void(
		const FOnEffectedTargetCallback&
		)>::FCallbackHandleSPtr ReceivedOhterCharacterCallbackDelegate;

private:
	
	TOnValueChangedCallbackContainer<uint8>::FCallbackHandleSPtr LevelChangedDelegateHandle;
	
	void OnLevelChanged(
		int32 Level
		);
};
