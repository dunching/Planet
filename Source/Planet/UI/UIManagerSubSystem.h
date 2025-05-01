// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <atomic>

#include "CoreMinimal.h"
#include "Engine/World.h"

#include "GenerateType.h"
#include <GameInstance/PlanetGameInstance.h>
#include "Planet.h"
#include "ItemProxy_Minimal.h"
#include "LayoutCommon.h"
#include "LayoutInterfacetion.h"
#include "MainMenuCommon.h"

#include "UIManagerSubSystem.generated.h"

struct FPawnDataStruct;
struct FSceneTool;
struct FCharacterAttributes;

/*
	HUD、UI
*/
UCLASS()
class PLANET_API UUIManagerSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	template <typename FWidgetType>
	using FBeforeDisplayFunc = std::function<void(
		FWidgetType*
		)>;

	static UUIManagerSubSystem* GetInstance();

	UUIManagerSubSystem();

	virtual ~UUIManagerSubSystem();

	virtual void Initialize(
		FSubsystemCollectionBase& Collection
		) override;

#pragma region Layout
	void SwitchLayout(
		ELayoutCommon MainHUDType,
		const ILayoutInterfacetion::FOnQuit& OnQuit = nullptr
		);
#pragma endregion

#pragma region Menu
	void SwitchMenuLayout(
		EMenuType MenuType
		);
#pragma endregion Menu

protected:

private:
};
