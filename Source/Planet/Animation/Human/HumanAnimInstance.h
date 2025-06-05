
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include <GameplayTagContainer.h>

#include "GameplayEffectTypes.h"
#include "Animation/AnimInstance.h"

#include "TemplateHelper.h"

#include "HumanAnimInstance.generated.h"

class UAbilitySystemComponent;
struct FOnAttributeChangeData;

UENUM(BlueprintType)
enum class EAnimationType : uint8
{
	kNormal,
	kMounted,
};

UCLASS(Config = Game)
class PLANET_API UHumanAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	using FValueChangedDelegateHandle =
		TOnValueChangedCallbackContainer<int32>::FCallbackHandleSPtr;

	virtual void InitializeWithAbilitySystem(UAbilitySystemComponent* ASC);

	virtual void NativeBeginPlay()override;

	virtual void BeginDestroy() override;

	// 角度旋转还是不正确？不确定
	UFUNCTION(BlueprintImplementableEvent)
	void SetIsMelee(bool bIsMeele);

	UPROPERTY(BlueprintReadWrite, Category = "Character State Data")
	EAnimationType AnimationType = EAnimationType::kNormal;

protected:
	
	virtual void NativeInitializeAnimation() override;
	
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	void OnMoveSpeedChanged(const FOnAttributeChangeData& CurrentValue);
	
	UFUNCTION(BlueprintImplementableEvent)
	void SetMoveSpeedChanged(int32 CurrentValue);
	
	UFUNCTION(BlueprintCallable)
	FQuat GetGravityToWorldTransform() const;

	UFUNCTION(BlueprintCallable, Category = "ASC")
	bool HasMatchingGameplayTag() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ASC")
	bool HasAnyMatchingGameplayTags() const;
	
	UFUNCTION(BlueprintPure, Category = "Character")
	virtual UPlanetAbilitySystemComponent* GetAbilitySystemComponent() const ;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character State Data")
	int32 JogSpeed = 300;

	UPROPERTY(BlueprintReadOnly, Category = "Character State Data")
	float HorseSpeed = 0.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "ASC")
	FGameplayTag TagToCheck;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "ASC")
	FGameplayTagContainer TagContainer;

	FValueChangedDelegateHandle MoveSpeedChangedHandle;

#pragma region Lyra 数据
	// Gameplay tags that can be mapped to blueprint variables. The variables will automatically update as the tags are added or removed.
	// These should be used instead of manually querying for the gameplay tags.
	UPROPERTY(EditDefaultsOnly, Category = "GameplayTags")
	FGameplayTagBlueprintPropertyMap GameplayTagPropertyMap;

	UPROPERTY(BlueprintReadOnly, Category = "Character State Data")
	float GroundDistance = -1.0f;
#pragma endregion
};
