// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Animation/AnimInstance.h"
#include "AnimInstanceDataStruct.h"
#include <functional>
#include "GenerateType.h"

#include "AnimInstanceBase.generated.h"

/**
 *
 */
UCLASS()
class PLANET_API UAnimInstanceBase : public UAnimInstance
{
	GENERATED_BODY()

public:

	using FAnimationNotify = std::function<void(EAnimationNotify)>;

	virtual void NativeBeginPlay()override;

	void ExcuteAnimCMD(EAnimCMDType NewAnimCMDType);

	UFUNCTION(BlueprintImplementableEvent, Category = Character)
		void StartCurrentAnimation();

	UFUNCTION(BlueprintCallable, Category = Character)
		void OnAnimationNotify(EAnimationNotify AnimationNotify);

	void ClearCB();

	void AddAnimationNotify(const FAnimationNotify& NewAttackIsStartCB);

protected:

	virtual void NativeUpdateAnimation(float DeltaSeconds)override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimCMDType")
		EAnimCMDType AnimCMDType = EAnimCMDType::kNone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
		float Forward = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
		float Right = 0;

	TArray<FAnimationNotify> AnimationNotifyAry;

};
