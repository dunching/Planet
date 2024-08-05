// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"

#include "UIInterfaces.h"
#include "Common/GenerateType.h"
#include "SceneElement.h"
#include "AllocationIconBase.h"

#include "WeaponsIcon.generated.h"

struct FStreamableHandle;
class UDragDropOperation;

class USkillUnit;
class UWeaponUnit;
class UBasicUnit;

UCLASS()
class PLANET_API UWeaponsIcon : public UAllocationIconBase
{
	GENERATED_BODY()

public:

	using FOnResetUnit_Weapon = TCallbackHandleContainer<void(UWeaponUnit*)>;

	UWeaponsIcon(const FObjectInitializer& ObjectInitializer);

	virtual void ResetToolUIByData(UBasicUnit* BasicUnitPtr)override;

	virtual void EnableIcon(bool bIsEnable)override;

	UWeaponUnit* UnitPtr = nullptr;

	FOnResetUnit_Weapon OnResetUnit_Weapon;

protected:

	virtual void NativeConstruct()override;

private:

};