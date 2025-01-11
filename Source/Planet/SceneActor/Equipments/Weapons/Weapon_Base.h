// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ToolProxyBase.h"
#include "PlanetGameplayAbility.h"
#include "Skill_Base.h"

#include "Weapon_Base.generated.h"

class AHumanCharacter;
struct FWeaponProxy;

UCLASS()
class PLANET_API AWeapon_Base : public AToolProxyBase
{
	GENERATED_BODY()

public:
	
	void SetWeaponProxy(const FGuid& WeaponProxy_ID);

	TSharedPtr<FWeaponProxy>GetWeaponProxy()const;
	
protected:

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason)override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void AttachToCharacter(ACharacterBase* CharacterPtr)override;

	UFUNCTION(NetMulticast, Reliable)
	void SetWeaponProxyImp(ACharacterBase* AllocationCharacterPtr);

	TSharedPtr<FWeaponProxy> WeaponProxyPtr = nullptr;

};
