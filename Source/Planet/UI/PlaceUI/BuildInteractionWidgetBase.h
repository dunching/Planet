// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UserWidget_Override.h"

#include "BuildInteractionWidgetBase.generated.h"

/**
 *
 */
UCLASS()
class PLANET_API UBuildInteractionWidgetBase : public UUserWidget_Override
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct()override;

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName CarryTextName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName FixBtnName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName FixNeedItemTextName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName DestroyBtnName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName DestroyGetItemTextName;

};
