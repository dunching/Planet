// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UIInterfaces.h"

#include "GenerateTypes.h"
#include "ItemProxy_Minimal.h"
#include <MenuInterface.h>

#include "BackpackMenu.generated.h"

struct FSceneObjContainer;
struct FSceneProxyContainer;
class UBackpackConsumableIcon;
class UBackpackToolIcon;
struct FSkillProxy;
struct FCharacterProxy;
struct FWeaponProxy;

/**
 *
 */
UCLASS()
class PLANET_API UBackpackMenu : 
	public UUserWidget_Override, 
	public IMenuInterface
{
	GENERATED_BODY()

public:

	using FOnDragIconDelegate = TCallbackHandleContainer<void(bool, const TSharedPtr<FBasicProxy>&)>;
	
	virtual void NativeConstruct()override;

	virtual void EnableMenu()override;

	virtual void DisEnableMenu()override;

	virtual EMenuType GetMenuType()const override final;
	
	bool bIsPlayerMenu = true;

	TSharedPtr<FCharacterProxy> CurrentProxyPtr = nullptr;

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
