#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"

#include "OnEffectedTawrgetCallback.h"

#include "ConversationComponent.generated.h"

class UPAD_TaskNode_Preset_Conversation;
class UConversationBorder;
class ACharacterBase;

/*
 * 角色的会话组件
 * 比如角色在念某一段台词是需要显示一个气泡和音频播放
 */
UCLASS(BlueprintType, Blueprintable)
class PLANET_API UConversationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	static FName ComponentName;

	using FOwnerType = ACharacterBase;

	UConversationComponent(const FObjectInitializer& ObjectInitializer);

#pragma region NPC弹出的对话
	UFUNCTION(NetMulticast, Reliable)
	virtual void DisplaySentence(
		const FTaskNode_Conversation_SentenceInfo&Sentence
		);

	UFUNCTION(NetMulticast, Reliable)
	void CloseConversationborder();
#pragma endregion 

#pragma region 玩家弹出的对话
	virtual void DisplaySentence_Player(
		const FTaskNode_Conversation_SentenceInfo&Sentence,
		const std::function<void()>&SentenceStop
		);

	virtual void CloseConversationborder_Player();
#pragma endregion 

protected:

};
