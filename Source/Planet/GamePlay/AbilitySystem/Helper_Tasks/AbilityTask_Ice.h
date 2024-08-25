// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "Abilities/Tasks/AbilityTask_ApplyRootMotion_Base.h"
#include "Engine/EngineTypes.h"
#include "UObject/ObjectMacros.h"

#include "Skill_Active_IceGun.h"
#include "AbilityTask_Ice.generated.h"

class UGameplayAbility;
class ATornado;
class ACharacterBase;

DECLARE_DELEGATE(FOnTaskFinished);

UCLASS()
class PLANET_API UAbilityTask_Ice : public UAbilityTask_ApplyRootMotion_Base
{
	GENERATED_BODY()

public:

	UAbilityTask_Ice(const FObjectInitializer& ObjectInitializer);

	static UAbilityTask_Ice* NewTask(
		UGameplayAbility* OwningAbility,
		FName TaskInstanceName,
		TWeakObjectPtr<ASkill_IceGun_Projectile> InTornadoPtr,
		ACharacterBase* InTargetCharacterPtr
	);

	virtual void Activate() override;

	virtual void OnDestroy(bool AbilityIsEnding) override;

	virtual void TickTask(float DeltaTime) override;

	FOnTaskFinished OnFinish;

protected:

	virtual void SharedInitAndApply() override;

protected:

	TWeakObjectPtr<ASkill_IceGun_Projectile> IceGunPtr = nullptr;

	ACharacterBase* TargetCharacterPtr = nullptr;

};
