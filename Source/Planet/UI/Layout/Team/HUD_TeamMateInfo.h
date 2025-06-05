// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UserWidget_Override.h"

#include "CharacterAttributesComponent.h"
#include "UIInterfaces.h"

#include "HUD_TeamMateInfo.generated.h"

struct FStreamableHandle;

struct FBasicProxy;
struct FCharacterProxy;

UCLASS()
class PLANET_API UHUD_TeamMateInfo :
	public UUserWidget_Override,
	public IItemProxyIconInterface
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct()override;

	virtual void ResetToolUIByData(const TSharedPtr<FBasicProxy>& BasicProxyPtr)override;

	virtual void EnableIcon(bool bIsEnable)override;

protected:

	virtual void InvokeReset(UUserWidget* BaseWidgetPtr)override;

private:

	TSharedPtr<FCharacterProxy> GroupMateProxyPtr = nullptr;

};
