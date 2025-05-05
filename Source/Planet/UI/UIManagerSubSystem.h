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

class UMainHUDLayout;
class UMainMenuLayout;

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
	UMainHUDLayout* GetMainHUDLayout() const;
	
	void SwitchLayout(
		ELayoutCommon MainHUDType,
		const ILayoutInterfacetion::FOnQuit& OnQuit = nullptr
		);
	
	FString GetLayoutName(
		ELayoutCommon MainHUDType
		)const;
#pragma endregion

#pragma region MenuLayout
	UMainMenuLayout* GetMainMenuLayout() const;
	
	void SwitchMenuLayout(
		EMenuType MenuType
		);
	
	FString GetMenuLayoutName(
		EMenuType MenuType
		)const;
#pragma endregion 

protected:

private:
};
