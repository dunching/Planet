// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UserWidget_Override.h"

#include "CharacterAttributesComponent.h"
#include "UIInterfaces.h"

#include "RegionPromt.generated.h"

struct FStreamableHandle;

struct FBasicProxy;
struct FCharacterProxy;
struct FTableRow_Regions;

/**
 * 进入区域的提示
 */
UCLASS()
class PLANET_API URegionPromt :
	public UUserWidget_Override
{
	GENERATED_BODY()

public:
	void SetRegionPromt(
		const FTableRow_Regions* TableRow_RegionsPtr
		);
};
