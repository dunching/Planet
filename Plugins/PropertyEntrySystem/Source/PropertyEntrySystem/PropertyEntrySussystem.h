// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"
#include "PropertyEntrys.h"

#include "PropertyEntrySussystem.generated.h"

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UGetPropertyEntrysDTInterface : public UInterface
{
	GENERATED_BODY()
};

class PROPERTYENTRYSYSTEM_API IGetPropertyEntrysDTInterface
{
	GENERATED_BODY()

public:
	
	virtual TSoftObjectPtr<UDataTable>GetPropertyEntrysDT()const = 0;
	
protected:

private:

};

/**
 *	
 */
UCLASS()
class PROPERTYENTRYSYSTEM_API UPropertyEntrySussystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(
		UObject* Outer
	) const override;

	static UPropertyEntrySussystem* GetInstance();
	
	TArray<FGeneratedPropertyEntryInfo> GenerationPropertyEntry(
		const TArray<int32>& LevelAry
		);

private:

	void GetCheckMap();
	
	int32 CacheTime = 5;

	FTimerHandle TimerHandle;
	
	// 级数、属性、权重
	TMap<int32, TMap<FGameplayTag, FGeneratiblePropertyEntryInfo>> CheckMap;

};