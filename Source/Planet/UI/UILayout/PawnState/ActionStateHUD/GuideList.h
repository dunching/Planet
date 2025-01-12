// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"

#include "CharacterAttributesComponent.h"
#include "HUDInterface.h"

#include "GuideList.generated.h"

class AGuideActor;
class UPAD_TaskNode_Guide;

UCLASS()
class PLANET_API UGuideList :
	public UMyUserWidget,
	public IHUDInterface
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	virtual void ResetUIByData() override;

protected:
	void OnCurrentGuideChagned(AGuideActor* NewGuidePtr);

	void OnGuideEnd(AGuideActor* NewGuidePtr);

	void OnCurrentTaskNodeChanged(const TSoftObjectPtr<UPAD_TaskNode_Guide>& CurrentTaskNode);

	// 当前追踪的引导
	TObjectPtr<AGuideActor> CurrentLineGuidePtr = nullptr;

	
};
