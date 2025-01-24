#include "EventSubjectComponent.h"

FName UEventSubjectComponent::ComponentName = TEXT("EventSubjectComponent");

UEventSubjectComponent::UEventSubjectComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 1.f;

	SetIsReplicatedByDefault(true);
}

void UEventSubjectComponent::ReceivedEventModifyDataCallback_Implementation(
	const FReceivedEventModifyDataCallback& ReceivedEventModifyDataCallback)
{
	MakedDamageDelegate(ReceivedEventModifyDataCallback);
}
