// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ToolProxyBase.h"
#include "Weapon_Base.h"

#include "PlanetWeapon_Base.generated.h"

class AHumanCharacter;
struct FWeaponProxy;

UCLASS()
class PLANET_API APlanetWeapon_Base : public AWeapon_Base
{
	GENERATED_BODY()

public:
	
	void SetWeaponProxy(const FGuid& WeaponProxy_ID);

	TSharedPtr<FWeaponProxy>GetWeaponProxy()const;
	
protected:

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason)override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void AttachToCharacter(ACharacter* CharacterPtr)override;

	virtual void AttachToCharacterBase(ACharacterBase* CharacterPtr);

	// UFUNCTION(NetMulticast, Reliable)
	void SetWeaponProxyImp(ACharacterBase* AllocationCharacterPtr);

	TSharedPtr<FWeaponProxy> WeaponProxyPtr = nullptr;

};
