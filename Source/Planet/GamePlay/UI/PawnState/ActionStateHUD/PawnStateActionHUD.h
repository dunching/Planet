// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"

#include "CharacterAttributesComponent.h"
#include "UIInterfaces.h"

#include "PawnStateActionHUD.generated.h"

struct FSceneTool;
struct FCharacterAttributes;

class UState_Talent_NuQi;
class UState_Talent_YinYang;
class ACharacterBase;

UCLASS()
class PLANET_API UPawnStateActionHUD : public UMyUserWidget, public IItemsMenuInterface
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	virtual void ResetUIByData()override;

	// �ⲿ��ʼ��
	ACharacterBase* CharacterPtr = nullptr;

protected:

	void InitialTalentUI();

	void InitialSkillIcon();

	void OnActivedWeaponChanged(EWeaponSocket WeaponSocket);

	TCallbackHandleContainer<void(EWeaponSocket)>::FCallbackHandleSPtr ActivedWeaponChangedDelegate;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UState_Talent_NuQi>State_Talent_NuQi_Class;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UState_Talent_YinYang>Talent_YinYang_Class;

private:

};
