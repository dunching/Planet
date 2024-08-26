// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "GenerateType.h"
#include <SceneElement.h>
#include "CharacterAttibutes.h"

#include "CharacterAttributesComponent.generated.h"

class IPlanetControllerInterface;
class UGAEvent_Received;

UCLASS(BlueprintType, Blueprintable)
class PLANET_API UCharacterAttributesComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	using FOwnerType = ACharacterBase;

	using FPawnType = ACharacterBase;

	UCharacterAttributesComponent(const FObjectInitializer& ObjectInitializer);

	static FName ComponentName;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)override;

	TSharedPtr<FCharacterAttributes> GetCharacterAttributes()const;

	void ProcessCharacterAttributes();

protected:

	virtual void BeginPlay()override;

private:

};