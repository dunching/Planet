// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include <GenerateTypes.h>

#include "GameplayTagContainer.h"

#include "ItemProxy_Minimal.h"
#include "CharacterAttibutes.h"
#include "ItemProxy_Descriptions.h"

#include "SceneProxyTable.generated.h"

class UDataTable;

struct FToolProxy;
struct FWeaponProxy;
struct FSkillProxy;
struct FCoinProxy;
struct FBasicProxy;
struct FCharacterProxy;
class APlanetWeapon_Base;
class AConsumable_Base;
class UItemProxy_Description;
class UItemDecription;

USTRUCT(BlueprintType)
struct PLANET_API FTableRowProxy_CommonCooldownInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int32 CoolDownTime = 10;
};
