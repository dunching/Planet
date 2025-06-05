// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "Components/RichTextBlock.h"

#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "Fonts/SlateFontInfo.h"
#include "Styling/SlateTypes.h"
#include "Framework/Text/TextLayout.h"
#include "Framework/Text/ISlateRun.h"
#include "Framework/Text/ITextDecorator.h"
#include "Components/RichTextBlockDecorator.h"
#include "Engine/DataTable.h"

#include "PlanetRichTextBlock.generated.h"

UCLASS(Blueprintable, BlueprintType)
class COMMON_UMG_API URichTextBlock_Button : public URichTextBlock
{
	GENERATED_BODY()

public:
	void OnBtnClicked(
		FString Text
	);

	void OnBtnHovered(
		FString Text
	);

	TMulticastDelegate<void(
		const FString&
	)> OnClickedDelegate;

	TMulticastDelegate<void(
		const FString&
	)> OnHoveredDelegate;

protected:
};

UCLASS()
class COMMON_UMG_API URichTextBlock_Button_Helper : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

	void SetText(const FString& Text);
	
	UFUNCTION(BlueprintCallable)
	void OnClicked();

	UFUNCTION(BlueprintCallable)
	void OnHovered();

	TMulticastDelegate<void()> OnClickedDelegate;

	TMulticastDelegate<void()> OnHoveredDelegate;

protected:
};

UCLASS(Abstract, Blueprintable)
class COMMON_UMG_API URichTextBlockButtonDecorator : public URichTextBlockDecorator
{
	GENERATED_BODY()

public:
	URichTextBlockButtonDecorator(
		const FObjectInitializer& ObjectInitializer
	);

	virtual TSharedPtr<ITextDecorator> CreateDecorator(
		URichTextBlock* InOwner
	) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<URichTextBlock_Button_Helper> UserWidgetClass;

protected:
};

const FString RichText_Emphasis = TEXT("RichText.Emphasis");

const FString RichText_Key = TEXT("RichText.Key");
