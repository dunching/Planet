// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyUserWidget.h"

#include "GenerateType.h"
#include "RaffleCommon.h"

#include "RaffleMenu.generated.h"

class UTalentIcon;
class URaffleType;
class URaffleBtn;
class URaffle_Unit;
class UBasicUnit;
struct FSceneToolsContainer;
struct FTableRowUnit;

/**
 *
 */
UCLASS()
class PLANET_API URaffleMenu : public UMyUserWidget
{
	GENERATED_BODY()

public:

	using FOnGetUnitDelegateHandle =
		TCallbackHandleContainer<void(const TArray<TPair<FTableRowUnit, TSubclassOf<UBasicUnit>>>&)>::FCallbackHandleSPtr;

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

	void InitialRaffleType();

	void InitialRaffleBtn();

	void SetHoldItemProperty(const FSceneToolsContainer& NewSPHoldItemPerperty);

	void SwitchDisplay(bool bIsDisplayRaffleUI);

	void ResetGetUnitAry(const TArray<TPair<FTableRowUnit, TSubclassOf<UBasicUnit>>>&Ary);

protected:

	FOnGetUnitDelegateHandle OnGetUnitDelegateHandle;

	void OnRaffleTypeSelected(URaffleType* RaffleTypePtr);
	
	void OnRaffleBtnSelected(URaffleBtn* RaffleTypePtr);

	UFUNCTION()
	void OnClickedConfirmGetUnitBtn();

	URaffleType* PreviouRaffleTypePtr = nullptr;

	ERaffleType LastRaffleType = ERaffleType::kRafflePermanent;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI")
	TSubclassOf<URaffle_Unit>Raffle_UnitClass;
	
};
