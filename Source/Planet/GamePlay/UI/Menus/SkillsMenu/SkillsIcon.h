// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"

#include "UIInterfaces.h"
#include "GenerateType.h"
#include "SceneElement.h"

#include "SkillsIcon.generated.h"

struct FStreamableHandle;
class UDragDropOperation;

class USkillUnit;

/**
 * ��Ϊ����Icon
 * 1.ͬ�����Ҫ�ܻ����Ƴ� 2.�Ҽ��Ƴ� 3.��קʱ���ö�Ӧ��Icon
 */
UCLASS()
class PLANET_API USkillsIcon : public UUserWidget, public IToolsIconInterface
{
	GENERATED_BODY()

public:

	using FOnResetUnit = TCallbackHandleContainer<void(USkillUnit*)>;

	using FOnDragDelegate = TCallbackHandleContainer<void(bool, USkillUnit*)>;

	USkillsIcon(const FObjectInitializer& ObjectInitializer);

	virtual void InvokeReset(UUserWidget* BaseWidgetPtr)override;

	virtual void ResetToolUIByData(UBasicUnit* BasicUnitPtr)override;

	virtual void EnableIcon(bool bIsEnable)override;

	void OnDragSkillIcon(bool bIsDragging, USkillUnit* SkillUnitPtr);

	void OnDragWeaponIcon(bool bIsDragging, UWeaponUnit* WeaponUnitPtr);

	void OnSublingIconReset(USkillUnit* SkillUnitPtr);

	FOnResetUnit OnResetUnit;
	
	FOnDragDelegate OnDragDelegate;

	USkillUnit* SkillUnitPtr = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SkillSocket")
	FGameplayTag IconSocket;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SkillType")
	ESkillType SkillType = ESkillType::kActive;

	bool bIsInBackpakc = false;

protected:

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)override;

	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)override;

	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)override;

	UFUNCTION()
	void OnDroped(UDragDropOperation* Operation);

	UFUNCTION(BlueprintImplementableEvent)
	void PlaySkillIsReady();

	void SetLevel();

	void SetItemType();

private:

	bool bIsReady_Previous = false;

	TSharedPtr<FStreamableHandle> AsyncLoadTextureHandle;

};