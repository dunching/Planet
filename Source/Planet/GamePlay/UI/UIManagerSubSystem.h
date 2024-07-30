// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <atomic>

#include "CoreMinimal.h"
#include "Engine/World.h"

#include "GenerateType.h"
#include <GameInstance/PlanetGameInstance.h>
#include "Planet.h"
#include "SceneElement.h"

#include "UIManagerSubSystem.generated.h"

class UWorld;

class ACharacterBase;

class UToolsMenu;
class UInteractionToAIMenu;
class UBackpackMenu;
class UCreateMenu;
class UPromptBox;
class AHumanCharacter;
class UPiginteraction;
class UDestroyProgress;
class UMainUILayout;
class UEffectsList;
class UProgressTips;
class UHUD_TeamInfo;
class UGetItemInfos;
class URaffleMenu;

struct FPawnDataStruct;
struct FSceneTool;
struct FCharacterAttributes;

UCLASS()
class PLANET_API UUIManagerSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	template<typename FWidgetType>
	using FBeforeDisplayFunc = std::function<void(FWidgetType*)>;

	static UUIManagerSubSystem* GetInstance();

	UUIManagerSubSystem();

	virtual ~UUIManagerSubSystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// 普通界面(技能状态HUD)、
	void DisplayActionStateHUD(bool bIsDisplay, ACharacterBase*CharacterPtr = nullptr);
	
	// 工具、
	void DisplayBuildingStateHUD(bool bIsDisplay);

	// 浏览 背包、装配技能
	void ViewBackpack(bool bIsDisplay);

	// 浏览 天赋界面
	void ViewTalentAllocation(bool bIsDisplay);
	
	// 浏览 成员管理界面
	void ViewGroupMatesManagger(bool bIsDisplay, AHumanCharacter* HumanCharacterPtr = nullptr);
	
	// 出战队员列表
	void DisplayTeamInfo(bool bIsDisplay, AHumanCharacter* HumanCharacterPtr = nullptr);

	// 抽卡界面
	void ViewRaffleMenu(bool bIsDisplay);

	// 效果栏（buff、debuff）
	UEffectsList* ViewEffectsList(bool bIsViewMenus);
	
	// 进度条 工具
	UProgressTips* ViewProgressTips(bool bIsViewMenus);
	
	// 进度条 工具
	UGetItemInfos* GetItemInfos();

protected:

	UMainUILayout* GetMainUILAyout();

	UPROPERTY()
	UMainUILayout* MainUILayoutPtr = nullptr;
	
	UPROPERTY()
	URaffleMenu* RaffleMenuPtr = nullptr;

private:

};
