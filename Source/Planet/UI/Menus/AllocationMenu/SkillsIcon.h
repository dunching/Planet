// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"

#include "UIInterfaces.h"
#include "GenerateType.h"
#include "ItemProxy_Minimal.h"
#include "AllocationIconBase.h"

#include "SkillsIcon.generated.h"

struct FStreamableHandle;
class UDragDropOperation;

struct FSkillProxy;

/**
 * 作为技能Icon
 * 1.同类的需要能互相移除 2.右键移除 3.拖拽时禁用对应的Icon
 */
UCLASS()
class PLANET_API USkillsIcon : public UAllocationIconBase
{
	GENERATED_BODY()

public:

	USkillsIcon(const FObjectInitializer& ObjectInitializer);

	virtual void ResetToolUIByData(const TSharedPtr<FAllocationbleProxy>& BasicProxyPtr)override;

	virtual void EnableIcon(bool bIsEnable)override;

	virtual void OnDragIcon(bool bIsDragging, const TSharedPtr<FAllocationbleProxy>& ProxyPtr)override;

protected:

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

	UFUNCTION(BlueprintImplementableEvent)
	void PlaySkillIsReady();

	void SetLevel();

private:

};