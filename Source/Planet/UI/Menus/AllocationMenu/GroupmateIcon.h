// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"

#include "UIInterfaces.h"
#include "Common/GenerateType.h"
#include "ItemProxy_Minimal.h"
#include "AllocationIconBase.h"

#include "GroupmateIcon.generated.h"

struct FStreamableHandle;
class UDragDropOperation;

struct FSkillProxy;
struct FCharacterProxy;
struct FAllocationbleProxy;

UCLASS()
class PLANET_API UGroupmateIcon :
	public UMyUserWidget,
	public IAllocationableProxyIconInterface
{
	GENERATED_BODY()

public:

	using FOnSelected = TCallbackHandleContainer<void(const TSharedPtr<FCharacterProxy>&)>;

	virtual void InvokeReset(UUserWidget* BaseWidgetPtr)override;

	virtual void ResetToolUIByData(const TSharedPtr<FAllocationbleProxy>& BasicProxyPtr)override;

	virtual void EnableIcon(bool bIsEnable)override;

	void SwitchSelectState(bool bIsSelect);

	FOnSelected OnSelected;

	TSharedPtr<FCharacterProxy> ProxyPtr = nullptr;

protected:

	virtual void NativeConstruct()override;

	void SetItemType();

	void SetName();

	UFUNCTION()
	void OnBtnCliked();

private:

};