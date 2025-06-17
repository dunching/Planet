// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Blueprint/IUserObjectListEntry.h>

#include "UIInterfaces.h"
#include "GenerateTypes.h"
#include "ItemProxy_Minimal.h"

#include "BackpackIconWrapper.generated.h"

struct FStreamableHandle;

struct FBasicProxy;
struct IProxy_Allocationble;
struct FToolProxy;

class UBackpackConsumableIcon;
class UBackpackToolIcon;
class UBackpackSkillIcon;
class UBackpackWeaponIcon;
class UBackpackMaterialIcon;
class UAllocationSkillsMenu;

/**
 *
 */
UCLASS()
class PLANET_API UBackpackIconWrapper :
	public UUserWidget_Override,
	public IItemProxyIconInterface,
	public IUserObjectListEntry
{
	GENERATED_BODY()

public:

	UBackpackIconWrapper(const FObjectInitializer& ObjectInitializer);

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject)override;

	virtual void InvokeReset(UUserWidget* BaseWidgetPtr)override;

	virtual void ResetToolUIByData(const TSharedPtr<FBasicProxy>& BasicProxyPtr)override;

	virtual void EnableIcon(bool bIsEnable)override;

	TSharedPtr<FBasicProxy> TargetBasicProxyPtr = nullptr;

	UAllocationSkillsMenu * AllocationSkillsMenuPtr= nullptr;
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Icon Class")
	TSubclassOf<UBackpackToolIcon> ToolProxyClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Icon Class")
	TSubclassOf<UBackpackConsumableIcon> ConsumableProxyClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Icon Class")
	TSubclassOf<UBackpackSkillIcon> BackpackSkillIconClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Icon Class")
	TSubclassOf<UBackpackWeaponIcon> BackpackWeaponIconClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Icon Class")
	TSubclassOf<UBackpackMaterialIcon> BackpackMaterialIconClass;

};
