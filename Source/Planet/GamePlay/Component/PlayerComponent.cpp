
#include "PlayerComponent.h"

#include "GeneratorColony.h"
#include "CharacterBase.h"
#include "GroupManagger.h"
#include "HumanAIController.h"
#include "HumanCharacter_Player.h"

FName UPlayerComponent::ComponentName = TEXT("PlayerComponent");

UPlayerComponent::UPlayerComponent(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
}

void UPlayerComponent::BeginPlay()
{
	Super::BeginPlay();

}

bool UPlayerComponent::TeleportTo(
	const FVector& DestLocation,
	const FRotator& DestRotation,
	bool bIsATest,
	bool bNoCheck
	)
{
	auto OnwerActorPtr = GetOwner<FOwnerType>();
	if (!OnwerActorPtr)
	{
		return false;
	}

	// TODO 使用EQS查询位置
	OnwerActorPtr->GetGroupSharedInfo()->GetTeamMatesHelperComponent()->TeleportTo(DestLocation, DestRotation, bIsATest, bNoCheck);
	
	return true;
}
