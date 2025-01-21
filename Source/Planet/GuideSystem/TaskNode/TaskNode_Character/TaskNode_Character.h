// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"

#include "TaskNode.h"

#include "TaskNode_Character.generated.h"

class USceneComponent;
class ACharacterBase;
class AHumanCharacter_AI;

UCLASS()
class PLANET_API UTaskNode_Temporary_Conversation : public UTaskNode_Temporary
{
	GENERATED_BODY()

public:
	
	UTaskNode_Temporary_Conversation(const FObjectInitializer& ObjectInitializer);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FTaskNode_Conversation_SentenceInfo Sentence;
	
};

UCLASS(Blueprintable, BlueprintType)
class PLANET_API UPAD_TaskNode_Preset_Conversation : public UPAD_TaskNode_Preset
{
	GENERATED_BODY()

public:
	
	UPAD_TaskNode_Preset_Conversation(const FObjectInitializer& ObjectInitializer);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<FTaskNode_Conversation_SentenceInfo> ConversationsAry;
	
};

UCLASS(Blueprintable, BlueprintType)
class PLANET_API UPAD_TaskNode_Preset_AutomaticConversation : public UPAD_TaskNode_Preset_Conversation
{
	GENERATED_BODY()

public:
	
	UPAD_TaskNode_Preset_AutomaticConversation(const FObjectInitializer& ObjectInitializer);
	
};
