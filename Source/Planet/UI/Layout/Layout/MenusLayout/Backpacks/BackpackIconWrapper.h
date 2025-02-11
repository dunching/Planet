// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Blueprint/IUserObjectListEntry.h>

#include "UIInterfaces.h"
#include "Common/GenerateType.h"
#include "ItemProxy_Minimal.h"

#include "BackpackIconWrapper.generated.h"

struct FStreamableHandle;

struct FBasicProxy;
struct FAllocationbleProxy;
struct FToolProxy;
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
	public IItemProxyIconInterface,
	public IUserObjectListEntry
{
	GENERATED_BODY()

public:

	using FOnDragIconDelegate = TCallbackHandleContainer<void(bool, const TSharedPtr<FBasicProxy>&)>;

	UBackpackIconWrapper(const FObjectInitializer& ObjectInitializer);

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject)override;

	virtual void InvokeReset(UUserWidget* BaseWidgetPtr)override;

	virtual void ResetToolUIByData(const TSharedPtr<FBasicProxy>& BasicProxyPtr)override;

	virtual void EnableIcon(bool bIsEnable)override;

	TSharedPtr<FBasicProxy> TargetBasicProxyPtr = nullptr;

	FOnDragIconDelegate OnDragIconDelegate;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Icon Class")
	TSubclassOf<UBackpackToolIcon> ToolProxyClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Icon Class")
	TSubclassOf<UBackpackConsumableIcon> ConsumableProxyClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Icon Class")
	TSubclassOf<UBackpackSkillIcon> BackpackSkillIconClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Icon Class")
	TSubclassOf<UBackpackWeaponIcon> BackpackWeaponIconClass;

};
