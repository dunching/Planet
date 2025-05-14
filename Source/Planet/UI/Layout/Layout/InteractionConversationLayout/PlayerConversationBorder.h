// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"

#include "CharacterAttributesComponent.h"
#include "HUDInterface.h"
#include "LayoutInterfacetion.h"

#include "PlayerConversationBorder.generated.h"

class AGuideActor;
class UPAD_TaskNode_Guide;
class ACharacterBase;

struct FTaskNode_Conversation_SentenceInfo;

UCLASS()
class PLANET_API UPlayerConversationBorder :
	public UMyUserWidget,
	public ILayoutItemInterfacetion
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual void Enable() override;
	
	virtual void DisEnable() override;

	void SetSentence(const FTaskNode_Conversation_SentenceInfo& InSentence);

	void EndBorder();
	
	ACharacterBase* CharacterPtr = nullptr;

protected:

	void OnPlayerHaveNewSentence(bool bIsDisplay, const FTaskNode_Conversation_SentenceInfo&Sentence);

	// 点击了显示文本的框，通常要跳过这一句
	UFUNCTION()
	void On_Skip_Clicked();

	FDelegateHandle OnPlayerHaveNewSentenceDelegateHandle;
};
