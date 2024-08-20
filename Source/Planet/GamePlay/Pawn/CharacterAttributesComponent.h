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

	using FOwnerType = IPlanetControllerInterface;

	using FPawnType = ACharacterBase;

	UCharacterAttributesComponent(const FObjectInitializer& ObjectInitializer);

	static FName ComponentName;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)override;

	const FCharacterAttributes& GetCharacterAttributes()const;

	FCharacterAttributes& GetCharacterAttributes();

	void ProcessCharacterAttributes();

protected:

	virtual void BeginPlay()override;

private:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FCharacterAttributes CharacterAttributes;

};