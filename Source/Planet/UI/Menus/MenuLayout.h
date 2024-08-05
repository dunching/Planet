// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyUserWidget.h"

#include "GenerateType.h"

#include "MenuLayout.generated.h"

class URaffleMenu;
class UTalentAllocation;
class UGroupManaggerMenu;
class UAllocationSkillsMenu;

/**
 *
 */
UCLASS()
class PLANET_API UMenuLayout : public UMyUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct()override;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<URaffleMenu>RaffleMenuClass;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UTalentAllocation>TalentAllocationClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UGroupManaggerMenu>GroupManaggerMenuClass;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UAllocationSkillsMenu>AllocationSkillsMenuClass;

protected:
	
};
