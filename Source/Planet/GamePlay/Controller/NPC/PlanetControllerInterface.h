 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "PlanetControllerInterface.generated.h"

class AGroupManagger;
class UPlanetAbilitySystemComponent;
class UInventoryComponent;
class UCharacterAttributesComponent;
class UTalentAllocationComponent;
struct FCharacterProxy;
class AHumanCharacter;
class ACharacterBase;

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UPlanetControllerInterface : public UInterface
{
	GENERATED_BODY()

public:

};

class PLANET_API IPlanetControllerInterface
{
	GENERATED_BODY()

public:

	virtual UPlanetAbilitySystemComponent* GetAbilitySystemComponent() const = 0;

	// “成员管理”记录了成员信息，因为
	virtual AGroupManagger* GetGroupManagger() const = 0;

	virtual void SetGroupSharedInfo(AGroupManagger*GroupManaggerPtr) = 0;

	virtual UInventoryComponent* GetHoldingItemsComponent()const = 0;

	virtual UCharacterAttributesComponent* GetCharacterAttributesComponent()const = 0;

	virtual UTalentAllocationComponent* GetTalentAllocationComponent()const = 0;

	virtual TSharedPtr<FCharacterProxy> GetCharacterProxy() = 0;

	virtual ACharacterBase* GetRealCharacter()const = 0;

	virtual void ResetGroupmateProxy(FCharacterProxy* NewGourpMateProxyPtr) = 0;

	// 我们设定每个PC只对应一个Character，
	// 如：Player在骑乘“马”时，PC会控制到“马”Character，
	// 但是我们会在PC上记录我们默认的控制
	virtual void BindPCWithCharacter() = 0;

protected:

	virtual TSharedPtr<FCharacterProxy> InitialCharacterProxy(ACharacterBase * CharaterPtr) = 0;

private:

	int32 Value = 0;

};
