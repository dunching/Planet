#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Talent_FASI.h"
#include "GenerateType.h"

#include "TalentAllocationComponent.generated.h"

class IPlanetControllerInterface;
class ACharacterBase;
struct FPropertySettlementModify_Talent;

UCLASS(BlueprintType, Blueprintable)
class UTalentAllocationComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	using FOwnerType = ACharacterBase;

	static FName ComponentName;

};