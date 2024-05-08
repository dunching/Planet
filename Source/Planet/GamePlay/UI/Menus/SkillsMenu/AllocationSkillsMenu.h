// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UIInterfaces.h"

#include "Common/GenerateType.h"
#include "Pawn/HoldingItemsComponent.h"
#include <SceneElement.h>

#include "AllocationSkillsMenu.generated.h"

class UWeaponUnit;
class UBasicUnit;

/**
 *
 */
UCLASS()
class PLANET_API UAllocationSkillsMenu : public UUserWidget, public IItemsMenuInterface
{
	GENERATED_BODY()

public:

	using FDelegateHandle = TCallbackHandleContainer<void(UWeaponUnit*)>::FCallbackHandleSPtr;

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

	void SetHoldItemProperty(const FSceneToolsContainer& NewSPHoldItemPerperty);

protected:

	virtual void ResetUIByData()override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Keys)
	FKey ActiveSkills_1;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Keys)
	FKey ActiveSkills_2;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Keys)
	FKey ActiveSkills_3;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Keys)
	FKey ActiveSkills_4;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Keys)
	FKey WeaponActiveSkills_1;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Keys)
	FKey WeaponActiveSkills_2;
	
private:

	void ResetUIByData_Skills();
	
	void ResetUIByData_Weapons();

	void BindEvent();

	UFUNCTION()
	void OnWeaponsBtnCliked();

	UFUNCTION()
	void OnSkillsBtnCliked();

	void OnMainWeaponChanged(UWeaponUnit*ToolSPtr);

	void OnSecondaryWeaponChanged(UWeaponUnit* ToolSPtr);

	FDelegateHandle MainDelegateHandleSPtr;

	FDelegateHandle SecondaryDelegateHandleSPtr;

	FSceneToolsContainer SPHoldItemPerpertyPtr;

};
