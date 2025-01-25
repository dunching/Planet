// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyUserWidget.h"

#include "GenerateType.h"
#include "MainMenuCommon.h"
#include "UICommon.h"

#include "MenuNavgationItem.generated.h"

class UTalentIcon;

/**
 *
 */
UCLASS()
class PLANET_API UMenuNavgationItem : public UMyUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct()override;

	UFUNCTION()
	void OnClicked();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMenuType MenuType = EMenuType::kAllocationSkill;

protected:

};
