// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include <GameplayTagContainer.h>

#include "MyUserWidget.h"
#include "GenerateType.h"

#include "CharacterTitle.generated.h"

class ACharacterBase;

class UToolIcon;
class UConversationBorder;
struct FOnAttributeChangeData;
struct FTaskNode_Conversation_SentenceInfo;

/**
 *
 */
UCLASS()
class PLANET_API UCharacterTitle : public UMyUserWidget
{
	GENERATED_BODY()

public:
	using FOnValueChangedDelegateHandle = TOnValueChangedCallbackContainer<int32>::FCallbackHandleSPtr;

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	virtual void NativeTick(
		const FGeometry& MyGeometry,
		float InDeltaTime
		);

	UFUNCTION(BlueprintImplementableEvent)
	void SetCampType(
		ECharacterCampType CharacterCampType
		);

	void SwitchCantBeSelect(
		bool bIsCantBeSelect
		);

	void SetData(
		ACharacterBase* CharacterPtr
		);

protected:
	ACharacterBase* CharacterPtr = nullptr;

protected:
	void OnGameplayEffectTagCountChanged(
		const FGameplayTag Tag,
		int32 Count
		);

	void OnHPChanged(
		const FOnAttributeChangeData& OnAttributeChangeData
		);

	void SetHPChanged(
		float Value,
		float MaxValue
		);

	void OnStaminaChanged(
		const FOnAttributeChangeData& OnAttributeChangeData
		);

	void SetStaminaChanged(
		float Value,
		float MaxValue
		);

	void OnManaChanged(
		const FOnAttributeChangeData& OnAttributeChangeData
		);

	void SetManaChanged(
		float Value,
		float MaxValue
		);

	void OnShieldChanged(
		const FOnAttributeChangeData& OnAttributeChangeData
		);

	void SetShieldChanged(
		float Value,
		float MaxValue
		);

	void ApplyCharaterNameToTitle();

	void ApplyStatesToTitle();

	void ApplyLevelToTitle();

	bool ResetPosition(
		float InDeltaTime
		);

private:
	void OnLevelChanged(
		int32 Level
		);

	FVector2D PreviousPt = FVector2D::ZeroVector;

	FVector2D DesiredPt = FVector2D::ZeroVector;

	FVector2D Size = FVector2D::ZeroVector;

	float Interval = 1.f / 30.f;

	float CurrentInterval = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	float Offset = 0.f;

	float HalfHeight = 0.f;

	float Scale = 1.f;

	FOnValueChangedDelegateHandle CurrentHPValueChanged;

	FOnValueChangedDelegateHandle MaxHPValueChanged;

	TArray<FOnValueChangedDelegateHandle> ValueChangedAry;

	FTSTicker::FDelegateHandle TickDelegateHandle;

	FDelegateHandle OnGameplayEffectTagCountChangedHandle;

	TSet<FGameplayTag> TagSet;

	TOnValueChangedCallbackContainer<uint8>::FCallbackHandleSPtr LevelChangedDelegateHandle;
};

UCLASS()
class PLANET_API UCharacterTitleBox :
	public UMyUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;

	virtual void NativeConstruct() override;

	void SetCampType(
		ECharacterCampType CharacterCampType
		);

	void SetData(
		ACharacterBase* CharacterPtr
		);

	void DisplaySentence(
		const FTaskNode_Conversation_SentenceInfo& Sentence
		);

	void CloseConversationborder();

	UPROPERTY(Transient)
	UConversationBorder* ConversationBorderPtr = nullptr;

protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI")
	TSubclassOf<UConversationBorder> ConversationBorderClass;

private:
	TObjectPtr<ACharacterBase> CharacterPtr = nullptr;
};
