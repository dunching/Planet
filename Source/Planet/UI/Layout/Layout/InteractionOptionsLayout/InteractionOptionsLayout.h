// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UserWidget_Override.h"

#include "GenerateTypes.h"
#include "HUDInterface.h"
#include "LayoutInterfacetion.h"


#include "InteractionOptionsLayout.generated.h"

class UToolsMenu;
class UOptionList;
class AQuestInteractionBase;
class ISceneActorInteractionInterface;



struct FToolsSocketInfo;
struct FConsumableSocketInfo;

/**
 *
 */
UCLASS()
class PLANET_API UInteractionOptionsLayout :
	public UUserWidget_Override,
	public ILayoutInterfacetion
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void Enable() override;

	virtual void DisEnable() override;

	virtual ELayoutCommon GetLayoutType() const  override final;
	
	UOptionList* GetOptions() const;

	void CloseOption();

protected:
	void SelectedInteractionItem(const TSubclassOf<AQuestInteractionBase>&GuideInteractionClass);

	UFUNCTION()
	void OnQuitBtnClicked();
	
	/**
	 * 交互的对象
	 */
	ISceneActorInteractionInterface* SceneActorInteractionInterfacePtr = nullptr;
		
	/**
	 * 交互的节点
	 */
	TObjectPtr<AQuestInteractionBase>GuideInteractionActorPtr = nullptr;
	
};
