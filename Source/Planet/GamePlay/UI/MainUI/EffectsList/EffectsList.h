// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"

#include "CharacterAttributesComponent.h"
#include "UIInterfaces.h"

#include "EffectsList.generated.h"

class UEffectItem;

UCLASS()
class PLANET_API UEffectsList : public UMyUserWidget, public IMenuInterface
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct()override;

	UEffectItem* AddEffectItem();

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Class")
	TSubclassOf<UEffectItem>EffectItemClass;

	virtual void ResetUIByData()override;

};
