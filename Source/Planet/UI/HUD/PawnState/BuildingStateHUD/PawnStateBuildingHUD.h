// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"

#include "GenerateType.h"
#include "HUDInterface.h"


#include "PawnStateBuildingHUD.generated.h"

class UToolsMenu;

struct FCharacterAttributes;

struct FToolsSocketInfo;
struct FConsumableSocketInfo;

/**
 *
 */
UCLASS()
class PLANET_API UPawnStateBuildingHUD :
	public UMyUserWidget,
	public IHUDInterface
{
	GENERATED_BODY()

public:

	using FDelegateHandle = typename TOnValueChangedCallbackContainer<int32>::FCallbackHandleSPtr;

	UToolsMenu* GetEquipMenu();

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

	virtual void ResetUIByData()override;
// 
// 	TMap<FGameplayTag, TSharedPtr<FToolsSocketInfo>>GetTools();
// 
// 	TMap<FGameplayTag, TSharedPtr<FConsumableSocketInfo>>GetConsumables();

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
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Keys)
	FKey ToolSocket7;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Keys)
	FKey ToolSocket8;

	FCharacterAttributes* PawnDataStructPtr = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName ItemMenu;

private:

};
