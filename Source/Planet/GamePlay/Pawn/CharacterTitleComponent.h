#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Components/WidgetComponent.h"

#include "ReceivedEventModifyDataCallback.h"
#include "MyUserWidget.h"
#include "GenerateType.h"
#include "GroupManaggerInterface.h"
#include "TaskNode.h"

#include "CharacterTitleComponent.generated.h"

class UPAD_TaskNode_Preset_Conversation;
class UConversationBorder;
class UCharacterTitle;
class ACharacterBase;
class AGroupManagger;

UCLASS()
class PLANET_API UCharacterTitleBox :
	public UMyUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;

	virtual void NativeConstruct() override;

	void SetData(ACharacterBase* CharacterPtr);
};

/*
 * 角色的抬头UI，如血条
 */
UCLASS(BlueprintType, Blueprintable)
class PLANET_API UCharacterTitleComponent :
	public UWidgetComponent,
	public IGroupManaggerInterface
{
	GENERATED_BODY()

public:
	static FName ComponentName;

	using FOwnerType = ACharacterBase;

	UCharacterTitleComponent(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void TickComponent(
		float DeltaTime,
		enum ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	) override;

	virtual void OnGroupManaggerReady(AGroupManagger* NewGroupSharedInfoPtr) override;

	void SetCampType(ECharacterCampType CharacterCampType);

	virtual void DisplaySentence(
		const FTaskNode_Conversation_SentenceInfo& Sentence
	);

	void CloseConversationborder();

protected:
	bool ResetPosition(float InDeltaTime);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Offset = 0.f;

	float HalfHeight = 0.f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI")
	TSubclassOf<UCharacterTitle> CharacterTitleClass;

	UPROPERTY(Transient)
	UCharacterTitle* CharacterTitlePtr = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI")
	TSubclassOf<UConversationBorder> ConversationBorderClass;

	UPROPERTY(Transient)
	UConversationBorder* ConversationBorderPtr = nullptr;
};
