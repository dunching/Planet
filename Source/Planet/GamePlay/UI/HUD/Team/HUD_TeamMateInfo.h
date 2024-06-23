// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"

#include "CharacterAttributesComponent.h"
#include "UIInterfaces.h"

#include "HUD_TeamMateInfo.generated.h"

struct FStreamableHandle;

class UBasicUnit;
class UGourpmateUnit;

UCLASS()
class PLANET_API UHUD_TeamMateInfo : public UUserWidget, public IToolsIconInterface
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct()override;

	virtual void ResetToolUIByData(UBasicUnit* BasicUnitPtr)override;

	virtual void EnableIcon(bool bIsEnable)override;

protected:

	virtual void InvokeReset(UUserWidget* BaseWidgetPtr)override;

private:

	UGourpmateUnit* GroupMateUnitPtr = nullptr;

	TSharedPtr<FStreamableHandle> AsyncLoadTextureHandle;

};
