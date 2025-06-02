// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "AssetRefBase.generated.h"

class USoundMix;
class USoundClass;
class UGameplayEffect;

UCLASS(BlueprintType, Blueprintable)
class ASSETREF_API UAssetRefBase : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:

#pragma region GEs
	/**
	 * 一次性
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GE")
	TSubclassOf<UGameplayEffect>OnceGEClass;

	/**
	 * 周期性
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GE")
	TSubclassOf<UGameplayEffect>DurationGEClass;

	/**
	 * 永久
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GE")
	TSubclassOf<UGameplayEffect>ForeverGEClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GE")
	TSubclassOf<UGameplayEffect>DamageCallbackClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GE")
	TSubclassOf<UGameplayEffect>SuppressClass;
#pragma endregion 
	
#pragma region Audios
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Audios")
	TSoftObjectPtr<USoundMix>SoundMixRef;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Audios")
	TSoftObjectPtr<USoundClass>BGMSoundClassRef;
#pragma endregion 
	
};
