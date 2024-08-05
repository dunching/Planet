// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"

#include "UIInterfaces.h"
#include "GenerateType.h"
#include "SceneElement.h"
#include "AllocationIconBase.h"

#include "SkillsIcon.generated.h"

struct FStreamableHandle;
class UDragDropOperation;

class USkillUnit;

/**
 * ��Ϊ����Icon
 * 1.ͬ�����Ҫ�ܻ����Ƴ� 2.�Ҽ��Ƴ� 3.��קʱ���ö�Ӧ��Icon
 */
UCLASS()
class PLANET_API USkillsIcon : public UAllocationIconBase
{
	GENERATED_BODY()

public:

	USkillsIcon(const FObjectInitializer& ObjectInitializer);

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