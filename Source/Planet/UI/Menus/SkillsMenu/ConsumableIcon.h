// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"

#include "UIInterfaces.h"
#include "Common/GenerateType.h"
#include "SceneElement.h"
#include "AllocationIconBase.h"

#include "ConsumableIcon.generated.h"

struct FStreamableHandle;
class UDragDropOperation;

class USkillUnit;
class UConsumableUnit;
class UBasicUnit;

UCLASS()
class PLANET_API UConsumableIcon : public UAllocationIconBase
{
	GENERATED_BODY()

public:

	UConsumableIcon(const FObjectInitializer& ObjectInitializer);
	
	virtual void ResetToolUIByData(UBasicUnit* BasicUnitPtr)override;

	virtual void EnableIcon(bool bIsEnable)override;

protected:

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

	UFUNCTION(BlueprintImplementableEvent)
	void PlaySkillIsReady();

	void SetLevel();

private:

};