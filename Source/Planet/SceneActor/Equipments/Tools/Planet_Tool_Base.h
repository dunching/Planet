// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"

#include "ToolProxyBase.h"

#include "Planet_Tool_Base.generated.h"

UCLASS()
class PLANET_API APlanet_Tool_Base : public AToolProxyBase
{
	GENERATED_BODY()

public:

	virtual void AttachToCharacter(ACharacter* CharacterPtr)override;

protected:

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason)override;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TSubclassOf<UToolFuture_Base> EquipmentAbilities;

	FGameplayAbilitySpecHandle EquipmentAbilitieHandle;

};
