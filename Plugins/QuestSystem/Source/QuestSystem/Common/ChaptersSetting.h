// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "ChaptersSetting.generated.h"

class AQuestChain_MainBase;

UCLASS(BlueprintType, Blueprintable)
class QUESTSYSTEM_API UChaptersSetting : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GuideThread")
	TArray<TSubclassOf<AQuestChain_MainBase>> MainGuideThreadChaptersAry;

};
