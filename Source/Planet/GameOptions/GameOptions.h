// Zowee. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include <GenerateType.h>

#include "GameplayTagContainer.h"
#include "SceneElement.h"

#include "GameOptions.generated.h"

UCLASS(BlueprintType, Blueprintable)
class PLANET_API UGameOptions : public UObject
{
	GENERATED_BODY()
public:

	static UGameOptions* GetInstance();
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "ActiveSkill")
	int32 ResetCooldownTime = 1;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "MoveToAttaclArea")
	int32 MoveToAttaclAreaOffset = 50;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "MoveToAttaclArea")
	bool bIsAllowAdjustTime = true;

};
