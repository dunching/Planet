// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UserWidget_Override.h"

#include "GenerateTypes.h"
#include "MenuInterface.h"
#include "RaffleCommon.h"

#include "RaffleMenu.generated.h"

class UTalentIcon;
class URaffleType;
class URaffleBtn;
class URaffle_Proxy;
struct FBasicProxy;
struct FSceneProxyContainer;
struct FTableRowProxy;

/**
 *
 */
UCLASS()
class PLANET_API URaffleMenu :
	public UUserWidget_Override,
	public IMenuInterface
{
	GENERATED_BODY()

public:
	using FOnGetProxyDelegateHandle =
	TCallbackHandleContainer<void(
		const TArray<FTableRowProxy*>&
	)>::FCallbackHandleSPtr;

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	virtual void EnableMenu()override;
	
	virtual void DisEnableMenu()override;

	virtual EMenuType GetMenuType()const override final;
	
	void InitialRaffleType();

	void InitialRaffleBtn();

	void SetHoldItemProperty(
		const TSharedPtr<FSceneProxyContainer>& NewSPHoldItemPerperty
	);

	void SwitchDisplay(
		bool bIsDisplayRaffleUI
	);

	void ResetGetProxyAry(
		const TArray<FTableRowProxy*>& Ary
	);

protected:
	FOnGetProxyDelegateHandle OnGetProxyDelegateHandle;

	void OnRaffleTypeSelected(
		URaffleType* RaffleTypePtr
	);

	void OnRaffleBtnSelected(
		URaffleBtn* RaffleTypePtr
	);

	UFUNCTION()
	void OnClickedConfirmGetProxyBtn();

	URaffleType* PreviouRaffleTypePtr = nullptr;

	ERaffleType LastRaffleType = ERaffleType::kRafflePermanent;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI")
	TSubclassOf<URaffle_Proxy> Raffle_ProxyClass;
};
