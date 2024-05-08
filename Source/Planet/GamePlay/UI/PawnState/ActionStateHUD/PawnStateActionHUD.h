// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"

#include "CharacterAttributesComponent.h"
#include "UIInterfaces.h"

#include "PawnStateActionHUD.generated.h"

struct FSceneTool;
struct FCharacterAttributes;

class UState_Talent_NuQi;
class ACharacterBase;

UCLASS()
class PLANET_API UPawnStateActionHUD : public UUserWidget, public IItemsMenuInterface
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct()override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	virtual void ResetUIByData()override;

	ACharacterBase* CharacterPtr = nullptr;

protected:

	void InitialTalentUI();

	void InitialSkillIcon();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UState_Talent_NuQi>TalentState_NuQi_Class;

private:

};
