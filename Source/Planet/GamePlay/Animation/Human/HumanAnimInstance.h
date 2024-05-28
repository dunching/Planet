
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include <GameplayTagContainer.h>

#include "Animation/AnimInstance.h"

#include "HumanAnimInstance.generated.h"

class UAbilitySystemComponent;

UENUM(BlueprintType)
enum class ERidingState_Anim : uint8
{
	kNone,
	kRequestMount,
	kMounted,
	kRequestDismount,
	kDismounted,
};

UCLASS(Config = Game)
class PLANET_API UHumanAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, Category = "Character State Data")
	ERidingState_Anim RidingState_Anim = ERidingState_Anim::kNone;

protected:

	UFUNCTION(BlueprintCallable)
	FQuat GetGravityToWorldTransform() const;

	UFUNCTION(BlueprintCallable, Category = "ASC")
	bool HasMatchingGameplayTag() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ASC")
	bool HasAnyMatchingGameplayTags() const;

	UFUNCTION(BlueprintPure, Category = "Character")
	virtual UPlanetAbilitySystemComponent* GetAbilitySystemComponent() const ;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadOnly, Category = "Character State Data")
	float GroundDistance = -1.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Character State Data")
	float HorseSpeed = 0.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "ASC")
	FGameplayTag TagToCheck;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "ASC")
	FGameplayTagContainer TagContainer;

};
