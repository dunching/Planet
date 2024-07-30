// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UIInterfaces.h"

#include "Common/GenerateType.h"
#include <SceneElement.h>

#include "BackpackMenu.generated.h"

struct FSceneObjContainer;
class UBackpackConsumableIcon;
class UBackpackToolIcon;
class USkillUnit;
class UWeaponUnit;

/**
 *
 */
UCLASS()
class PLANET_API UBackpackMenu : public UMyUserWidget, public IItemsMenuInterface
{
	GENERATED_BODY()

public:

	using FOnDragIconDelegate = TCallbackHandleContainer<void(bool, UBasicUnit*)>;
	
	virtual void NativeConstruct()override;

	virtual void ResetUIByData()override;

	void SetHoldItemProperty(
		const FSceneUnitContainer& PlayerStateSceneUnitContariner,
		const FSceneUnitContainer& CharacterSceneUnitContariner
	);

	bool bIsPlayerMenu = true;

	FOnDragIconDelegate OnDragIconDelegate;

protected:

	void BindEvent();

	UFUNCTION()
	void OnWeaponBtnCliked();

	UFUNCTION()
	void OnSkillBtnCliked();

	UFUNCTION()
	void OnConsumableBtnCliked();

	UFUNCTION()
	void OnShowAllBtnCliked();

private:

	void ResetUIByData_Skill();

	void ResetUIByData_Weapon();

	void ResetUIByData_Consumable();

	void ResetUIByData_All();

	FSceneUnitContainer PlayerStateSceneUnitContariner;

	FSceneUnitContainer CharacterSceneUnitContariner;

};
