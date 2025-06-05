// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include <GameplayTagContainer.h>

#include "AIComponent.h"
#include "UserWidget_Override.h"
#include "GenerateTypes.h"
#include "STT_CommonData.h"

#include "ConversationBorder.generated.h"

class ACharacterBase;

class UToolIcon;
struct FOnAttributeChangeData;

/**
 *
 */
UCLASS()
class PLANET_API UConversationBorder : public UUserWidget_Override
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	void SetSentence(const FTaskNode_Conversation_SentenceInfo& InSentence);
	
	ACharacterBase* CharacterPtr = nullptr;

protected:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Offset = 0.f;

	float HalfHeight = 0.f;

	bool ResetPosition(float InDeltaTime);

	float Interval = 1.f / 30.f;

	float CurrentInterval = 0.f;

	FTSTicker::FDelegateHandle TickDelegateHandle;

	FTaskNode_Conversation_SentenceInfo Sentence;
};
