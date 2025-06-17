// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UIInterfaces.h"

#include "GenerateTypes.h"
#include "ItemProxy_Minimal.h"
#include <MenuInterface.h>

#include "BackpackMenu.generated.h"

class UBackpackConsumableIcon;
class UBackpackToolIcon;
class UAllocationSkillsMenu;
class UButton;

struct FSceneObjContainer;
struct FSceneProxyContainer;
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
	
	using FOnSelectedProxy = TCallbackHandleContainer<void(const TSharedPtr<FBasicProxy>&)>;
	
	virtual void NativeConstruct()override;

	virtual void EnableMenu()override;

	virtual void DisEnableMenu()override;

	virtual EMenuType GetMenuType()const override final;
	
	bool bIsPlayerMenu = true;

	TSharedPtr<FCharacterProxy> CurrentProxyPtr = nullptr;

	UAllocationSkillsMenu * AllocationSkillsMenuPtr= nullptr;
	
protected:

	void BindEvent();

	UFUNCTION()
	void OnWeaponBtnCliked();

	UFUNCTION()
	void OnSkillBtnCliked();

	UFUNCTION()
	void OnConsumableBtnCliked();

	UFUNCTION()
	void OnMaterialBtnCliked();

	UFUNCTION()
	void OnShowAllBtnCliked();

private:

	UPROPERTY(meta = (BindWidget))
	UButton* MaterialBtn = nullptr;

	TArray<TSharedPtr<FBasicProxy>>GetProxys()const;

	void ResetUIByData_Skill();

	void ResetUIByData_Weapon();

	void ResetUIByData_Consumable();

	void ResetUIByData_All();

	void ResetProxys(const TSet<FGameplayTag>&TargetProxyTypeTag);
	
};
