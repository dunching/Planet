#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"

#include "TeamConfigure.h"

#include "TeamConfigureomponent.generated.h"

class APlanetPlayerState;

/*
 *	群组内每个Pawn分配的技能或天赋（包括玩家的Pawn）
 */
UCLASS(BlueprintType, Blueprintable)
class UTeamConfigureComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	using FOwnerType = APlanetPlayerState;

	static FName ComponentName;

	UTeamConfigureComponent(const FObjectInitializer& ObjectInitializer);

	void UpdateConfigure(const FTeamConfigure_FASI& Teammate_FASI);
	
	UPROPERTY(Replicated)
	FTeamConfigure_FASI_Container Teammate_FASI_Container;
	
protected:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void InitializeComponent()override;

};