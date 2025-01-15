// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyUserWidget.h"

#include "GenerateType.h"
#include "LayoutInterfacetion.h"
#include "UICommon.h"
#include "MenuInterface.h"
#include "MainMenuCommon.h"

#include "MainMenuLayout.generated.h"

class URaffleMenu;
class UTalentAllocation;
class UGroupManaggerMenu;
class UAllocationSkillsMenu;

/**
 *
 */
UCLASS()
class PLANET_API UMainMenuLayout :
	public UMyUserWidget,
	public ILayoutInterfacetion
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

	virtual void Enable()override;
	
	virtual void DisEnable()override;

	void SwitchViewer(EMenuType MenuType);

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
