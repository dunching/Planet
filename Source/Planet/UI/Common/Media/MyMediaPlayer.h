// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UserWidget_Override.h"

#include "MyMediaPlayer.generated.h"

class UFileMediaSource;
class UMediaPlayer;

UCLASS()
class PLANET_API UMyMediaPlayer :
	public UUserWidget_Override
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void DisplayVideo();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UFileMediaSource> FileMediaSourceRef;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMediaPlayer* MediaPlayerPtr = nullptr;
};
