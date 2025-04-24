// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyUserWidget.h"

#include "GenerateType.h"
#include "MenuInterface.h"

#include "ViewTaskMenu.generated.h"

class UTaskItem;
class UTaskItemCategory;

/**
 *
 */
UCLASS()
class PLANET_API UViewTaskMenu :
	public UMyUserWidget,
	public IMenuInterface
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

protected:

	virtual void ResetUIByData()override;

	virtual void SyncData()override;

	virtual EMenuType GetMenuType()const override final;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UTaskItem>TaskItemClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UTaskItemCategory>TaskItemCategoryClass;
};
