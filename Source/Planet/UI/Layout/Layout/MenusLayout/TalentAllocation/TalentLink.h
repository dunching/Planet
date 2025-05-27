// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UIInterfaces.h"
#include "Talent_FASI.h"

#include "TalentLink.generated.h"

struct FStreamableHandle;

class UTalentIcon;

/**
 *
 */
UCLASS()
class PLANET_API UTalentLink : public UUserWidget_Override
{
	GENERATED_BODY()

public:

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UTalentIcon* PreConditionsPtr = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UTalentIcon* PostConditionPtr = nullptr;

};