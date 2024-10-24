// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"

#include "MainUILAyout.generated.h"

class UMainUILayout;
class URaffleMenu;
class UPawnStateActionHUD;
class UPawnStateConsumablesHUD;
class UPawnStateBuildingHUD;
class UItemsDragDropOperation;
class UDragDropOperationWidget;
class UAllocationToolsMenu;
class UAllocationSkillsMenu;
class UBackpackMenu;
class UCharacterTitle;
class UEffectsList;
class UProgressTips;
class UTalentAllocation;
class UGroupManaggerMenu;
class UHUD_TeamInfo;
class UFocusIcon;
class UCharacterRisingTips;
class AHumanCharacter;
class AHorseCharacter;
class AGeneratorNPC;

/**
 *
 */
UCLASS()
class PLANET_API UMainUILayout : public UMyUserWidget
{
	GENERATED_BODY()

public:

#if WITH_EDITOR
	virtual void NativeConstruct()override;
#else
	virtual void NativePreConstruct()override;
#endif
	
#pragma region MenusUI
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UBackpackMenu>BackpackMenuClass;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UPawnStateActionHUD>PawnStateActionHUDClass;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UPawnStateConsumablesHUD>PawnStateConsumablesHUDClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UPawnStateBuildingHUD>PawnStateBuildingHUDClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UEffectsList>EffectsListClass;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UProgressTips>ProgressTipsClass;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UHUD_TeamInfo>HUD_TeamInfoClass;
#pragma endregion MenusUI

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName PawnBuildingStateHUDSocket;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName GroupMatesManaggerSocket;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName TalentAllocationSocket;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName EffectsListSocket;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName ProgressTipsSocket;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName HUD_TeamSocket;
	
};
