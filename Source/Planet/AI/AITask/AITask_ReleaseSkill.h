
// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "UObject/ObjectMacros.h"
#include "Engine/EngineTypes.h"
#include "AITypes.h"
#include "Navigation/PathFollowingComponent.h"
#include "Tasks/AITask.h"
#include "UnitProxyProcessComponent.h"

#include "AITask_ReleaseSkill.generated.h"

struct FSocket_FASI;
class UGameplayAbility;

class ACharacterBase;

UCLASS()
class PLANET_API UAITask_ReleaseSkill : public UAITask
{
	GENERATED_BODY()

public:

	UAITask_ReleaseSkill(const FObjectInitializer& ObjectInitializer);

	void ConditionalPerformTask();

	void SetUp(ACharacterBase* InChracterPtr);

	bool bIsPauseRelease = true;

protected:

	virtual void Activate() override;

	virtual void OnDestroy(bool bOwnerFinished) override;
	
	UFUNCTION()
	bool PerformTask(float Delta);

	void StopReleaseSkill();

	void OnOnGameplayAbilityEnded(UGameplayAbility* GAPtr);

	TMap<UGameplayAbility*, TSharedPtr<FSocket_FASI>>ReleasingSkillMap;

	ACharacterBase* CharacterPtr = nullptr;

	FTSTicker::FDelegateHandle TickDelegateHandle;

	const float Frequency = 1.f;

};
