// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"

#include "CharacterAttributesComponent.h"
#include "HUDInterface.h"

#include "PawnStateActionHUD.generated.h"

struct FSceneTool;
struct FCharacterAttributes;
struct FSocket_FASI;
struct FWeaponProxy;

class UState_Talent_NuQi;
class UState_Talent_YinYang;
class ACharacterBase;

UCLASS()
class PLANET_API UPawnStateActionHUD :
	public UMyUserWidget, 
	public IHUDInterface
{
	GENERATED_BODY()

public:

	using FOnInitaliedGroupSharedInfo =
		TCallbackHandleContainer<void()>::FCallbackHandleSPtr;

	using FOnAllocationSkillChangedHandle =
		TCallbackHandleContainer<void()>::FCallbackHandleSPtr;

	using FOnCanAciveSkillChangedHandle =
		TCallbackHandleContainer<void()>::FCallbackHandleSPtr;

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	virtual void ResetUIByData()override;

	ACharacterBase* CharacterPtr = nullptr;

protected:

	void BindEvent();

	void InitialTalentUI();

	void InitialActiveSkillIcon();

	void InitialWeaponSkillIcon();

	FOnInitaliedGroupSharedInfo OnInitaliedGroupSharedInfoHandle;
	
	TArray<FOnAllocationSkillChangedHandle> OnAllocationSkillChangedDelegateAry;

	FOnCanAciveSkillChangedHandle OnCanAciveSkillChangedHandle;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UState_Talent_NuQi>State_Talent_NuQi_Class;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UState_Talent_YinYang>Talent_YinYang_Class;

private:

};