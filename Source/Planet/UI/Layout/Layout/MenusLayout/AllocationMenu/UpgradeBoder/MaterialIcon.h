// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UserWidget_Override.h"

#include "CharacterAttributesComponent.h"
#include "HUDInterface.h"
#include "LayoutInterfacetion.h"

#include "MaterialIcon.generated.h"

class UProxyIcon;
class UTextBlock;
class UButton;

class UUpgradeBoder;

struct FMaterialProxy;

UCLASS()
class PLANET_API UMaterialIcon :
	public UUserWidget_Override
{
	GENERATED_BODY()

public:

using FDelegateHandle=  TOnValueChangedCallbackContainer<int32>::FCallbackHandleSPtr ;
	
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;
	
	void BindData(
		const TSharedPtr<FMaterialProxy>& ProxySPtr
		);

	UUpgradeBoder*UpgradeBoderPtr = nullptr;

	TSharedPtr<FMaterialProxy> ProxySPtr = nullptr;
	
protected:
	void OnValueChanged(int32 OldValue, int32 NewValue);
	
	UFUNCTION()
	void OnClickedAddAllBtn();
	
	UFUNCTION()
	void OnClickedAddBtn();
	
	UFUNCTION()
	void OnClickedSubBtn();

	void Update();
	
	int32 CurrentNum = 0;
	
	UPROPERTY(meta = (BindWidget))
	UButton* AddAllBtn = nullptr;
	
	UPROPERTY(meta = (BindWidget))
	UButton* AddBtn = nullptr;
	
	UPROPERTY(meta = (BindWidget))
	UButton* SubBtn = nullptr;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text = nullptr;

	UPROPERTY(meta = (BindWidget))
	UProxyIcon* ProxyIcon = nullptr;

	FDelegateHandle DelegateHandle = nullptr;
};

