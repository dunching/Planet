// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/HUD.h"

#include "MyUserWidget.h"
#include "LayoutInterfacetion.h"

#include "RegularActionLayout.generated.h"

struct FOnAttributeChangeData;
struct FOnEffectedTawrgetCallback;

class AGuideThread;
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

/**
 *
 */
UCLASS()
class PLANET_API URegularActionLayout :
	public UMyUserWidget,
	public ILayoutInterfacetion
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	virtual void NativeDestruct() override;
	
	virtual void Enable() override;

	virtual void DisEnable() override;

	virtual ELayoutCommon GetLayoutType() const override final;

private:
	
	void OnStartGuide(AGuideThread*GuideThread);
	
	void OnStopGuide(AGuideThread*GuideThread);
	
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
	UEffectsList* InitialEffectsList();

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
		const FOnEffectedTawrgetCallback& ReceivedEventModifyDataCallback
		);

	TCallbackHandleContainer<void(const FOnEffectedTawrgetCallback&)>::FCallbackHandleSPtr EffectOhterCharacterCallbackDelegate;
	
#pragma region MenusUI
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UCharacterRisingTips>FightingTipsClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UBackpackMenu> BackpackMenuClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UPawnStateActionHUD> PawnStateActionHUDClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UPawnStateConsumablesHUD> PawnStateConsumablesHUDClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UPawnStateBuildingHUD> PawnStateBuildingHUDClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UEffectsList> EffectsListClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UProgressTips> ProgressTipsClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UFocusTitle> FocusTitleClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UHUD_TeamInfo> HUD_TeamInfoClass;
#pragma endregion MenusUI

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName PawnBuildingStateHUDSocket;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName GroupMatesManaggerSocket;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName TalentAllocationSocket;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName EffectsListSocket;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName ProgressTipsSocket;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName HUD_TeamSocket;

	UPROPERTY(Transient)
	UFocusIcon* FocusIconPtr = nullptr;

	TObjectPtr<ACharacterBase>PreviousTargetCharacterPtr = nullptr;
};
