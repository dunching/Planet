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

	using FOnDragSkillIconDelegate = TCallbackHandleContainer<void(bool, USkillUnit*)>;
	
	using FOnDragWeaponIconDelegate = TCallbackHandleContainer<void(bool, UWeaponUnit*)>;

	virtual void NativeConstruct()override;

	virtual void ResetUIByData()override;

	void SetHoldItemProperty(
		const FSceneUnitContainer& PlayerStateSceneUnitContariner,
		const FSceneUnitContainer& CharacterSceneUnitContariner
	);

	bool bIsPlayerMenu = true;

	FOnDragSkillIconDelegate OnDragSkillIconDelegate;

	FOnDragWeaponIconDelegate OnDragWeaponIconDelegate;

protected:

	void BindEvent();

	UFUNCTION()
	void OnWeaponsBtnCliked();

	UFUNCTION()
	void OnSkillsBtnCliked();

private:

	void ResetUIByData_Skills();

	void ResetUIByData_Weapons();

	void ResetUIByData_All();

	FSceneUnitContainer PlayerStateSceneUnitContariner;

	FSceneUnitContainer CharacterSceneUnitContariner;

};
