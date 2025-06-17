// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UserWidget_Override.h"

#include "CharacterAttributesComponent.h"
#include "HUDInterface.h"
#include "LayoutInterfacetion.h"

#include "UpgradeBoder.generated.h"

class UProgressBar;
class UButton;
class UTextBlock;
class UScrollBox;
class UMaterialIcon;

struct FPassiveSkillProxy;

UCLASS()
class PLANET_API UUpgradeBoder :
	public UUserWidget_Override
{
	GENERATED_BODY()
public:

	virtual void NativeConstruct() override;
	
	void BindData(
		const TSharedPtr<FPassiveSkillProxy>& ProxySPtr
		);

protected:
	UFUNCTION()
	void OnClickedUpgradeBtn();
	
	UFUNCTION()
	void OnClickedCancelBtn();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UMaterialIcon> MaterialIconClass = nullptr;

	UPROPERTY(meta = (BindWidget))
	UButton* UpgradeBtn = nullptr;

	UPROPERTY(meta = (BindWidget))
	UButton* CancelBtn = nullptr;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* ProgressBar = nullptr;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentLevelText = nullptr;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* OffsetLevelText = nullptr;

	UPROPERTY(meta = (BindWidget))
	UScrollBox* ScrollBox = nullptr;

	TSharedPtr<FPassiveSkillProxy> ProxySPtr = nullptr;

	TSet<TSharedPtr<FBasicProxy> >CosumeProxysSet;

	int32 OffsetLevel = 0;
};
