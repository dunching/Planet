// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UserWidget_Override.h"

#include "GenerateTypes.h"
#include "MenuInterface.h"

#include "TaskItem.generated.h"

class UTalentIcon;
class AGuideThreadBase;
class AGuideThread_MainBase;
class AGuideThread_BranchBase;

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

	void SetGuideThreadType(const TSubclassOf<AGuideThread_MainBase>& GuideThreadClass);

	void SetGuideThreadType(const TSubclassOf<AGuideThread_BranchBase>& GuideThreadClass);

	UFUNCTION()
	void OnClicked();

	void SwitchSelected(bool bIsSelected);

	FOnSeleted OnSeleted; 
	
	TSubclassOf<AGuideThread_MainBase> MainGuideThreadClass;
	
	TSubclassOf<AGuideThread_BranchBase> BrandGuideThreadClass;

protected:
	
	UFUNCTION(BlueprintImplementableEvent)
	void SwitchSelected_Blueprint(bool bIsSelected_);

private:

	bool bIsSelected = false;
	
};
