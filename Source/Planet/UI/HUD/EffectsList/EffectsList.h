// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"

#include "CharacterAttributesComponent.h"
#include "UIInterfaces.h"
#include "HUDInterface.h"

#include "EffectsList.generated.h"

struct FCharacterStateInfo;
class UEffectItem;
class ACharacterBase;
class UCS_Base;

UCLASS()
class PLANET_API UEffectsList : 
	public UMyUserWidget, 
	public IHUDInterface
{
	GENERATED_BODY()

public:

	using FCallbackHandle = 
		TCallbackHandleContainer<void(ECharacterStateType, UCS_Base*)>::FCallbackHandleSPtr;

	using FCharacterStateMapHandle =
		TCallbackHandleContainer<void(const TSharedPtr<FCharacterStateInfo>&, bool)>::FCallbackHandleSPtr;

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

	UEffectItem* AddEffectItem();

	void BindCharacterState(ACharacterBase*TargetCharacterPtr);

protected:

	virtual void ResetUIByData()override;

	void OnCharacterStateChanged(ECharacterStateType CharacterStateType, UCS_Base* CharacterStatePtr);
	
	void OnCharacterStateMapChanged(const TSharedPtr<FCharacterStateInfo>& CharacterStatePtr, bool bIsAdd);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Class")
	TSubclassOf<UEffectItem>EffectItemClass;

	FCallbackHandle CallbackHandle;

	FCharacterStateMapHandle CharacterStateMapHandle;

	TMap<FGameplayTag, UEffectItem*>EffectItemMap;

};
