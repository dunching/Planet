// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ToolProxyBase.h"

#include "Weapon_Base.generated.h"

class AHumanCharacter;
struct FWeaponProxy;

UCLASS()
class SCENEACTOR_API AWeapon_Base : public AToolProxyBase
{
	GENERATED_BODY()

public:
	
protected:

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason)override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void AttachToCharacter(ACharacter* CharacterPtr)override;

};
