// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UIInterfaces.h"
#include "Common/GenerateType.h"

#include "SceneElement.h"

#include "SkillsIcon.generated.h"

struct FStreamableHandle;

class USkillUnit;

/**
 *
 */
UCLASS()
class PLANET_API USkillsIcon : public UUserWidget, public IToolsIconInterface
{
	GENERATED_BODY()

public:

	USkillsIcon(const FObjectInitializer& ObjectInitializer);

	virtual void InvokeReset(UUserWidget* BaseWidgetPtr)override;

	virtual void ResetToolUIByData(UBasicUnit* BasicUnitPtr)override;

	void UpdateSkillState();

	USkillUnit* SkillUnitPtr = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SkillSocket")
	FGameplayTag IconSocket;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SkillType")
	ESkillType SkillType = ESkillType::kActive;

	bool bIsInBackpakc = false;

protected:

	void SetLevel(int32 NewNum);

	void SetRemainingCooldown(
		bool bIsReady,
		bool bCooldownIsReady,
		float RemainingTime,
		float Percent
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

	TSharedPtr<FStreamableHandle> AsyncLoadTextureHandle;

};