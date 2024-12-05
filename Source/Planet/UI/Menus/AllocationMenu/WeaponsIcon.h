// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"

#include "UIInterfaces.h"
#include "Common/GenerateType.h"
#include "ItemProxy.h"
#include "AllocationIconBase.h"

#include "WeaponsIcon.generated.h"

struct FStreamableHandle;
class UDragDropOperation;

struct FSkillProxy;
struct FWeaponProxy;
struct FBasicProxy;

UCLASS()
class PLANET_API UWeaponsIcon : public UAllocationIconBase
{
	GENERATED_BODY()

public:

	using FOnResetUnit_Weapon = TCallbackHandleContainer<void(const TSharedPtr<FWeaponProxy>&)>;

	UWeaponsIcon(const FObjectInitializer& ObjectInitializer);

	virtual void ResetToolUIByData(const TSharedPtr<FBasicProxy>& BasicUnitPtr)override;

	virtual void EnableIcon(bool bIsEnable)override;

	TSharedPtr<FWeaponProxy> WeaponProxyPtr = nullptr;

	FOnResetUnit_Weapon OnResetUnit_Weapon;

protected:

	virtual void NativeConstruct()override;

private:

};