// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/IUserObjectListEntry.h"

#include "MyUserWidget.h"

#include "GenerateType.h"
#include "HUDInterface.h"
#include "LayoutInterfacetion.h"
#include "UIInterfaces.h"


#include "GoodsItem.generated.h"

class UToolsMenu;

struct FCharacterAttributes;

struct FToolsSocketInfo;
struct FConsumableSocketInfo;

/**
 * 濒死状态
 */
UCLASS()
class PLANET_API UGoodsItem :
	public UMyUserWidget,
	public IItemProxyIconInterface,
	public IUserObjectListEntry
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;
	
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject)override;

	virtual void InvokeReset(UUserWidget* BaseWidgetPtr)override;

	virtual void ResetToolUIByData(const TSharedPtr<FBasicProxy>& BasicProxyPtr)override;

	virtual void EnableIcon(bool bIsEnable)override;

	TObjectPtr<AHumanCharacter_AI> CharacterPtr = nullptr;
	
	TSharedPtr<FBasicProxy> BasicProxyPtr = nullptr;

	TDelegate<void(UGoodsItem*)> OnClickedDelegate;
	
private:
	
	virtual	void SetItemType(const TSharedPtr<FBasicProxy>& BasicProxyPtr);

	virtual	void SetNum(const TSharedPtr<FBasicProxy>& BasicProxyPtr);

	virtual	void SetValue(const TSharedPtr<FBasicProxy>& BasicProxyPtr);

	UFUNCTION()
	void OnClicked();
	
};