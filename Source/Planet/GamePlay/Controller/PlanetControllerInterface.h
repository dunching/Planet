// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "PlanetControllerInterface.generated.h"

class UGroupMnaggerComponent;
class UPlanetAbilitySystemComponent;
class UHoldingItemsComponent;
class UCharacterAttributesComponent;
class UTalentAllocationComponent;
class UCharacterUnit;
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

	// ����Ա������¼�˳�Ա��Ϣ����Ϊ
	virtual UGroupMnaggerComponent* GetGroupMnaggerComponent() const = 0;

	virtual UHoldingItemsComponent* GetHoldingItemsComponent()const = 0;

	virtual UCharacterAttributesComponent* GetCharacterAttributesComponent()const = 0;

	virtual UTalentAllocationComponent* GetTalentAllocationComponent()const = 0;

	virtual UCharacterUnit* GetGourpMateUnit() = 0;

	virtual ACharacterBase* GetRealCharacter()const = 0;

	virtual void ResetGroupmateUnit(UCharacterUnit* NewGourpMateUnitPtr) = 0;

	// �����趨ÿ��PCֻ��Ӧһ��Character��
	// �磺Player����ˡ���ʱ��PC����Ƶ�����Character��
	// �������ǻ���PC�ϼ�¼����Ĭ�ϵĿ���
	virtual void BindPCWithCharacter() = 0;

protected:

private:

};
