// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyUserWidget.h"

#include "GenerateType.h"
#include "MenuInterface.h"
#include "ScaleableWidget.h"

#include "TalentAllocation.generated.h"

class UTalentIcon;

/**
 *
 */
UCLASS()
class PLANET_API UTalentAllocation :
	public UScaleableWidget,
	public IMenuInterface
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

protected:

	virtual void EnableMenu()override;

	virtual void DisEnableMenu()override;

	virtual EMenuType GetMenuType()const override final;
	
	void OnUsedTalentNumChanged(int32 UsedNum, int32 TatolNum);

	UFUNCTION()
	bool OnAddPoint(UTalentIcon* TalentIconPtr, bool bIsAdd);

private:
	void OnSelectedCharacterProxy(
		const TSharedPtr<FCharacterProxy>& ProxyPtr
		);

	TSharedPtr<FCharacterProxy> CurrentProxyPtr = nullptr;
};
