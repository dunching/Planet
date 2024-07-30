// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UIInterfaces.h"

#include "Common/GenerateType.h"
#include "HoldingItemsComponent.h"
#include <SceneElement.h>

#include "AllocationSkillsMenu.generated.h"

class UWeaponUnit;
class UBasicUnit;

/**
 *
 */
UCLASS()
class PLANET_API UAllocationSkillsMenu : public UMyUserWidget, public IItemsMenuInterface
{
	GENERATED_BODY()

public:

	using FOnWeaponChangedDelegate = TCallbackHandleContainer<void(UWeaponUnit*)>::FCallbackHandleSPtr;

	UAllocationSkillsMenu(const FObjectInitializer& ObjectInitializer);

	virtual void PostCDOContruct() override;

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

protected:

	virtual void ResetUIByData()override;

	void ResetUIByData_WeaponSkills(EWeaponSocket WeaponSocket);

	void BindEvent();

	void OnMainWeaponChanged(UWeaponUnit* ToolSPtr);

	void OnSecondaryWeaponChanged(UWeaponUnit* ToolSPtr);

	void OnDragSkillIcon(bool bIsDragging, USkillUnit* SkillUnitPtr);

	void OnDragWeaponIcon(bool bIsDragging, UWeaponUnit* WeaponUnitPtr);

	FOnWeaponChangedDelegate MainDelegateHandleSPtr;

	FOnWeaponChangedDelegate SecondaryDelegateHandleSPtr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Keys)
	FKey ActiveSkills_1_Key;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Keys)
	FKey ActiveSkills_2_Key;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Keys)
	FKey ActiveSkills_3_Key;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Keys)
	FKey ActiveSkills_4_Key;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Keys)
	FKey WeaponActiveSkills_Key;
	
private:

};
