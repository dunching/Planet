// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"

#include "GenerateType.h"
#include "HUDInterface.h"
#include "InventoryComponent.h"
#include "LayoutInterfacetion.h"


#include "InteractionTransactionLayout.generated.h"

class UToolsMenu;
class UGoodsItem;



struct FToolsSocketInfo;
struct FConsumableSocketInfo;

/**
 * 濒死状态
 */
UCLASS()
class PLANET_API UInteractionTransactionLayout :
	public UMyUserWidget,
	public ILayoutInterfacetion
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void Enable() override;

	virtual void DisEnable() override;

	virtual ELayoutCommon GetLayoutType() const override final;

private:

	UFUNCTION()
	void OnQuitClicked();

	UFUNCTION()
	void OnBuyClicked();

	UFUNCTION()
	void OnAddClicked();

	UFUNCTION()
	void OnSubClicked();

	UFUNCTION()
	void OnMaxClicked();

	UFUNCTION()
	void OnEditableTextBoxChangedEvent(
		const FText& Text
		);

	UFUNCTION()
	void OnItemClicked(
		UGoodsItem* ItemPtr
		);

	void OnCoinChanged(
		int32,
		int32 NewValue
		);

	void OnTraderSkillProxyChanged(
		const TSharedPtr<
			FSkillProxy>& ProxySPtr,
		EProxyModifyType ProxyModifyType
		);

	void OnTraderWeaponProxyChanged(
		const TSharedPtr<
			FWeaponProxy>& ProxySPtr,
		EProxyModifyType ProxyModifyType
		);

	void OnTraderConsumableProxyChanged(
		const TSharedPtr<
			FConsumableProxy>& ProxySPtr,
		EProxyModifyType ProxyModifyType,
		int32 Num
		);

	void NewNum(
		int32 Num
		);

	int32 CalculateCost() const;

	TObjectPtr<AHumanCharacter_AI> CharacterPtr = nullptr;

	TSharedPtr<FBasicProxy> CurrentProxyPtr = nullptr;

	int32 CurrentNum = 0;

	UInventoryComponent::FOnSkillProxyChanged::FCallbackHandleSPtr OnSkillProxyChangedDelegateHandle;

	UInventoryComponent::FOnWeaponProxyChanged::FCallbackHandleSPtr OnWeaponProxyChangedDelegateHandle;

	UInventoryComponent::FOnConsumableProxyChanged::FCallbackHandleSPtr OnConsumableProxyChangedDelegateHandle;

	TOnValueChangedCallbackContainer<int32>::FCallbackHandleSPtr OnCoinChangedDelegateHandle;
};
