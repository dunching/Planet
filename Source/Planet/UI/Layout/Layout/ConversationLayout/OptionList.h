// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"

#include "CharacterAttributesComponent.h"
#include "HUDInterface.h"

#include "OptionList.generated.h"

class AGuideActor;
class AGuideInteractionActor;
class UOptionItem;
class ACharacterBase;
class AHumanCharacter_AI;
class UPAD_TaskNode_Interaction_Option;

UCLASS()
class PLANET_API UOptionList :
	public UMyUserWidget,
	public IHUDInterface
{
	GENERATED_BODY()

public:
	
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	virtual void ResetUIByData() override;

	// 显示AI可以互动的节点
	void UpdateDisplay(
		AHumanCharacter_AI* InTargetCharacterPtr,
		const std::function<void(const TSubclassOf<AGuideInteractionActor>&)>& InCallback
		);

	// 显示选项
	void UpdateDisplay(
		const TSoftObjectPtr<UPAD_TaskNode_Interaction_Option>&InTaskNodeRef,
		const std::function<void(int32)>& InCallback
		);
	
	void UpdateDisplay(
		const TArray<FString>&OptionAry,
		const std::function<void(int32)>& InCallback
		);
	
	void CloseUI();

protected:
	
	bool ResetPosition(float InDeltaTime);

	AHumanCharacter_AI* TargetCharacterPtr = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Item Class")
	TSubclassOf<UOptionItem>InteractionItemClass;

	FTSTicker::FDelegateHandle TickDelegateHandle;

};
