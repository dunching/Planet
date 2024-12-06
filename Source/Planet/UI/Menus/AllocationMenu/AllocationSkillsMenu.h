// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UIInterfaces.h"

#include "Common/GenerateType.h"
#include "HoldingItemsComponent.h"
#include "ItemProxy_Minimal.h"
#include "MenuInterface.h"

#include "AllocationSkillsMenu.generated.h"

class UGroupmateIcon;
class UAllocationIconBase;
struct FWeaponProxy;
struct FBasicProxy;
struct FCharacterProxy;

/**
 *
 */
UCLASS()
class PLANET_API UAllocationSkillsMenu :
	public UMyUserWidget,
	public IMenuInterface
{
	GENERATED_BODY()

public:

	using FOnWeaponChangedDelegate = 
		TCallbackHandleContainer<void(const TSharedPtr<FWeaponProxy>&)>::FCallbackHandleSPtr;

	using FOnResetDataDelegate =
		TCallbackHandleContainer<void(UAllocationIconBase*)>::FCallbackHandleSPtr;

	UAllocationSkillsMenu(const FObjectInitializer& ObjectInitializer);

	virtual void PostCDOContruct() override;

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

protected:

	virtual void ResetUIByData()override;

	virtual void SyncData()override;

	void BindEvent();

	void ResetUI(
		const TSharedPtr<FCharacterProxy>& TargetCharacterUnitPtr, 
		const TSharedPtr<FCharacterProxy>& PlayerCharacterUnitPtr
	);

	void InitialGroupmateList();

	void ResetBackpack(
		const TSharedPtr<FCharacterProxy>& AICharacterUnitPtr,
		const TSharedPtr<FCharacterProxy>& PlayerCharacterUnitPtr
	);

	void ResetUIByData_WeaponSkills(
		const TSharedPtr<FCharacterProxy>& TargetCharacterUnitPtr,
		const TSharedPtr<FCharacterProxy>& PlayerCharacterUnitPtr
		);

	void ResetUIByData_Skills(const TSharedPtr<FCharacterProxy>& PlayerCharacterUnitPtr);

	void ResetUIByData_Consumable(const TSharedPtr<FCharacterProxy>& PlayerCharacterUnitPtr);

	void OnDragIcon(bool bIsDragging, const TSharedPtr<FBasicProxy>& UnitPtr);

	void OnSelectedCharacterUnit(const TSharedPtr<FCharacterProxy>& UnitPtr);

	void OnMainWeaponChanged(const TSharedPtr<FWeaponProxy>& ToolSPtr);

	void OnSecondaryWeaponChanged(const TSharedPtr<FWeaponProxy>& ToolSPtr);

	void OnResetData(
		UAllocationIconBase* UAllocationIconPtr
	);

	void OnWeaponUnitChanged(
		const TSharedPtr<FBasicProxy>& PreviousUnitPtr,
		const TSharedPtr<FBasicProxy>& NewUnitPtr
	);

	void OnSkillUnitChanged(
		const TSharedPtr<FBasicProxy>& PreviousUnitPtr,
		const TSharedPtr<FBasicProxy>& NewUnitPtr
	);

	void OnConsumableUnitChanged(
		const TSharedPtr<FBasicProxy>& PreviousUnitPtr, 
		const TSharedPtr<FBasicProxy>& NewUnitPtr
	);

	void SetAllocation(
		const TSharedPtr<FBasicProxy>& PreviousUnitPtr, 
		const TSharedPtr<FBasicProxy>& NewUnitPtr, 
		bool bIsReplaced
	);

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

	TSharedPtr<FCharacterProxy> CurrentUnitPtr = nullptr;

};
