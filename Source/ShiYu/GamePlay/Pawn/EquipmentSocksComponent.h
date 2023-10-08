// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include <Components/ActorComponent.h>
#include <GenerateType.h>
#include <ItemType.h>

#include "EquipmentSocksComponent.generated.h"

class AEquipmentBase;

UCLASS()
class SHIYU_API UEquipmentSocksComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UEquipmentSocksComponent(const FObjectInitializer& ObjectInitializer);

	virtual void UseItem(const FItemNum&Item);

	virtual void DoAction(EEquipmentActionType ActionType);

	static FName ComponentName;

protected:

	TMap<EItemSocketType, AEquipmentBase*>ItemsActionMap;

};
