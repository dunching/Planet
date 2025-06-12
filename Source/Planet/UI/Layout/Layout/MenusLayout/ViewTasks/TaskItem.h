// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UserWidget_Override.h"

#include "GenerateTypes.h"
#include "MenuInterface.h"

#include "TaskItem.generated.h"

class UTalentIcon;
class AQuestChainBase;
class AQuestChain_MainBase;
class AQuestChain_BranchBase;

/**
 *
 */
UCLASS()
class PLANET_API UTaskItem :
	public UUserWidget_Override
{
	GENERATED_BODY()

public:

	using FOnSeleted = TDelegate<void(UTaskItem*)>;
	
	virtual void NativeConstruct()override;

	void SetGuideThreadType(const TSubclassOf<AQuestChain_MainBase>& GuideThreadClass);

	void SetGuideThreadType(const TSubclassOf<AQuestChain_BranchBase>& GuideThreadClass);

	UFUNCTION()
	void OnClicked();

	void SwitchSelected(bool bIsSelected);

	FOnSeleted OnSeleted; 
	
	TSubclassOf<AQuestChain_MainBase> MainGuideThreadClass;
	
	TSubclassOf<AQuestChain_BranchBase> BrandGuideThreadClass;

protected:
	
	UFUNCTION(BlueprintImplementableEvent)
	void SwitchSelected_Blueprint(bool bIsSelected_);

private:

	bool bIsSelected = false;
	
};
