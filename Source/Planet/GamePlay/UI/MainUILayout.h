// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"

#include "MainUILAyout.generated.h"

/**
 *
 */
UCLASS()
class PLANET_API UMainUILayout : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName PawnActionStateHUDSocket;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName PawnBuildingStateHUDSocket;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName RightBackpackSocket;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName AllocationSkillsSocket;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName GroupMatesManaggerSocket;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName TalentAllocationSocket;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName EffectsListSocket;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName ProgressTipsSocket;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName HUD_TeamSocket;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	UWidget* Ttt = nullptr;

};
