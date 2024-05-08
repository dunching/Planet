// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"

#include "GenerateType.h"
#include "UIInterfaces.h"
#include "EquipmentElementComponent.h"

#include "PawnStateBuildingHUD.generated.h"

class UToolsMenu;

struct FCharacterAttributes;

/**
 *
 */
UCLASS()
class PLANET_API UPawnStateBuildingHUD : public UUserWidget, public IItemsMenuInterface
{
	GENERATED_BODY()

public:

	using FDelegateHandle = typename TOnValueChangedCallbackContainer<int32>::FCallbackHandleSPtr;

	UToolsMenu* GetEquipMenu();

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

	virtual void ResetUIByData()override;

	TArray<FToolsSocketInfo>GetEquipMenus();

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Keys)
	FKey ToolSocket1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Keys)
	FKey ToolSocket2;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Keys)
	FKey ToolSocket3;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Keys)
	FKey ToolSocket4;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Keys)
	FKey ToolSocket5;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Keys)
	FKey ToolSocket6;

	FCharacterAttributes* PawnDataStructPtr = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName ItemMenu;

private:

};
