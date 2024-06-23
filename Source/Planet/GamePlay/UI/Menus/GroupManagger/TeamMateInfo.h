// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UIInterfaces.h"
#include "Common/GenerateType.h"
#include "SceneElement.h"

#include "TeamMateInfo.generated.h"

struct FStreamableHandle;

class UBasicUnit;
class UGourpmateUnit;
class UTeamMatesList;

/**
 *
 */
UCLASS()
class PLANET_API UTeamMateInfo :
	public UUserWidget,
	public IToolsIconInterface
{
	GENERATED_BODY()

public:

	friend UTeamMatesList;

	using FCallbackHandleContainer = TCallbackHandleContainer<void(UTeamMateInfo*)>;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)override;

	virtual void InvokeReset(UUserWidget* BaseWidgetPtr)override;

	virtual void ResetToolUIByData(UBasicUnit* BasicUnitPtr)override;

	virtual void EnableIcon(bool bIsEnable)override;

	FCallbackHandleContainer OnDroped;

private:

	void AddMember();

	bool bIsInBackpakc = false;

	UGourpmateUnit* GroupMateUnitPtr = nullptr;

	TSharedPtr<FStreamableHandle> AsyncLoadTextureHandle;

};
