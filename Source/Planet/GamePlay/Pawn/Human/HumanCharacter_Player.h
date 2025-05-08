// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ConversationComponent.h"
#include "HumanCharacter.h"
#include "SceneActorInteractionComponent.h"
#include "StateProcessorComponent.h"

#include "HumanCharacter_Player.generated.h"

// 是否开启、语句内容
using FOnPlayerHaveNewSentence = TMulticastDelegate<void(
	bool,
	const FTaskNode_Conversation_SentenceInfo&
)>;

using FOnPlayerInteraction = TMulticastDelegate<void(
	ISceneActorInteractionInterface*
)>;

class UPlayerComponent;
class AHumanCharacter_AI;
class USpringArmComponent;
class UCameraComponent;

class UInteractionList;
class AHumanCharacter_Player;

/**
 *
 */
UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class PLANET_API USceneCharacterPlayerInteractionComponent : public USceneActorInteractionComponent
{
	GENERATED_BODY()

public:
	FOnPlayerInteraction OnPlayerInteraction;
};

/**
 *
 */
UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class PLANET_API UCharacterPlayerStateProcessorComponent : public UStateProcessorComponent
{
	GENERATED_BODY()

public:
	using FOwnerType = AHumanCharacter_Player;

	/**
	 * 锁定敌人
	 */
	virtual void FocusTarget();

	void SetFocusCharactersAry(
		ACharacterBase* TargetCharacterPtr
	);

	void ClearFocusCharactersAry();

private:
	UFUNCTION(Server, Reliable)
	void SetFocusCharactersAry_Server(
		ACharacterBase* TargetCharacterPtr
	);

	UFUNCTION(Server, Reliable)
	void ClearFocusCharactersAry_Server();
	
	UFUNCTION()
	void OnFocusCharacterDestroyed(AActor* DestroyedActor);

	virtual void OnGameplayEffectTagCountChanged(const FGameplayTag Tag, int32 Count) override;

	FDelegateHandle OnGameplayEffectTagCountChangedHandle;

	TObjectPtr<ACharacterBase>PreviousFocusCharactersPtr = nullptr;
};

/* 角色的会话组件
 * 比如角色在念某一段台词是需要显示一个气泡和音频播放
 */
UCLASS(BlueprintType, Blueprintable)
class PLANET_API UPlayerConversationComponent : public UConversationComponent
{
	GENERATED_BODY()

public:
	virtual void DisplaySentence_Player(
		const FTaskNode_Conversation_SentenceInfo& Sentence,
		const std::function<void()>& SentenceStop
	) override;

	virtual void CloseConversationborder_Player() override;

	FOnPlayerHaveNewSentence OnPlayerHaveNewSentence;

	/**
	 * 但规矩播放完成
	 */
	std::function<void()> SentenceStop;

protected:
};

UCLASS()
class PLANET_API AHumanCharacter_Player : public AHumanCharacter
{
	GENERATED_BODY()

public:
	AHumanCharacter_Player(
		const FObjectInitializer& ObjectInitializer
	);

	UCameraComponent* GetCameraComp();;

	USpringArmComponent* GetCameraBoom();;

	UPlayerConversationComponent* GetPlayerConversationComponent() const;

	USceneCharacterPlayerInteractionComponent* GetSceneCharacterPlayerInteractionComponent() const;

	UCharacterPlayerStateProcessorComponent* GetCharacterPlayerStateProcessorComponent() const;

	void UpdateSightActor();

	virtual TPair<FVector, FVector> GetCharacterViewInfo();

	/**
	 * 与场景中的可交互对象开始交互
	 * @param SceneObjPtr 
	 */
	virtual bool InteractionSceneActor(
		ASceneActor* SceneObjPtr
	);

	virtual bool InteractionSceneCharacter(
		AHumanCharacter_AI* CharacterPtr
	);

	virtual void StartLookAt(
		ISceneActorInteractionInterface* SceneActorInteractionInterfacePtr
	);

	virtual void LookingAt(
		ISceneActorInteractionInterface* SceneActorInteractionInterfacePtr
	);

	virtual void EndLookAt();

	ISceneActorInteractionInterface* LookAtSceneActorPtr = nullptr;

protected:
	virtual void BeginPlay() override;

	virtual void PossessedBy(
		AController* NewController
	) override;

	virtual void OnRep_Controller() override;

	virtual void UnPossessed() override;

	virtual bool TeleportTo(
		const FVector& DestLocation,
		const FRotator& DestRotation,
		bool bIsATest = false,
		bool bNoCheck = false
	) override;

	virtual void SetupPlayerInputComponent(
		UInputComponent* PlayerInputComponent
	) override;

	virtual void OnRep_GroupSharedInfoChanged() override;

	virtual void OnGroupManaggerReady(
		AGroupManagger* NewGroupSharedInfoPtr
	) override;

	UFUNCTION(Server, Reliable)
	virtual void InteractionSceneObj_Server(
		ASceneActor* SceneObjPtr
	);

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
