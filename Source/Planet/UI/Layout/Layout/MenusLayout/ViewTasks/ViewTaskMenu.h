// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UserWidget_Override.h"

#include "GenerateTypes.h"
#include "MenuInterface.h"

#include "ViewTaskMenu.generated.h"

class UTaskItem;
class UTaskItemCategory;
class UProxyIcon;

/**
 *
 */
UCLASS()
class PLANET_API UViewTaskMenu :
	public UUserWidget_Override,
	public IMenuInterface
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

protected:

	virtual void EnableMenu()override;

	virtual void DisEnableMenu()override;

	virtual EMenuType GetMenuType()const override final;

private:

	// 显示选择的任务
	void ActiveCurrentCorrespondingItem();
	
	UFUNCTION()
	void OnSelected(UTaskItem* ItemPtr);
	
	void ModifyTaskText();
	
	void ModifyActiveGuideThreadText();
	
	void ModifyTaskList();
	
	void ModifyRewardProxysList();
	
	UFUNCTION()
	void OnActiveGuideThread();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UTaskItem>TaskItemClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UTaskItemCategory>TaskItemCategoryClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UProxyIcon>RewardProxysClass;
	
	UTaskItem* TaskItemPtr = nullptr;
};
