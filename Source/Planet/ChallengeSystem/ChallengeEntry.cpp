#include "ChallengeEntry.h"

#include "CollisionDataStruct.h"
#include "GuideSubSystem.h"
#include "NiagaraComponent.h"
#include "Async/TaskGraphInterfaces.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/ArrowComponent.h"
#include "Components/BillboardComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/Texture2D.h"
#include "Components/BrushComponent.h"

#include "GuideThread.h"
#include "HumanCharacter_Player.h"
#include "TestCommand.h"

AChallengeEntry::AChallengeEntry(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));

	SceneActorInteractionComponentPtr = CreateDefaultSubobject<USceneActorInteractionComponent>(
		USceneActorInteractionComponent::ComponentName);
}

USceneActorInteractionComponent* AChallengeEntry::GetSceneActorInteractionComponent() const
{
	return SceneActorInteractionComponentPtr;
}

void AChallengeEntry::HasbeenInteracted(ACharacterBase* CharacterPtr)
{
}

void AChallengeEntry::HasBeenStartedLookAt(ACharacterBase* CharacterPtr)
{
}

void AChallengeEntry::HasBeenLookingAt(ACharacterBase* CharacterPtr)
{
}

void AChallengeEntry::HasBeenEndedLookAt()
{
}
