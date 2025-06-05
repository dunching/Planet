// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SceneActorInteractionInterface.h"

#include "ToolProxyBase.h"

#include "Planet_Consumable_Base.generated.h"

class USkill_Consumable_Base;

UCLASS()
class PLANET_API APlanet_Consumable_Base : public AToolProxyBase,
	public ISceneActorInteractionInterface
{
	GENERATED_BODY()

public:

	virtual USceneActorInteractionComponent* GetSceneActorInteractionComponent() const override;

	virtual void HasbeenInteracted(
		ACharacterBase* CharacterPtr
		) override;

	virtual void HasBeenStartedLookAt(
		ACharacterBase* CharacterPtr
		) override;

	virtual void HasBeenLookingAt(
		ACharacterBase* CharacterPtr
		) override;

	virtual void HasBeenEndedLookAt() override;

};
