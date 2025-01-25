// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"

#include "CharacterAttributesComponent.h"
#include "HUDInterface.h"

#include "GuideList.generated.h"

class AGuideActor;
class AGuideThread;
class UPAD_TaskNode_Guide;

struct FTaskNodeDescript;

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
	void OnCurrentGuideChagned(AGuideThread* NewGuidePtr);

	void OnGuideEnd(AGuideThread* NewGuidePtr);

	void OnCurrentTaskNodeChanged(const FTaskNodeDescript& CurrentTaskNode);

	// 当前追踪的引导
	TObjectPtr<AGuideThread> CurrentLineGuidePtr = nullptr;

	
};
