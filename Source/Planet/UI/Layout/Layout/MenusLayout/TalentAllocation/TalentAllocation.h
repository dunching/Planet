// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyUserWidget.h"

#include "GenerateType.h"
#include "MenuInterface.h"

#include "TalentAllocation.generated.h"

class UTalentIcon;

/**
 *
 */
UCLASS()
class PLANET_API UTalentAllocation :
	public UMyUserWidget,
	public IMenuInterface
{
	GENERATED_BODY()

public:

	// 可用的总点数变更
	using FPointsDelegateHandle = TOnValueChangedCallbackContainer<int32>::FCallbackHandleSPtr;

	// 分配的点数变更
	using FPointDelegateHandle = TCallbackHandleContainer<void(UTalentIcon*, bool)>::FCallbackHandleSPtr;

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

protected:

	virtual void EnableMenu()override;

	virtual void DisEnableMenu()override;

	virtual EMenuType GetMenuType()const override final;
	
	void OnUsedTalentNumChanged(int32 OldNum, int32 NewNum);
	
	void OnAddPoint(UTalentIcon* TalentIconPtr, bool bIsAdd);

	FPointsDelegateHandle OnValueChanged;
	
	TArray<FPointDelegateHandle> OnPointChangedHandleAry;

};
