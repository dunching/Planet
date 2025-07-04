// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UIInterfaces.h"

#include "GenerateTypes.h"
#include "InventoryComponent.h"
#include "ItemProxy_Minimal.h"
#include "MenuInterface.h"

#include "AllocationSkillsMenu.generated.h"

class UGroupmateIcon;
class UAllocationIconBase;
class UItemDetails;
struct FWeaponProxy;
struct IProxy_Allocationble;
struct FCharacterProxy;
struct FCharacterProxy;

/**
 * 分配菜单
 * 中介者
 *
 * 我们为什么要设计两套Icon？
 * 因为在背包里面的Icon可能具备一些额外的功能或者显示
 */
UCLASS()
class PLANET_API UAllocationSkillsMenu :
	public UUserWidget_Override,
	public IMenuInterface
{
	GENERATED_BODY()

public:
	using FOnWeaponChangedDelegate =
	TCallbackHandleContainer<void(
		const TSharedPtr<FWeaponProxy>&
		)>::FCallbackHandleSPtr;

	using FOnResetDataDelegate =
	TCallbackHandleContainer<void(
		UAllocationIconBase*
		)>::FCallbackHandleSPtr;

	UAllocationSkillsMenu(
		const FObjectInitializer& ObjectInitializer
		);

	virtual void PostCDOContruct() override;

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	void OnSelectedProxy(
		const TSharedPtr<FBasicProxy>& ProxyPtr
		);

	void OnItemProxyDragIcon(
		bool bIsDragging,
		const TSharedPtr<FBasicProxy>& ProxyPtr
		);

protected:
	virtual void EnableMenu() override;

	virtual void DisEnableMenu() override;

	virtual EMenuType GetMenuType() const override final;

	void BindEvent();

	void ResetUI(
		const TSharedPtr<FCharacterProxy>& PlayerCharacterProxyPtr
		);

	void InitialGroupmateList();

	void ResetBackpack(
		const TSharedPtr<FCharacterProxy>& PlayerCharacterProxyPtr
		);

	void ResetUIByData_WeaponSkills(
		const TSharedPtr<FCharacterProxy>& TargetCharacterProxyPtr
		);

	void ResetUIByData_Skills(
		const TSharedPtr<FCharacterProxy>& TargetCharacterProxyPtr
		);

	void ResetUIByData_Consumable(
		const TSharedPtr<FCharacterProxy>& TargetCharacterProxyPtr
		);

	void OnAllocationbableDragIcon(
		bool bIsDragging,
		const TSharedPtr<IProxy_Allocationble>& ProxyPtr
		);

	/**
	 * 选择了要分配的人物
	 * @param ProxyPtr 
	 */
	void OnSelectedCharacterProxy(
		const TSharedPtr<FCharacterProxy>& ProxyPtr
		);

	void OnMainWeaponChanged(
		const TSharedPtr<FWeaponProxy>& ToolSPtr
		);

	void OnSecondaryWeaponChanged(
		const TSharedPtr<FWeaponProxy>& ToolSPtr
		);

	void OnResetData(
		UAllocationIconBase* UAllocationIconPtr
		);

	void OnWeaponProxyChanged(
		const TSharedPtr<IProxy_Allocationble>& PreviousProxyPtr,
		const TSharedPtr<IProxy_Allocationble>& NewProxyPtr,
		const FGameplayTag& SocketTag
		);

	void OnSkillProxyChanged(
		const TSharedPtr<IProxy_Allocationble>& PreviousProxyPtr,
		const TSharedPtr<IProxy_Allocationble>& NewProxyPtr,
		const FGameplayTag& SocketTag
		);

	void OnConsumableProxyChanged(
		const TSharedPtr<IProxy_Allocationble>& PreviousProxyPtr,
		const TSharedPtr<IProxy_Allocationble>& NewProxyPtr,
		const FGameplayTag& SocketTag
		);

	void UpdateAllocation(
		const TSharedPtr<IProxy_Allocationble>& PreviousProxyPtr,
		const TSharedPtr<IProxy_Allocationble>& NewProxyPtr,
		const FGameplayTag& SocketTag
		);

	void SetAllocation(
		const TSharedPtr<IProxy_Allocationble>& NewProxyPtr,
		const TSharedPtr<FCharacterProxy>& AllocationCharacterProxyPtr,
		const FGameplayTag& SocketTag
		);

	FOnWeaponChangedDelegate MainDelegateHandleSPtr;

	FOnWeaponChangedDelegate SecondaryDelegateHandleSPtr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UGroupmateIcon> GroupmateIconClass;

	UPROPERTY(meta = (BindWidget))
	UItemDetails* ItemDetails = nullptr;

private:
	TSharedPtr<FCharacterProxy> CurrentProxyPtr = nullptr;
};
