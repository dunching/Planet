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

UCLASS()
class PLANET_API UMaterialIcon :
	public UUserWidget_Override
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;
	
	void BindData(
		const TSharedPtr<FBasicProxy>& ProxySPtr
		);

protected:
	UFUNCTION()
	void OnClickedAddBtn();
	
	UFUNCTION()
	void OnClickedSubBtn();
	
	uint32 CurrentNum = 1;
	
	UPROPERTY(meta = (BindWidget))
	UButton* AddBtn = nullptr;
	
	UPROPERTY(meta = (BindWidget))
	UButton* SubBtn = nullptr;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text = nullptr;

	UPROPERTY(meta = (BindWidget))
	UProxyIcon* ProxyIcon = nullptr;

};

