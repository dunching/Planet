
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include <GameplayTagContainer.h>

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

	virtual void NativeBeginPlay()override;

	virtual void BeginDestroy() override;

	// 角度旋转还是不正确？不确定
	UFUNCTION(BlueprintImplementableEvent)
	void SetIsMelee(bool bIsMeele);

	UPROPERTY(BlueprintReadWrite, Category = "Character State Data")
	EAnimationType AnimationType = EAnimationType::kNormal;

protected:
	
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

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character State Data")
	int32 JogSpeed = 350;

	UPROPERTY(BlueprintReadOnly, Category = "Character State Data")
	float GroundDistance = -1.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Character State Data")
	float HorseSpeed = 0.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "ASC")
	FGameplayTag TagToCheck;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "ASC")
	FGameplayTagContainer TagContainer;

	FValueChangedDelegateHandle MoveSpeedChangedHandle;

};
