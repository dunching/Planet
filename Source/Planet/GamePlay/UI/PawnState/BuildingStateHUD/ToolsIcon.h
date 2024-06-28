// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include <GameplayTagContainer.h>

#include "UIInterfaces.h"
#include "GenerateType.h"

#include "SceneElement.h"

#include "ToolsIcon.generated.h"

class UBasicUnit;
class UToolUnit;

struct FStreamableHandle;

UCLASS()
class PLANET_API UToolIcon : public UUserWidget, public IToolsIconInterface
{
	GENERATED_BODY()

public:

	using FOnResetUnit = TCallbackHandleContainer<void(UWeaponUnit*)>;

	UToolIcon(const FObjectInitializer& ObjectInitializer);

	virtual void InvokeReset(UUserWidget* BaseWidgetPtr)override;

	virtual void ResetToolUIByData(UBasicUnit * BasicUnitPtr)override;

	virtual void EnableIcon(bool bIsEnable)override;

	UToolUnit* GetToolUnit()const;

	void OnSublingIconReset(UToolUnit* InToolUnitPtr);

	FOnResetUnit OnResetUnit;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SkillSocket")
	FGameplayTag IconSocket;

protected:

	void SetNum();

	void SetItemType();

	void ResetSize(const FVector2D& Size);

	virtual void NativeConstruct()override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)override;

private:

	UToolUnit*ToolUnitPtr = nullptr;

	TSharedPtr<FStreamableHandle> AsyncLoadTextureHandle;

};
