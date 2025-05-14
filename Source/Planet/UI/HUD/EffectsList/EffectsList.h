// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"

#include "MyUserWidget.h"

#include "CharacterAttributesComponent.h"
#include "UIInterfaces.h"
#include "HUDInterface.h"
#include "LayoutInterfacetion.h"

#include "EffectsList.generated.h"

struct FCharacterStateInfo;
struct FGameplayEffectSpec;
struct FActiveGameplayEffect;

class UEffectItem;
class ACharacterBase;
class UCS_Base;
class UAbilitySystemComponent;

UCLASS()
class PLANET_API UEffectsList : 
	public UMyUserWidget, 
	public ILayoutItemInterfacetion
{
	GENERATED_BODY()

public:

	using FCallbackHandle = 
		TCallbackHandleContainer<void(ECharacterStateType, UCS_Base*)>::FCallbackHandleSPtr;

	using FCharacterStateMapHandle =
		TCallbackHandleContainer<void(const TSharedPtr<FCharacterStateInfo>&, bool)>::FCallbackHandleSPtr;

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

	virtual void SynchronizeProperties()override;

	virtual void Enable() override;
	
	virtual void DisEnable() override;

	UEffectItem* AddEffectItem(
		const TObjectPtr<UAbilitySystemComponent>& AbilitySystemComponentPtr,
		FActiveGameplayEffectHandle NewActiveGameplayEffectHandle
		);

	void BindCharacterState(ACharacterBase*TargetCharacterPtr);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WrapBox Settings")
	bool bIsPositiveSequence = true;

protected:

	void OnCharacterStateChanged(ECharacterStateType CharacterStateType, UCS_Base* CharacterStatePtr);
	
	void OnCharacterStateMapChanged(const TSharedPtr<FCharacterStateInfo>& CharacterStatePtr, bool bIsAdd);

	void OnActiveGameplayEffect(
		UAbilitySystemComponent*,
		const FGameplayEffectSpec&,
		FActiveGameplayEffectHandle
		);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Class")
	TSubclassOf<UEffectItem>EffectItemClass;
	
	FCallbackHandle CallbackHandle;

	FDelegateHandle ActiveGameplayEffectHandle;
	
	FDelegateHandle AppliedGameplayEffectHandle;
	
	// FCharacterStateMapHandle CharacterStateMapHandle;

	// TMap<FGuid, UEffectItem*>EffectItemMap;

	ACharacterBase* TargetCharacterPtr = nullptr;
	
};
