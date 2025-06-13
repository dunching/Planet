// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UserWidget_Override.h"

#include "CharacterAttributesComponent.h"
#include "UIInterfaces.h"

#include "PropertyEntryDescription.generated.h"

struct FStreamableHandle;

struct FBasicProxy;
struct FCharacterProxy;
struct FTableRow_Regions;

class UTextBlock;
class UWidgetSwitcher;

/**
 * 进入区域的提示
 */
UCLASS()
class PLANET_API UPropertyEntryDescription :
	public UUserWidget_Override
{
	GENERATED_BODY()

public:

	void SetDta(const FGeneratedPropertyEntryInfo&GeneratedPropertyEntryInfo);

protected:
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* PropertyName = nullptr;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Value = nullptr;
	
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* WidgetSwitcher = nullptr;
	
};
