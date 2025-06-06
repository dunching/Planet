// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"

#include "UIInterfaces.h"
#include "GenerateTypes.h"
#include "ItemProxy_Minimal.h"
#include "AllocationIconBase.h"

#include "WeaponsIcon.generated.h"

struct FStreamableHandle;
class UDragDropOperation;

struct FSkillProxy;
struct FWeaponProxy;
struct IProxy_Allocationble;

UCLASS()
class PLANET_API UWeaponsIcon : public UAllocationIconBase
{
	GENERATED_BODY()

public:

	using FOnResetProxy_Weapon = TCallbackHandleContainer<void(const TSharedPtr<FWeaponProxy>&)>;

	UWeaponsIcon(const FObjectInitializer& ObjectInitializer);

	virtual void ResetToolUIByData(const TSharedPtr<FBasicProxy>& BasicProxyPtr)override;

	virtual void EnableIcon(bool bIsEnable)override;

	TSharedPtr<FWeaponProxy> WeaponProxyPtr = nullptr;

	FOnResetProxy_Weapon OnResetProxy_Weapon;

protected:

	virtual void NativeConstruct()override;

private:

};