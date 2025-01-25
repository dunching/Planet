// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"

#include "CharacterAttributesComponent.h"
#include "HUDInterface.h"

#include "OptionItem.generated.h"

class AGuideActor;
class UPAD_TaskNode_Guide;
class UPAD_TaskNode_Interaction;
class AGuideInteractionActor;

using FOnClickedInteractionItem =  TMulticastDelegate<void(const TSubclassOf<AGuideInteractionActor>&)>;

using FOnClickedIndex =  TMulticastDelegate<void(int32)>;

UCLASS()
class PLANET_API UOptionItem :
	public UMyUserWidget,
	public IHUDInterface
{
	GENERATED_BODY()

public:
	
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	virtual void ResetUIByData() override;

	void SetData(
		const TSubclassOf<AGuideInteractionActor>&TaskNode,
		const std::function<void(const TSubclassOf<AGuideInteractionActor>&)>& InCallback
		);

	void SetData(
		const FString&InOption, int32 InIndex, const std::function<void(int32)>& InOnClickedIndex
		);

protected:

	UFUNCTION()
	void OnClicked();
	
	TSubclassOf<AGuideInteractionActor> TaskNode;

	FOnClickedInteractionItem OnClickedInteractionItem;
	
	FOnClickedIndex OnClickedIndex;
	
	FString Option;

	int32 Index = 0;
	
};
