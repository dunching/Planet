#include "ChildActorComponentBase.h"

#include "GeneratorBase.h"

FName UChildActorComponentBaseBase::ComponentName = TEXT("ChildActorComponentBaseBase");

UChildActorComponentBaseBase::UChildActorComponentBaseBase(
	const FObjectInitializer& ObjectInitializer
) :
  Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UChildActorComponentBaseBase::RespawnChildActor(
)
{
}
