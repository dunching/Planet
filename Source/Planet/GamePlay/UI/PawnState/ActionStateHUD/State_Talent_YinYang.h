// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"

#include "GenerateType.h"
#include "Skill_Talent_YinYang.h"

#include "State_Talent_YinYang.generated.h"

UCLASS()
class PLANET_API UState_Talent_YinYang : public UUserWidget
{
	GENERATED_BODY()

public:

	using FCurrentTalentType = FTalent_YinYang;

	using FDelegateHandle = TOnValueChangedCallbackContainer<int32>::FCallbackHandleSPtr;

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

	ACharacterBase* TargetCharacterPtr = nullptr;

protected:

	void OnCurrentValueChanged(int32 CurrentValue);

	void OnValueChanged();

	UFUNCTION(BlueprintImplementableEvent)
	void SetValue(ETalent_State_Type Talent_State_Type, int32 CurrentValue, int32 MaxValue);

	FDelegateHandle OnValueChangedHandle;

	TSharedPtr<FCurrentTalentType>TalentSPtr;

};
