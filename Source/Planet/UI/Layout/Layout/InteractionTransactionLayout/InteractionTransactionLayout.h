// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"

#include "GenerateType.h"
#include "HUDInterface.h"
#include "LayoutInterfacetion.h"


#include "InteractionTransactionLayout.generated.h"

class UToolsMenu;
class UGoodsItem;

struct FCharacterAttributes;

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

	virtual void NativeConstruct()override;

	virtual void Enable()override;
	
	virtual void DisEnable()override;

	virtual ELayoutCommon GetLayoutType() const  override final;
	
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
	void OnEditableTextBoxChangedEvent( const FText& Text);
	
	UFUNCTION()
	void OnItemClicked(UGoodsItem*ItemPtr);

	void OnCoinChanged(int32, int32 NewValue);
	
	void NewNum(int32 Num);

	int32 CalculateCost()const;
	
	TObjectPtr<AHumanCharacter_AI> CharacterPtr = nullptr;

	TSharedPtr<FBasicProxy> CurrentProxyPtr = nullptr;

	int32 CurrentNum = 0;

	TOnValueChangedCallbackContainer<int32>::FCallbackHandleSPtr OnCoinChangedDelegateHandle;
};
