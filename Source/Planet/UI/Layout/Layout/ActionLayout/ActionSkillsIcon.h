// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include <Blueprint/IUserObjectListEntry.h>

#include "UIInterfaces.h"
#include "GenerateTypes.h"

#include "ItemProxy_Minimal.h"

#include "ActionSkillsIcon.generated.h"

struct FStreamableHandle;

struct FSkillProxy;

/**
 * 主动技能和武器的使用状态
 */
UCLASS()
class PLANET_API UActionSkillsIcon :
	public UUserWidget_Override,
	public IItemProxyIconInterface,
	public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	UActionSkillsIcon(
		const FObjectInitializer& ObjectInitializer
		);

	virtual void NativeOnListItemObjectSet(
		UObject* ListItemObject
		) override;

	virtual void InvokeReset(
		UUserWidget* BaseWidgetPtr
		) override;

	virtual void ResetToolUIByData(
		const TSharedPtr<FBasicProxy>& BasicProxyPtr
		) override;

	virtual void EnableIcon(
		bool bIsEnable
		) override;

	virtual void UpdateSkillState();

	TSharedPtr<FSkillProxy> ProxyPtr = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SkillSocket")
	FGameplayTag IconSocket;

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

	void SetNum(
		bool bIsDisplay,
		int32 Num
		);

	void SetInputRemainPercent(
		bool bIsAcceptInput,
		float Percent
		);

	void SetDurationPercent(
		bool bIsHaveDuration,
		float Percent
		);

	virtual void NativeConstruct() override;

	virtual FReply NativeOnMouseButtonDown(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent
		) override;

	virtual bool NativeOnDrop(
		const FGeometry& InGeometry,
		const FDragDropEvent& InDragDropEvent,
		UDragDropOperation* InOperation
		) override;

	virtual void NativeOnDragDetected(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent,
		UDragDropOperation*& OutOperation
		) override;

	UFUNCTION(BlueprintImplementableEvent)
	void PlaySkillIsReady();

	bool bIsReady_Previous = false;
};

UCLASS()
class PLANET_API UActionActiveSkillsIcon : public UActionSkillsIcon
{
	GENERATED_BODY()

public:
	virtual void ResetToolUIByData(
		const TSharedPtr<FBasicProxy>& BasicProxyPtr
		) override;

	virtual void UpdateSkillState() override;

private:
	void UpdateSkillState_ActiveSkill();
};

UCLASS()
class PLANET_API UActionWeaponSkillsIcon : public UActionSkillsIcon
{
	GENERATED_BODY()

public:
	virtual void ResetToolUIByData(
		const TSharedPtr<FBasicProxy>& BasicProxyPtr
		) override;

	virtual void UpdateSkillState() override;

private:
	void UpdateSkillState_ActiveWeapon();
};

UCLASS()
class PLANET_API UActionPassiveSkillsIcon : public UActionSkillsIcon
{
	GENERATED_BODY()

public:
	virtual void ResetToolUIByData(
		const TSharedPtr<FBasicProxy>& BasicProxyPtr
		) override;

	virtual void UpdateSkillState() override;

private:
	void UpdateSkillState_PassiveSkill();
};
