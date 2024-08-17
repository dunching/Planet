// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"

#include "CharacterAttributesComponent.h"
#include "UIInterfaces.h"

#include "EffectsList.generated.h"

class UEffectItem;
class ACharacterBase;
class UCS_Base;

UCLASS()
class PLANET_API UEffectsList : public UMyUserWidget, public IMenuInterface
{
	GENERATED_BODY()

public:

	using FCallbackHandle = TCallbackHandleContainer<void(ECharacterStateType, UCS_Base*)>::FCallbackHandleSPtr;

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

	UEffectItem* AddEffectItem();

	void BindCharacterState(ACharacterBase*TargetCharacterPtr);

protected:

	virtual void ResetUIByData()override;

	void OnCharacterStateChanged(ECharacterStateType CharacterStateType, UCS_Base* CharacterStatePtr);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Class")
	TSubclassOf<UEffectItem>EffectItemClass;

	FCallbackHandle CallbackHandle;

	TMap<FGameplayTag, UEffectItem*>EffectItemMap;

};
