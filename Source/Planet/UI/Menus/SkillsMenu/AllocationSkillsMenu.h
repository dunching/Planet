// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UIInterfaces.h"

#include "Common/GenerateType.h"
#include "HoldingItemsComponent.h"
#include <SceneElement.h>

#include "AllocationSkillsMenu.generated.h"

class UGroupmateIcon;
class UWeaponUnit;
class UBasicUnit;
class UCharacterUnit;

/**
 *
 */
UCLASS()
class PLANET_API UAllocationSkillsMenu : public UMyUserWidget, public IMenuInterface
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

	void BindEvent();

	void ResetUI(UCharacterUnit* TargetCharacterUnitPtr, UCharacterUnit* PlayerCharacterUnitPtr);

	void InitialGroupmateList();

	void ResetBackpack(UCharacterUnit * AICharacterUnitPtr, UCharacterUnit* PlayerCharacterUnitPtr);

	void ResetUIByData_WeaponSkills(UCharacterUnit* PlayerCharacterUnitPtr);

	void ResetUIByData_Skills(UCharacterUnit* PlayerCharacterUnitPtr);

	void ResetUIByData_Consumable(UCharacterUnit* PlayerCharacterUnitPtr);

	void OnDragIcon(bool bIsDragging, UBasicUnit* UnitPtr);
	
	void OnSelectedCharacterUnit(UCharacterUnit* UnitPtr);

	void OnMainWeaponChanged(UWeaponUnit* ToolSPtr);

	void OnSecondaryWeaponChanged(UWeaponUnit* ToolSPtr);

	void OnWeaponUnitChanged(UBasicUnit* PreviousUnitPtr, UBasicUnit* NewUnitPtr);

	void OnSkillUnitChanged(UBasicUnit* PreviousUnitPtr, UBasicUnit* NewUnitPtr);
	
	void OnConsumableUnitChanged(UBasicUnit* PreviousUnitPtr, UBasicUnit* NewUnitPtr);
	
	void SetAllocation(UBasicUnit* PreviousUnitPtr, UBasicUnit* NewUnitPtr, bool bIsReplaced);

	void SyncAllocation2Character();

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
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Keys)
	FKey Consumable_1_Key;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Keys)
	FKey Consumable_2_Key;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Keys)
	FKey Consumable_3_Key;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Keys)
	FKey Consumable_4_Key;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UGroupmateIcon>GroupmateIconClass;
	
private:

	UCharacterUnit* CurrentUnitPtr = nullptr;

};
