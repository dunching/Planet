// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Blueprint/IUserObjectListEntry.h>

#include "UIInterfaces.h"
#include "Common/GenerateType.h"
#include "SceneElement.h"

#include "BackpackIconWrapper.generated.h"

struct FStreamableHandle;

class UBasicUnit;
class UToolUnit;
class UBackpackConsumableIcon;
class UBackpackToolIcon;
class UBackpackSkillIcon;
class UBackpackWeaponIcon;

/**
 *
 */
UCLASS()
class PLANET_API UBackpackIconWrapper :
	public UMyUserWidget,
	public IToolsIconInterface,
	public IUserObjectListEntry
{
	GENERATED_BODY()

public:

	using FOnDragIconDelegate = TCallbackHandleContainer<void(bool, UBasicUnit*)>;

	UBackpackIconWrapper(const FObjectInitializer& ObjectInitializer);

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject)override;

	virtual void InvokeReset(UUserWidget* BaseWidgetPtr)override;

	virtual void ResetToolUIByData(UBasicUnit* BasicUnitPtr)override;

	virtual void EnableIcon(bool bIsEnable)override;

	UBasicUnit* TargetBasicUnitPtr = nullptr;

	FOnDragIconDelegate OnDragIconDelegate;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Icon Class")
	TSubclassOf<UBackpackToolIcon> ToolUnitClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Icon Class")
	TSubclassOf<UBackpackConsumableIcon> ConsumableUnitClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Icon Class")
	TSubclassOf<UBackpackSkillIcon> BackpackSkillIconClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Icon Class")
	TSubclassOf<UBackpackWeaponIcon> BackpackWeaponIconClass;

};
