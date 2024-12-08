// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Blueprint/IUserObjectListEntry.h>

#include "UIInterfaces.h"
#include "MyUserWidget.h"
#include "Common/GenerateType.h"
#include "ItemProxy_Minimal.h"

#include "TeamMateInfo.generated.h"

struct FStreamableHandle;

struct FBasicProxy;
struct FCharacterProxy;
class UTeamMatesList;

/**
 *
 */
UCLASS()
class PLANET_API UTeamMateInfo :
	public UMyUserWidget,
	public IItemProxyIconInterface,
	public IUserObjectListEntry
{
	GENERATED_BODY()

public:

	friend UTeamMatesList;

	using FCallbackHandleContainer =
		TCallbackHandleContainer<void(
			UTeamMateInfo*,
			const TSharedPtr<FCharacterProxy>&,
			const TSharedPtr<FCharacterProxy>&
			)>;

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject)override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)override;

	virtual void InvokeReset(UUserWidget* BaseWidgetPtr)override;

	virtual void ResetToolUIByData(const TSharedPtr<FBasicProxy>& BasicProxyPtr)override;

	virtual void EnableIcon(bool bIsEnable)override;

	void SynMember2Config();

	FCallbackHandleContainer OnDroped;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Index)
	int32 Index = 0;

	bool bPaseInvokeOnResetProxyEvent = false;
	
private:

	bool bIsInBackpakc = false;

	TSharedPtr<FCharacterProxy> GroupMateProxyPtr = nullptr;

};
