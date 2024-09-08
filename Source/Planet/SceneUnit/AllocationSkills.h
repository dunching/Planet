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

USTRUCT(BlueprintType)
struct FSocketBase
{
	GENERATED_USTRUCT_BODY()

	virtual ~FSocketBase();

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	FKey Key;

	FGameplayTag Socket;

};

template<>
struct TStructOpsTypeTraits<FSocketBase> :
	public TStructOpsTypeTraitsBase2<FSocketBase>
{
	enum
	{
		WithNetSerializer = true,
	};
};

USTRUCT(BlueprintType)
struct FSkillSocket : public FSocketBase
{
	GENERATED_USTRUCT_BODY()

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)override;

	TWeakPtr<FSkillProxy> UnitPtr = nullptr;
};

template<>
struct TStructOpsTypeTraits<FSkillSocket> :
	public TStructOpsTypeTraitsBase2<FSkillSocket>
{
	enum
	{
		WithNetSerializer = true,
	};
};

USTRUCT(BlueprintType)
struct FWeaponSocket : public FSocketBase
{
	GENERATED_USTRUCT_BODY()

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)override;

	TWeakPtr<FWeaponProxy> UnitPtr = nullptr;
};

template<>
struct TStructOpsTypeTraits<FWeaponSocket> :
	public TStructOpsTypeTraitsBase2<FWeaponSocket>
{
	enum
	{
		WithNetSerializer = true,
	};
};

USTRUCT(BlueprintType)
struct FConsumableSocket : public FSocketBase
{
	GENERATED_USTRUCT_BODY()

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

	bool Active(const FGameplayTag& Socket);

	void Cancel(const TSharedPtr<FSocketBase>& Socket);

	void Cancel(const FGameplayTag& Socket);

	TSharedPtr<FSkillSocket> FindSkill(const FGameplayTag& Socket);
	
	TSharedPtr<FWeaponSocket> FindWeapon(const FGameplayTag& Socket);
	
	TSharedPtr<FConsumableSocket> FindConsumable(const FGameplayTag& Socket);

	TMap<FGameplayTag, TSharedPtr<FSkillSocket>>GetSkillsMap()const;
	
	TMap<FGameplayTag, TSharedPtr<FWeaponSocket>>GetWeaponsMap()const;

	TMap<FGameplayTag, TSharedPtr<FConsumableSocket>>GetConsumablesMap()const;

	TSharedPtr<FCharacterProxy>OwnerCharacter = nullptr;

private:

	TMap<FGameplayTag, TSharedPtr<FSkillSocket>>SkillsMap;

	TMap<FGameplayTag, TSharedPtr<FWeaponSocket>>WeaponsMap;
	
	TMap<FGameplayTag, TSharedPtr<FConsumableSocket>>ConsumablesMap;

};