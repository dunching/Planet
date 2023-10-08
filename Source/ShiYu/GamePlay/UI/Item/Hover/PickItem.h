// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickItem.generated.h"

class UWidgetComponent;

/**
 * 
 */
UCLASS()
class SHIYU_API APickItem : public AActor
{
	GENERATED_BODY()
	
public:

	APickItem();

	void SetUIClass(UClass* ClassPtr);

	void SetVisible(bool Visibility);

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Components")
		UWidgetComponent* WidgetCompptr = nullptr;

private:
	
};
