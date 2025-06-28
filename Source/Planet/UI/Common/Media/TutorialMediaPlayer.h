// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyMediaPlayer.h"
#include "TemplateHelper.h"

#include "UserWidget_Override.h"

#include "TutorialMediaPlayer.generated.h"

class UFileMediaSource;
class UMediaPlayer;
class UButton;

UCLASS()
class PLANET_API UTutorialMediaPlayer :
	public UMyMediaPlayer
{
	GENERATED_BODY()

public:
	using FOnLayoutChanged = TCallbackHandleContainer<void(

		)>;

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;
	
	virtual FReply NativeOnKeyDown(
		const FGeometry& InGeometry,
		const FKeyEvent& InKeyEvent
		) override;

	void Display();

	void SetOnClicked(
		const std::function<void()>& InOnClicked
		);

	UFUNCTION()
	void OnConfirmBtnClicked();

	UPROPERTY(meta = (BindWidget))
	UButton* ConfirmBtn = nullptr;

	FOnLayoutChanged OnClicked;
};
