#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"

#include "TeamConfigure.h"

#include "TeamConfigureomponent.generated.h"

class APlanetPlayerState;

UCLASS(BlueprintType, Blueprintable)
class UTeamConfigureComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	using FOwnerType = APlanetPlayerState;

	static FName ComponentName;

	UTeamConfigureComponent(const FObjectInitializer& ObjectInitializer);

	void UpdateConfigure(const FTeammate_FASI& Teammate_FASI);

protected:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(Replicated)
	FTeammate_FASI_Container Teammate_FASI_Container;
	
};