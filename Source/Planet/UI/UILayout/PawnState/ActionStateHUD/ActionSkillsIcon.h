// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include <Blueprint/IUserObjectListEntry.h>

#include "UIInterfaces.h"
#include "GenerateType.h"

#include "ItemProxy_Minimal.h"

#include "ActionSkillsIcon.generated.h"

struct FStreamableHandle;

struct FSkillProxy;

/**
 *
 */
UCLASS()
class PLANET_API UActionSkillsIcon : 
	public UMyUserWidget,
	public IUnitIconInterface,
	public IUserObjectListEntry
{
	GENERATED_BODY()

public:

	UActionSkillsIcon(const FObjectInitializer& ObjectInitializer);

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject)override;

	virtual void InvokeReset(UUserWidget* BaseWidgetPtr)override;

	virtual void ResetToolUIByData(const TSharedPtr<FBasicProxy>& BasicUnitPtr)override;

	virtual void EnableIcon(bool bIsEnable)override;

	void UpdateSkillState();

	TSharedPtr<FSkillProxy> UnitPtr = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SkillSocket")
	FGameplayTag IconSocket;

protected:

	void UpdateSkillState_ActiveSkill();

	void UpdateSkillState_ActiveWeapon();

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

	void SetNum(bool bIsDisplay, int32 Num);

	void SetInputRemainPercent(bool bIsAcceptInput, float Percent);

	void SetDurationPercent(bool bIsHaveDuration, float Percent);

	virtual void NativeConstruct()override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)override;

	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)override;

	UFUNCTION(BlueprintImplementableEvent)
	void PlaySkillIsReady();

private:

	bool bIsReady_Previous = false;

};