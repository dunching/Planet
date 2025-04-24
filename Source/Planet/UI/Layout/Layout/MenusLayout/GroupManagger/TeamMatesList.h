// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Common/GenerateType.h"
#include "ItemProxy_Minimal.h"
#include "MenuInterface.h"

#include "TeamMatesList.generated.h"

struct FStreamableHandle;

struct FBasicProxy;
struct FCharacterProxy;
class UTeamMateInfo;
class AHumanCharacter;

/**
 *
 */
UCLASS()
class PLANET_API UTeamMatesList :
	public UMyUserWidget,
	public IMenuInterface
{
	GENERATED_BODY()

public:
	using FDelegateHandle =
	TCallbackHandleContainer<void(
		UTeamMateInfo*,
		const TSharedPtr<FCharacterProxy>&,
		const TSharedPtr<FCharacterProxy>&
	)>::FCallbackHandleSPtr;

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	virtual void SyncData() override;

	virtual EMenuType GetMenuType()const override final;
	
	AHumanCharacter* HumanCharacterPtr = nullptr;

protected:
	
	virtual void ResetUIByData() override;

	void OnWeaponProxyChanged(
		UTeamMateInfo*UIPtr,
		const TSharedPtr<FCharacterProxy>& PrevProxycharacterSPtr,
		const TSharedPtr<FCharacterProxy>& NewProxycharacterSPtr
	);

	void OnTeammateChanged(UTeamMateInfo* GourpMateProxyPtr);

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UTeamMateInfo> TeamMateInfoClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	int32 MaxMemberNum = 6;

	TArray<FDelegateHandle> DelegateAry;
};

UCLASS()
class PLANET_API UTeanMatesList : public UTeamMatesList
{
	GENERATED_BODY()
};
