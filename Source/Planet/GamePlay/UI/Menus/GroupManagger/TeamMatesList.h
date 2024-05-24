// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UIInterfaces.h"
#include "Common/GenerateType.h"
#include "SceneElement.h"

#include "TeamMatesList.generated.h"

struct FStreamableHandle;

class UBasicUnit;
class UGourpmateUnit;
class UTeamMateInfo;
class AHumanCharacter;

/**
 *
 */
UCLASS()
class PLANET_API UTeamMatesList : public UUserWidget, public IItemsMenuInterface
{
	GENERATED_BODY()

public:

	using FDelegateHandle = TCallbackHandleContainer<void(UTeamMateInfo*)>::FCallbackHandleSPtr;

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

	AHumanCharacter* HumanCharacterPtr = nullptr;

protected:

	virtual void ResetUIByData()override;

	void OnTeammateChanged(UTeamMateInfo* GourpMateUnitPtr);

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UTeamMateInfo>TeamMateInfoClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	int32 MaxMemberNum = 6;

	TArray<FDelegateHandle>DelegateAry;

};

UCLASS()
class PLANET_API UTeanMatesList : public UTeamMatesList
{
	GENERATED_BODY()
};