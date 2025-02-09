// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UIInterfaces.h"

#include "Common/GenerateType.h"
#include "InventoryComponent.h"
#include "ItemProxy_Minimal.h"
#include "MenuInterface.h"

#include "AllocationSkillsMenu.generated.h"

class UGroupmateIcon;
class UAllocationIconBase;
struct FWeaponProxy;
struct FAllocationbleProxy;
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
		const TSharedPtr<FCharacterProxy>& TargetCharacterProxyPtr, 
		const TSharedPtr<FCharacterProxy>& PlayerCharacterProxyPtr
	);

	void InitialGroupmateList();

	void ResetBackpack(
		const TSharedPtr<FCharacterProxy>& AICharacterProxyPtr,
		const TSharedPtr<FCharacterProxy>& PlayerCharacterProxyPtr
	);

	void ResetUIByData_WeaponSkills(
		const TSharedPtr<FCharacterProxy>& TargetCharacterProxyPtr,
		const TSharedPtr<FCharacterProxy>& PlayerCharacterProxyPtr
		);

	void ResetUIByData_Skills(const TSharedPtr<FCharacterProxy>& PlayerCharacterProxyPtr);

	void ResetUIByData_Consumable(const TSharedPtr<FCharacterProxy>& PlayerCharacterProxyPtr);

	void OnItemProxyDragIcon(bool bIsDragging, const TSharedPtr<FBasicProxy>& ProxyPtr);

	void OnAllocationbableDragIcon(bool bIsDragging, const TSharedPtr<FAllocationbleProxy>& ProxyPtr);

	void OnSelectedCharacterProxy(const TSharedPtr<FCharacterProxy>& ProxyPtr);

	void OnMainWeaponChanged(const TSharedPtr<FWeaponProxy>& ToolSPtr);

	void OnSecondaryWeaponChanged(const TSharedPtr<FWeaponProxy>& ToolSPtr);

	void OnResetData(
		UAllocationIconBase* UAllocationIconPtr
	);

	void OnWeaponProxyChanged(
		const TSharedPtr<FAllocationbleProxy>& PreviousProxyPtr,
		const TSharedPtr<FAllocationbleProxy>& NewProxyPtr,
		const FGameplayTag&SocketTag
	);

	void OnSkillProxyChanged(
		const TSharedPtr<FAllocationbleProxy>& PreviousProxyPtr,
		const TSharedPtr<FAllocationbleProxy>& NewProxyPtr,
		const FGameplayTag&SocketTag
	);

	void OnConsumableProxyChanged(
		const TSharedPtr<FAllocationbleProxy>& PreviousProxyPtr, 
		const TSharedPtr<FAllocationbleProxy>& NewProxyPtr,
		const FGameplayTag&SocketTag
	);

	void SetAllocation(
		const TSharedPtr<FAllocationbleProxy>& PreviousProxyPtr, 
		const TSharedPtr<FAllocationbleProxy>& NewProxyPtr, 
		const FGameplayTag&SocketTag,
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

	TSharedPtr<FCharacterProxy> CurrentProxyPtr = nullptr;

};
