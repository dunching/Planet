// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"

#include "UIInterfaces.h"
#include "Common/GenerateType.h"
#include "SceneElement.h"
#include "AllocationIconBase.h"

#include "GroupmateIcon.generated.h"

struct FStreamableHandle;
class UDragDropOperation;

class USkillUnit;
class UGourpmateUnit;
class UBasicUnit;

UCLASS()
class PLANET_API UGroupmateIcon : public UMyUserWidget
{
	GENERATED_BODY()

public:

protected:

	virtual void NativeConstruct()override;

private:

	UGourpmateUnit* UnitPtr = nullptr;

};