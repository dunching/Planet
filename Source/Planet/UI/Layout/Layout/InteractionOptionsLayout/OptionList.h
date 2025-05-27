// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UserWidget_Override.h"

#include "CharacterAttributesComponent.h"
#include "HUDInterface.h"
#include "LayoutInterfacetion.h"

#include "OptionList.generated.h"

class AGuideActor;
class AGuideInteraction_Actor;
class UOptionItem;
class ACharacterBase;
class AHumanCharacter_AI;
class UPAD_TaskNode_Interaction_Option;
class ISceneActorInteractionInterface;

UCLASS()
class PLANET_API UOptionList :
	public UUserWidget_Override,
	public ILayoutItemInterfacetion
{
	GENERATED_BODY()

public:
	
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	virtual void Enable() override;
	
	virtual void DisEnable() override;

	// 显示AI可以互动的节点
	void UpdateDisplay(
		AHumanCharacter_AI* InTargetCharacterPtr,
		const std::function<void(const TSubclassOf<AGuideInteraction_Actor>&)>& InCallback
		);

	void UpdateDisplay(
		ISceneActorInteractionInterface*SceneActorInteractionInterfacePtr,
		const std::function<void(const TSubclassOf<AGuideInteraction_Actor>&)>& InCallback
		);

	void UpdateDisplay(
		const TArray<FString>&OptionAry,
		const std::function<void(int32)>& InCallback
		);
	
	void CloseUI();

protected:
	
	bool ResetPosition(float InDeltaTime);

	AHumanCharacter_AI* TargetCharacterPtr = nullptr;

	ISceneActorInteractionInterface* SceneActorInteractionInterfacePtr = nullptr;
		
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Item Class")
	TSubclassOf<UOptionItem>InteractionItemClass;

	FTSTicker::FDelegateHandle TickDelegateHandle;

};
