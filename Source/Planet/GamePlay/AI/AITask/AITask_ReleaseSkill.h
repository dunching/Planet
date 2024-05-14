
// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "UObject/ObjectMacros.h"
#include "Engine/EngineTypes.h"
#include "AITypes.h"
#include "Navigation/PathFollowingComponent.h"
#include "Tasks/AITask.h"

#include "AITask_ReleaseSkill.generated.h"

class UGameplayAbility;

class ACharacterBase;

UCLASS()
class PLANET_API UAITask_ReleaseSkill : public UAITask
{
	GENERATED_BODY()

public:

	UAITask_ReleaseSkill(const FObjectInitializer& ObjectInitializer);

	void ConditionalPerformMove();

	bool WasMoveSuccessful() const;

	void SetUp(ACharacterBase* InChracterPtr);

protected:

	bool bIsReleasingSkill = false;

	ACharacterBase* CharacterPtr = nullptr;

	virtual void Activate() override;

	virtual void OnDestroy(bool bOwnerFinished) override;

	virtual void PerformMove();

	void OnOnGameplayAbilityEnded(UGameplayAbility* GAPtr);

};
