// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"

#include "GenerateType.h"
#include "HUDInterface.h"
#include "LayoutInterfacetion.h"


#include "ConversationLayout.generated.h"

class UToolsMenu;
class UOptionList;
class AGuideInteractionActor;

struct FCharacterAttributes;

struct FToolsSocketInfo;
struct FConsumableSocketInfo;

/**
 *
 */
UCLASS()
class PLANET_API UConversationLayout :
	public UMyUserWidget,
	public ILayoutInterfacetion
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void Enable() override;

	virtual void DisEnable() override;

	UOptionList* GetOptions() const;

	void CloseOption();

protected:
	void SelectedInteractionItem(const TSubclassOf<AGuideInteractionActor>&GuideInteractionClass);
	
	AGuideInteractionActor*GuideInteractionActorPtr = nullptr;
	
};
