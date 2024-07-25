// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"

#include "MainUILAyout.generated.h"

/**
 *
 */
UCLASS()
class PLANET_API UMainUILayout : public UMyUserWidget
{
	GENERATED_BODY()

public:

#if WITH_EDITOR
	virtual void NativeConstruct()override;
#else
	virtual void NativePreConstruct()override;
#endif

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
	
};
