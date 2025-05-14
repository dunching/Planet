// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"

#include "UIInterfaces.h"
#include "HUDInterface.h"
#include "LayoutInterfacetion.h"


#include "HUD_TeamInfo.generated.h"

class UHUD_TeamMateInfo;

UCLASS()
class PLANET_API UHUD_TeamInfo :
	public UMyUserWidget,
	public ILayoutItemInterfacetion
{
	GENERATED_BODY()

public:
	using FCharacterProxyType = FCharacterProxy;

	using FTeammateOptionChangedDelegate =
	TCallbackHandleContainer<void(
		ETeammateOption,
		const TSharedPtr<FCharacterProxyType>&
	)>::FCallbackHandleSPtr;

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	virtual FReply NativeOnKeyDown(
		const FGeometry& InGeometry,
		const FKeyEvent& InKeyEvent
	) override;

	virtual void Enable() override;
	
	virtual void DisEnable() override;

protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UHUD_TeamMateInfo> TeamMateInfoClass;

private:
	void OnTeammateOptionChanged(
		ETeammateOption TeammateOption,
		const TSharedPtr<FCharacterProxyType>& LeaderPCPtr
	);

	FTeammateOptionChangedDelegate TeammateOptionChangedDelegateContainer;
};
