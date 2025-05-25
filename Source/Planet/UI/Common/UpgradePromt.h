// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"

#include "CharacterAttributesComponent.h"
#include "UIInterfaces.h"

#include "UpgradePromt.generated.h"

struct FStreamableHandle;

struct FBasicProxy;
struct FCharacterProxy;
struct FTableRow_Regions;

/**
 * 进入区域的提示
 */
UCLASS()
class PLANET_API UUpgradePromt :
	public UMyUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	void SetLevel(
		uint8 Level
		);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<USoundWave>UpgradePromtSoundRef = nullptr;
	
};
