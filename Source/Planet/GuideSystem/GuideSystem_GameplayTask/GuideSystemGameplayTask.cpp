#include "GuideSystemGameplayTask.h"

#include "AssetRefMap.h"
#include "HumanCharacter_Player.h"
#include "TargetPoint_Runtime.h"

void UGameplayTask_Base::SetPlayerCharacter(AHumanCharacter_Player* InPlayerCharacterPtr)
{
	PlayerCharacterPtr = InPlayerCharacterPtr;
}

EStateTreeRunStatus UGameplayTask_Base::GetStateTreeRunStatus() const
{
	return StateTreeRunStatus;
}

UGameplayTask_MoveToLocation::UGameplayTask_MoveToLocation(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
	bIsPausable = true;
}

void UGameplayTask_MoveToLocation::Activate()
{
	Super::Activate();

	if (PlayerCharacterPtr)
	{
		FTransform AbsoluteTransform = FTransform::Identity;
		AbsoluteTransform.SetLocation(TargetLocation);
		
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = GetOwnerActor();
		
		TargetPointPtr = PlayerCharacterPtr->GetWorld()->SpawnActor<ATargetPoint_Runtime>(
			UAssetRefMap::GetInstance()->TargetPoint_RuntimeClass,
			AbsoluteTransform,
			SpawnParameters
		);
	}
}

void UGameplayTask_MoveToLocation::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if (FVector::Distance(TargetLocation, PlayerCharacterPtr->GetActorLocation()) < ReachedRadius)
	{
		StateTreeRunStatus = EStateTreeRunStatus::Succeeded;
		EndTask();
	}
}

void UGameplayTask_MoveToLocation::OnDestroy(bool bInOwnerFinished)
{
	if (TargetPointPtr)
	{
		TargetPointPtr->Destroy();
		TargetPointPtr = nullptr;
	}

	Super::OnDestroy(bOwnerFinished);
}

void UGameplayTask_MoveToLocation::SetUp(const FVector& InTargetLocation, int32 InReachedRadius)
{
	TargetLocation = InTargetLocation;
	ReachedRadius = InReachedRadius;
}
