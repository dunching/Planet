// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"

#include "GenerateType.h"
#include "HUDInterface.h"
#include "LayoutInterfacetion.h"


#include "InteractionConversationLayout.generated.h"

class UToolsMenu;
class UOptionList;
class AGuideInteraction_Actor;
class ISceneActorInteractionInterface;



struct FToolsSocketInfo;
struct FConsumableSocketInfo;

/**
 *
 */
UCLASS()
class PLANET_API UInteractionConversationLayout :
	public UMyUserWidget,
	public ILayoutInterfacetion
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void Enable() override;

	virtual ELayoutCommon GetLayoutType() const  override final;

private:
	
	UFUNCTION()
	void OnQuitBtnClicked();
	
};
