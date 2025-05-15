// Copyright Epic Games, Inc. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/ObjectMacros.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"

#include "PlanetAnimNotifyState.generated.h"

class UAnimSequenceBase;
class USkeletalMeshComponent;
class USoundBase;

class ACharacterBase;

/**
 * 此期间不允许角色移动
 * 为什么不使用 UAnimNotifyState_DisableRootMotion？因为UAnimNotifyState_DisableRootMotion仅覆盖了Velocity
 * 未覆盖加速度，导致Lyra的动画表现不正确
 */
UCLASS()
class PLANET_API UAnimNotifyState_AddTagInDuration : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	using FOwnerPawnType = ACharacterBase;

	virtual void NotifyBegin(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		float TotalDuration,
		const FAnimNotifyEventReference& EventReference
		) override;

	virtual void NotifyEnd(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference
		) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tags)
	FGameplayTagContainer ActivedTags;
};
