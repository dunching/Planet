// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"

#include "TaskNode.h"

#include "TaskNode_Character.generated.h"

class USceneComponent;
class ACharacterBase;
class AHumanCharacter_AI;

USTRUCT(Blueprintable, BlueprintType)
struct PLANET_API FTaskNode_Conversation_SentenceInfo
{
	GENERATED_USTRUCT_BODY()

public:

	FTaskNode_Conversation_SentenceInfo();
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FString Sentence;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float DelayTime = 1.f;

	// 念这句词的角色，为空则是“自己”念，否则把这句推送给 AvatorCharacterPtr 念
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSoftObjectPtr<AHumanCharacter_AI>AvatorCharacterPtr = nullptr;
	
};
 
template<>
struct TStructOpsTypeTraits<FTaskNode_Conversation_SentenceInfo> :
	public TStructOpsTypeTraitsBase2<FTaskNode_Conversation_SentenceInfo>
{
	enum
	{
		WithNetSerializer = false,
	};
};

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
