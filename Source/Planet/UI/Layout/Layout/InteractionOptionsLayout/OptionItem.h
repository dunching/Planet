// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UserWidget_Override.h"

#include "CharacterAttributesComponent.h"
#include "HUDInterface.h"
#include "LayoutInterfacetion.h"

#include "OptionItem.generated.h"

class AGuideActor;
class UPAD_TaskNode_Guide;
class UPAD_TaskNode_Interaction;
class AQuestInteractionBase;

using FOnClickedInteractionItem =  TMulticastDelegate<void(const TSubclassOf<AQuestInteractionBase>&)>;

using FOnClickedIndex =  TMulticastDelegate<void(int32)>;

UCLASS()
class PLANET_API UOptionItem :
	public UUserWidget_Override,
	public ILayoutItemInterfacetion
{
	GENERATED_BODY()

public:
	
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	void SetData(
		const TSubclassOf<AQuestInteractionBase>&TaskNode,
		const std::function<void(const TSubclassOf<AQuestInteractionBase>&)>& InCallback
		);

	void SetData(
		const FString&InOption, int32 InIndex, const std::function<void(int32)>& InOnClickedIndex
		);

protected:

	virtual void Enable() override;
	
	virtual void DisEnable() override;

	UFUNCTION()
	void OnClicked();
	
	TSubclassOf<AQuestInteractionBase> TaskNode;

	FOnClickedInteractionItem OnClickedInteractionItem;
	
	FOnClickedIndex OnClickedIndex;
	
	FString Option;

	int32 Index = 0;
	
};
