// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"

#include "UIInterfaces.h"
#include "GroupsManaggerSubSystem.h"

#include "HUD_TeamInfo.generated.h"

class UHUD_TeamMateInfo;

UCLASS()
class PLANET_API UHUD_TeamInfo :
	public UMyUserWidget,
	public IMenuInterface
{
	GENERATED_BODY()

public:

	using FCharacterUnitType = FCharacterProxy;

	using FTeammateOptionChangedDelegate =
		TCallbackHandleContainer<void(ETeammateOption, const TSharedPtr<FCharacterUnitType>&)>::FCallbackHandleSPtr;

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)override;

protected:

	virtual void ResetUIByData()override;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UHUD_TeamMateInfo>TeamMateInfoClass;

private:

	void OnTeammateOptionChanged(
		ETeammateOption TeammateOption, 
		const TSharedPtr<FCharacterUnitType>& LeaderPCPtr
	);

	FTeammateOptionChangedDelegate TeammateOptionChangedDelegateContainer;

};
