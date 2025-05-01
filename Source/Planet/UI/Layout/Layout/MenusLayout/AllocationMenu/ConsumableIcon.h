// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"

#include "UIInterfaces.h"
#include "Common/GenerateType.h"
#include "ItemProxy_Minimal.h"
#include "AllocationIconBase.h"

#include "ConsumableIcon.generated.h"

struct FStreamableHandle;
class UDragDropOperation;

struct FSkillProxy;
struct FConsumableProxy;
struct IProxy_Allocationble;

UCLASS()
class PLANET_API UConsumableIcon : public UAllocationIconBase
{
	GENERATED_BODY()

public:

	UConsumableIcon(const FObjectInitializer& ObjectInitializer);
	
	virtual void ResetToolUIByData(const TSharedPtr<FBasicProxy>& BasicProxyPtr)override;

	virtual void EnableIcon(bool bIsEnable)override;

protected:

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

	UFUNCTION(BlueprintImplementableEvent)
	void PlaySkillIsReady();

	void SetLevel();

private:

};