// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"

#include "UIInterfaces.h"
#include "Common/GenerateType.h"

#include "SceneElement.h"

#include "WeaponsIcon.generated.h"

struct FStreamableHandle;
class UDragDropOperation;

class USkillUnit;
class UWeaponUnit;
class UBasicUnit;

UCLASS()
class PLANET_API UWeaponsIcon : public UMyUserWidget, public IToolsIconInterface
{
	GENERATED_BODY()

public:

	using FOnResetUnit = TCallbackHandleContainer<void(UWeaponUnit*)>;

	UWeaponsIcon(const FObjectInitializer& ObjectInitializer);

	virtual void InvokeReset(UUserWidget* BaseWidgetPtr)override;

	virtual void ResetToolUIByData(UBasicUnit* BasicUnitPtr)override;

	virtual void EnableIcon(bool bIsEnable)override;

	void OnDragSkillIcon(bool bIsDragging, USkillUnit* SkillUnitPtr);

	void OnDragWeaponIcon(bool bIsDragging, UWeaponUnit* WeaponUnitPtr);

	void OnSublingIconReset(UWeaponUnit* InWeaponUnitPtr);

	FOnResetUnit OnResetUnit;

	UWeaponUnit* WeaponUnitPtr = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SkillSocket")
	FGameplayTag IconSocket;

protected:

	void SetItemType();

	virtual void NativeConstruct()override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)override;

private:

};