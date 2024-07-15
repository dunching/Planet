// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "UIInterfaces.h"
#include "GenerateType.h"

#include "SceneElement.h"

#include "ActionSkillsIcon.generated.h"

struct FStreamableHandle;

class USkillUnit;

/**
 *
 */
UCLASS()
class PLANET_API UActionSkillsIcon : public UMyUserWidget, public IToolsIconInterface
{
	GENERATED_BODY()

public:

	UActionSkillsIcon(const FObjectInitializer& ObjectInitializer);

	virtual void InvokeReset(UUserWidget* BaseWidgetPtr)override;

	virtual void ResetToolUIByData(UBasicUnit* BasicUnitPtr)override;

	virtual void EnableIcon(bool bIsEnable)override;

	void UpdateSkillState();

	USkillUnit* ToolPtr = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SkillSocket")
	FGameplayTag IconSocket;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SkillType")
	ESkillType SkillType = ESkillType::kActive;

protected:

	void SetLevel();

	void SetRemainingCooldown(
		bool bCooldownIsReady,
		float RemainingTime,
		float Percent
	);

	void SetCanRelease(
		bool bIsReady_In
	);

	void SetItemType();

	virtual void NativeConstruct()override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)override;

	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)override;

	UFUNCTION(BlueprintImplementableEvent)
	void PlaySkillIsReady();

private:

	bool bIsReady_Previous = false;

};