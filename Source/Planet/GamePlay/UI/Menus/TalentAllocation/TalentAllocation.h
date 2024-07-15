// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyUserWidget.h"

#include "GenerateType.h"

#include "TalentAllocation.generated.h"

class UTalentIcon;

/**
 *
 */
UCLASS()
class PLANET_API UTalentAllocation : public UMyUserWidget
{
	GENERATED_BODY()

public:

	// ���õ��ܵ������
	using FPointsDelegateHandle = TOnValueChangedCallbackContainer<int32>::FCallbackHandleSPtr;

	// ����ĵ������
	using FPointDelegateHandle = TCallbackHandleContainer<void(UTalentIcon*, bool)>::FCallbackHandleSPtr;

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

protected:

	void OnUsedTalentNumChanged(int32 OldNum, int32 NewNum);
	
	void OnAddPoint(UTalentIcon* TalentIconPtr, bool bIsAdd);

	FPointsDelegateHandle OnValueChanged;
	
	TArray<FPointDelegateHandle> OnPointChangedHandleAry;

};
