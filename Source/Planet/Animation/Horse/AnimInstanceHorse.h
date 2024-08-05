// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstanceBase.h"
#include "AnimInstanceHorse.generated.h"

/**
 *
 */
UCLASS()
class PLANET_API UAnimInstanceHorse : public UAnimInstanceBase
{
	GENERATED_BODY()

public:

	using FAnimationNotifyCV = std::function<void()>;

	void SetIsCrounch(bool IsCrounch);;

	void SetPitch(float Val);;

protected:

	virtual void NativeUpdateAnimation(float DeltaSeconds)override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
		bool bIsInAir = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
		bool bIsCrounch = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
		float Pitch = 0.f;

private:

};