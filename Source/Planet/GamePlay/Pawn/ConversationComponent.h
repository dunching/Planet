#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"

#include "CDCaculator.h"

#include "ConversationComponent.generated.h"

class UPAD_TaskNode_Conversation;
class UConversationBorder;

/* 角色的会话组件
 * 比如角色在念某一段台词是需要显示一个气泡和音频播放
 */
UCLASS(BlueprintType, Blueprintable)
class PLANET_API UConversationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	static FName ComponentName;

	UConversationComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(NetMulticast, Reliable)
	void DisplaySentence(
		UPAD_TaskNode_Conversation* InPAD_TaskNode_ConversationPtr,
		int32 InSentenceIndex
		);

	UFUNCTION(NetMulticast, Reliable)
	void CloseConversationborder();

protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI")
	TSubclassOf<UConversationBorder> UConversationBorderClass;

	UPROPERTY(Transient)
	UPAD_TaskNode_Conversation* PAD_TaskNode_ConversationPtr = nullptr;

	UPROPERTY(Transient)
	UConversationBorder* ConversationBorderPtr = nullptr;

	int32 SentenceIndex = 0;
	
};
