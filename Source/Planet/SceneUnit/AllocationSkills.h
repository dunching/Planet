// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include <GameplayTagContainer.h>

#include "GenerateType.h"
#include "BaseData.h"
#include "GAEvent_Helper.h"

#include "AllocationSkills.generated.h"

struct FSkillProxy;
struct FWeaponProxy;
struct FConsumableProxy;

struct FSocketBase
{
	FKey Key;

	FGameplayTag Socket;
};

struct FSkillSocket : public FSocketBase
{
	TWeakPtr<FSkillProxy> UnitPtr = nullptr;
};

struct FWeaponSocket : public FSocketBase
{
	TWeakPtr<FWeaponProxy> UnitPtr = nullptr;
};

struct FConsumableSocket : public FSocketBase
{
	TWeakPtr<FConsumableProxy> UnitPtr = nullptr;
};

USTRUCT(BlueprintType)
struct PLANET_API FAllocationSkills final
{
	GENERATED_USTRUCT_BODY()

	void Update(const TSharedPtr<FSkillSocket>& Socket);
	
	void Update(const TSharedPtr<FWeaponSocket>& Socket);

	void Update(const TSharedPtr<FConsumableSocket>& Socket);

	bool Active(const TSharedPtr<FSocketBase>& Socket);

	void Cancel(const TSharedPtr<FSocketBase>& Socket);

	TSharedPtr<FSkillSocket> FindSkill(const FGameplayTag& Socket);
	
	TSharedPtr<FConsumableSocket> FindConsumable(const FGameplayTag& Socket);

	TMap<FGameplayTag, TSharedPtr<FSkillSocket>>GetSkillsMap()const;
	
	TMap<FGameplayTag, TSharedPtr<FWeaponSocket>>GetWeaponsMap()const;

	TMap<FGameplayTag, TSharedPtr<FConsumableSocket>>GetConsumablesMap()const;

private:

	TMap<FGameplayTag, TSharedPtr<FSkillSocket>>SkillsMap;

	TMap<FGameplayTag, TSharedPtr<FWeaponSocket>>WeaponsMap;
	
	TMap<FGameplayTag, TSharedPtr<FConsumableSocket>>ConsumablesMap;

};