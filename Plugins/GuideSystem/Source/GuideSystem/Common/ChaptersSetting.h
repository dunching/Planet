// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "ChaptersSetting.generated.h"

class AGuideThread_MainBase;

UCLASS(BlueprintType, Blueprintable)
class GUIDESYSTEM_API UChaptersSetting : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GuideThread")
	TArray<TSubclassOf<AGuideThread_MainBase>> MainGuideThreadChaptersAry;

};
