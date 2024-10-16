// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UIInterfaces.h"

#include "Common/GenerateType.h"
#include <SceneElement.h>
#include <MenuInterface.h>

#include "BackpackMenu.generated.h"

struct FSceneObjContainer;
struct FSceneUnitContainer;
class UBackpackConsumableIcon;
class UBackpackToolIcon;
struct FSkillProxy;
struct FWeaponProxy;

/**
 *
 */
UCLASS()
class PLANET_API UBackpackMenu : 
	public UMyUserWidget, 
	public IMenuInterface
{
	GENERATED_BODY()

public:

	using FOnDragIconDelegate = TCallbackHandleContainer<void(bool, const TSharedPtr<FBasicProxy>&)>;
	
	virtual void NativeConstruct()override;

	virtual void ResetUIByData()override;

	virtual void SyncData()override;

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

	TArray<TSharedPtr<FBasicProxy>>GetProxys()const;

	void ResetUIByData_Skill();

	void ResetUIByData_Weapon();

	void ResetUIByData_Consumable();

	void ResetUIByData_All();

};
