// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"

#include "GenerateType.h"
#include "HUDInterface.h"
#include "LayoutInterfacetion.h"


#include "PawnStateBuildingHUD.generated.h"

class UToolsMenu;



struct FToolsSocketInfo;
struct FConsumableSocketInfo;

/**
 *
 */
UCLASS()
class PLANET_API UPawnStateBuildingHUD :
	public UMyUserWidget,
	public ILayoutItemInterfacetion
{
	GENERATED_BODY()

public:

	using FDelegateHandle = typename TOnValueChangedCallbackContainer<int32>::FCallbackHandleSPtr;

	UToolsMenu* GetEquipMenu();

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

	// virtual void ResetUIByData()override;
// 
// 	TMap<FGameplayTag, TSharedPtr<FToolsSocketInfo>>GetTools();
// 
// 	TMap<FGameplayTag, TSharedPtr<FConsumableSocketInfo>>GetConsumables();

protected:

	virtual void Enable() override;
	
	virtual void DisEnable() override;

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

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName ItemMenu;

private:

};
