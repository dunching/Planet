// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"

#include "UIInterfaces.h"
#include "Common/GenerateType.h"
#include "SceneElement.h"
#include "AllocationIconBase.h"

#include "GroupmateIcon.generated.h"

struct FStreamableHandle;
class UDragDropOperation;

class USkillUnit;
class UCharacterUnit;
class UBasicUnit;

UCLASS()
class PLANET_API UGroupmateIcon :
	public UMyUserWidget,
	public IToolsIconInterface
{
	GENERATED_BODY()

public:

	using FOnSelected = TCallbackHandleContainer<void(UCharacterUnit*)>;

	virtual void InvokeReset(UUserWidget* BaseWidgetPtr)override;

	virtual void ResetToolUIByData(UBasicUnit* BasicUnitPtr)override;

	virtual void EnableIcon(bool bIsEnable)override;

	void SwitchSelectState(bool bIsSelect);

	FOnSelected OnSelected;

	UCharacterUnit* UnitPtr = nullptr;

protected:

	virtual void NativeConstruct()override;

	void SetItemType();

	void SetName();

	UFUNCTION()
	void OnBtnCliked();

private:

};