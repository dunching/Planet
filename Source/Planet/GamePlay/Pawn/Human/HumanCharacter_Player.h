// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ConversationComponent.h"
#include "HumanCharacter.h"

#include "HumanCharacter_Player.generated.h"

// 是否开启、语句内容
using FOnPlayerHaveNewSentence = TMulticastDelegate<void(bool, const FTaskNode_Conversation_SentenceInfo&)>;

class UPlayerComponent;
class USpringArmComponent;
class UCameraComponent;

class UInteractionList;

/* 角色的会话组件
 * 比如角色在念某一段台词是需要显示一个气泡和音频播放
 */
UCLASS(BlueprintType, Blueprintable)
class PLANET_API UPlayerConversationComponent : public UConversationComponent
{
	GENERATED_BODY()

public:
	virtual void DisplaySentence_Implementation(
		const FTaskNode_Conversation_SentenceInfo& Sentence
	) override;

	void CloseConversationborder_Implementation();

	FOnPlayerHaveNewSentence OnPlayerHaveNewSentence;
};

UCLASS()
class PLANET_API AHumanCharacter_Player : public AHumanCharacter
{
	GENERATED_BODY()

public:
	AHumanCharacter_Player(const FObjectInitializer& ObjectInitializer);

	virtual void InteractionSceneCharacter(AHumanCharacter_AI* CharacterPtr) override;

	UCameraComponent* GetCameraComp();;

	USpringArmComponent* GetCameraBoom();;

	UPlayerConversationComponent* GetPlayerConversationComponent() const;

	void UpdateSightActor();
	
	virtual TPair<FVector, FVector> GetCharacterViewInfo();

	virtual void StartLookAt(ISceneActorInteractionInterface* SceneActorInteractionInterfacePtr);

	virtual void LookingAt(ISceneActorInteractionInterface* SceneActorInteractionInterfacePtr);

	virtual void EndLookAt();

	ISceneActorInteractionInterface* LookAtSceneActorPtr = nullptr;

protected:
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void UnPossessed() override;

	virtual void OnRep_GroupSharedInfoChanged() override;

	virtual void OnGroupSharedInfoReady(AGroupSharedInfo* NewGroupSharedInfoPtr) override;

	virtual void InitialGroupSharedInfo();

#if WITH_EDITORONLY_DATA

#endif

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom = nullptr;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera = nullptr;

	UPROPERTY()
	TObjectPtr<UPlayerComponent> PlayerComponentPtr = nullptr;
	
	UPROPERTY(Transient)
	UInteractionList* InteractionListPtr = nullptr;
 	
};
