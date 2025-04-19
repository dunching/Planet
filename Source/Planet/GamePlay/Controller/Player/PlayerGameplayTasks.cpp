#include "PlayerGameplayTasks.h"

#include "OpenWorldSystem.h"
#include "PlanetPlayerController.h"
#include "Teleport.h"

UGameplayTask_TeleportPlayer::UGameplayTask_TeleportPlayer(
	const FObjectInitializer& ObjectInitializer
):
 Super(ObjectInitializer)
{
	bTickingTask = true;
	bIsPausable = true;
}

void UGameplayTask_TeleportPlayer::Activate()
{
	Super::Activate();

	auto Teleport = UOpenWorldSubSystem::GetInstance()->GetTeleportPlayerToNearest(TargetPCPtr);
	
	UOpenWorldSubSystem::GetInstance()->SwitchDataLayer(Teleport, TargetPCPtr);

	Target = UOpenWorldSubSystem::GetInstance()->GetTeleport(Teleport);
	
	Target.LoadSynchronous();
}

void UGameplayTask_TeleportPlayer::TickTask(
	float DeltaTime
)
{
	Super::TickTask(DeltaTime);

	const auto Distance = FVector::Dist2D(Target->GetLandTransform().GetLocation(), TargetPCPtr->GetPawn()->GetActorLocation());
	if (Distance < DistanceThreshold)
	{
		bIsSuccessful = true;
		EndTask();
	}
}

void UGameplayTask_TeleportPlayer::OnDestroy(
	bool bInOwnerFinished
)
{
	OnEnd.Broadcast(bIsSuccessful);
	
	Super::OnDestroy(bInOwnerFinished);
}
