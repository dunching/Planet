
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Components/StateTreeComponent.h"

#include "QuestSystemStateTreeComponent.generated.h"

class UStateTreeComponent;

/**
 *
 */
UCLASS()
class QUESTSYSTEM_API UQuestSystemStateTreeComponent : public UStateTreeComponent
{
	GENERATED_BODY()

public:
	
	static FName ComponentName;
	
	UQuestSystemStateTreeComponent(const FObjectInitializer& ObjectInitializer);

	virtual UGameplayTasksComponent* GetGameplayTasksComponent(const UGameplayTask& Task) const override;

	virtual void StartLogic() override;

	virtual void StopLogic(const FString& Reason)  override;
	
};
