// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UserWidget_Override.h"

#include "CharacterAttributesComponent.h"
#include "HUDInterface.h"
#include "LayoutInterfacetion.h"

#include "GuideItem.generated.h"

class AGuideActor;
class AQuestChainBase;
class UPAD_TaskNode_Guide;

struct FTaskNodeDescript;

UCLASS()
class PLANET_API UGuideItem :
	public UUserWidget_Override,
	public ILayoutItemInterfacetion
{
	GENERATED_BODY()

public:
	
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual void NativeDestruct() override;
	
	void BindGuide(AQuestChainBase* NewGuidePtr);

protected:
	virtual void Enable() override;
	
	virtual void DisEnable() override;

	void OnStopGuide(AQuestChainBase* NewGuidePtr);

	void OnCurrentTaskNodeChanged(const FTaskNodeDescript& CurrentTaskNode);

	void OnGuideThreadPropertyChagned(const FString& NewTaskName);
	
	// 当前追踪的引导
	TObjectPtr<AQuestChainBase> CurrentLineGuidePtr = nullptr;

	FDelegateHandle OnStartGuideHandle;
};
