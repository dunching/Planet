// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyUserWidget.h"

#include "GenerateType.h"
#include "MenuInterface.h"

#include "TaskItem.generated.h"

class UTalentIcon;
class AGuideThread;
class AGuideThread_Main;
class AGuideThread_Branch;

/**
 *
 */
UCLASS()
class PLANET_API UTaskItem :
	public UMyUserWidget
{
	GENERATED_BODY()

public:

	using FOnSeleted = TDelegate<void(UTaskItem*)>;
	
	virtual void NativeConstruct()override;

	void SetGuideThreadType(const TSubclassOf<AGuideThread_Main>& GuideThreadClass);

	void SetGuideThreadType(const TSubclassOf<AGuideThread_Branch>& GuideThreadClass);

	UFUNCTION()
	void OnClicked();

	void SwitchSelected(bool bIsSelected);

	FOnSeleted OnSeleted; 
	
	TSubclassOf<AGuideThread_Main> MainGuideThreadClass;
	
	TSubclassOf<AGuideThread_Branch> BrandGuideThreadClass;

protected:
	
	UFUNCTION(BlueprintImplementableEvent)
	void SwitchSelected_Blueprint(bool bIsSelected_);

private:

	bool bIsSelected = false;
	
};
