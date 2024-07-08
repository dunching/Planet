// Copyright 2020 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"

#include "Abilities/GameplayAbility.h"

#include "CharacterAttibutes.h"

#include "GAOwnerTypeInterface.generated.h"

class USkeletalMeshComponent;
class UAbilitySystemComponent;

class UInteractiveBaseGAComponent;
class UInteractiveConsumablesComponent;
class UInteractiveSkillComponent;
class UInteractiveToolComponent;

struct FCharacterAttributes;

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UGAOwnerInterface : public UInterface
{
	GENERATED_BODY()
};

class ABILITYSYSTEM_API IGAOwnerInterface
{
	GENERATED_BODY()

public:

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const = 0;

	virtual USkeletalMeshComponent* GetMesh() const = 0;

	virtual FCharacterAttributes& GetCharacterAttributes() = 0;

	virtual UInteractiveBaseGAComponent* GetInteractiveBaseGAComponent() = 0;

	virtual UInteractiveConsumablesComponent* GetInteractiveConsumablesComponent() = 0;

	virtual UInteractiveSkillComponent* GetInteractiveSkillComponent() = 0;

	virtual UInteractiveToolComponent* GetInteractiveToolComponent() = 0;

protected:

private:

};
