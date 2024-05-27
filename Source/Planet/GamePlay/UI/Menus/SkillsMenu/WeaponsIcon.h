// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"

#include "UIInterfaces.h"
#include "Common/GenerateType.h"

#include "SceneElement.h"

#include "WeaponsIcon.generated.h"

struct FStreamableHandle;

class UWeaponUnit;
class UBasicUnit;

UCLASS()
class PLANET_API UWeaponsIcon : public UUserWidget, public IToolsIconInterface
{
	GENERATED_BODY()

public:

	using FCallbackHandleContainer = TCallbackHandleContainer<void(UWeaponUnit*)>;

	UWeaponsIcon(const FObjectInitializer& ObjectInitializer);

	virtual void InvokeReset(UUserWidget* BaseWidgetPtr)override;

	virtual void ResetToolUIByData(UBasicUnit* BasicUnitPtr)override;

	FCallbackHandleContainer OnDroped;

	UWeaponUnit* WeaponUnitPtr = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SkillSocket")
	FGameplayTag IconSocket;

	bool bIsInBackpakc = false;

protected:

	void SetItemType();

	virtual void NativeConstruct()override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)override;

	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)override;

private:

	TSharedPtr<FStreamableHandle> AsyncLoadTextureHandle;

};