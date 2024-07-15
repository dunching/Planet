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

	// ��ͨ���桢
	void DisplayActionStateHUD(bool bIsDisplay, ACharacterBase*CharacterPtr = nullptr);
	
	// ���ߡ�
	void DisplayBuildingStateHUD(bool bIsDisplay);

	// ��� ����
	void ViewBackpack(bool bIsDisplay, const FSceneToolsContainer& NewSPHoldItemPerperty = FSceneToolsContainer());

	// ��� װ�似��
	void ViewSkills(bool bIsDisplay, const FSceneToolsContainer& NewSPHoldItemPerperty = FSceneToolsContainer());
	
	// ��� �츳����
	void ViewTalentAllocation(bool bIsDisplay);
	
	// ��� ��Ա�������
	void ViewGroupMatesManagger(bool bIsDisplay, AHumanCharacter* HumanCharacterPtr = nullptr);
	
	// ��ս��Ա�б�
	void DisplayTeamInfo(bool bIsDisplay, AHumanCharacter* HumanCharacterPtr = nullptr);
	
	// Ч������buff��debuff��
	UEffectsList* ViewEffectsList(bool bIsViewMenus);
	
	// ������ ����
	UProgressTips* ViewProgressTips(bool bIsViewMenus);
	
	// ������ ����
	UGetItemInfos* GetItemInfos();

protected:

	UMainUILayout* GetMainUILAyout();

	UPROPERTY()
	UMainUILayout* MainUILayoutPtr = nullptr;

private:

};
