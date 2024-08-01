// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"

#include "UIInterfaces.h"
#include "GroupsManaggerSubSystem.h"

#include "HUD_TeamInfo.generated.h"

class UHUD_TeamMateInfo;

UCLASS()
class PLANET_API UHUD_TeamInfo : public UMyUserWidget, public IItemsMenuInterface
{
	GENERATED_BODY()

public:

	using FTeammateOptionChangedDelegateContainer =
		UGroupsManaggerSubSystem::FTeammateOptionChangedDelegateContainer::FCallbackHandleSPtr;

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
		UGroupsManaggerSubSystem::FPawnType* LeaderPCPtr
	);

	FTeammateOptionChangedDelegateContainer TeammateOptionChangedDelegateContainer;

};
