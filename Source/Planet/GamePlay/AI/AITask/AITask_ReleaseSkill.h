
// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "UObject/ObjectMacros.h"
#include "Engine/EngineTypes.h"
#include "AITypes.h"
#include "Navigation/PathFollowingComponent.h"
#include "Tasks/AITask.h"
#include "EquipmentElementComponent.h"

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

	virtual void Activate() override;

	virtual void OnDestroy(bool bOwnerFinished) override;

	virtual void PerformMove();

	void OnOnGameplayAbilityEnded(UGameplayAbility* GAPtr);

	bool ReleasingSKill();

	TMap<FGameplayAbilitySpecHandle, FSkillsSocketInfo>ReleasingSkillMap;

	TMap<FGameplayAbilitySpecHandle, FDelegateHandle>ReleasingSkillDelegateMap;

	ACharacterBase* CharacterPtr = nullptr;

};
