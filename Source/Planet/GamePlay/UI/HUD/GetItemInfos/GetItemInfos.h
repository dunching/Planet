// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"

#include "UIInterfaces.h"
#include "GroupsManaggerSubSystem.h"

#include "GetItemInfos.generated.h"

class UHUD_TeamMateInfo;

class UGetItemInfosItem;

class UBasicUnit;
class UCoinUnit;
class USkillUnit;
class UConsumableUnit;

UCLASS()
class PLANET_API UGetItemInfos : public UMyUserWidget, public IItemsMenuInterface
{
	GENERATED_BODY()

public:

#if WITH_EDITOR
	virtual void NativeConstruct()override;
#else
	virtual void NativePreConstruct()override;
#endif

	virtual void ResetUIByData()override;

	void OnSkillUnitChanged(USkillUnit* UnitPtr, bool bIsAdd);

	void OnCoinUnitChanged(UCoinUnit* UnitPtr, bool bIsAdd, int32 Num);

	void OnConsumableUnitChanged(UConsumableUnit* UnitPtr, bool bIsAdd, int32 Num);
	
	void OnGourpmateUnitChanged(UGourpmateUnit* UnitPtr, bool bIsAdd);

protected:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UGetItemInfosItem>GetItemInfosClass;

};
