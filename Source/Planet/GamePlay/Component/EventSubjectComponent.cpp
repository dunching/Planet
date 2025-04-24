#include "EventSubjectComponent.h"

#include "OnEffectedTawrgetCallback.h"

FName UEventSubjectComponent::ComponentName = TEXT("EventSubjectComponent");

UEventSubjectComponent::UEventSubjectComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 1.f;

	SetIsReplicatedByDefault(true);
}

void UEventSubjectComponent::OnEffectOhterCharacter_Implementation(
	const FOnEffectedTawrgetCallback& ReceivedEventModifyDataCallback)
{
	MakedDamageDelegate(ReceivedEventModifyDataCallback);
}
