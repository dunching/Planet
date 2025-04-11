#include "AreaVolume.h"

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

AAreaVolume::AAreaVolume(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	// SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);

	GetBrushComponent()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetBrushComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetBrushComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetBrushComponent()->SetCollisionResponseToChannel(Pawn_Object, ECollisionResponse::ECR_Overlap);
}

void AAreaVolume::BeginPlay()
{
	Super::BeginPlay();

#if UE_EDITOR || UE_CLIENT
	if (GetLocalRole() != ROLE_Authority)
	{
		OnActorBeginOverlap.Clear();
		OnActorEndOverlap.Clear();
		
		OnActorBeginOverlap.AddDynamic(this, &ThisClass::OnActorBeginOverlapCB);
		OnActorEndOverlap.AddDynamic(this, &ThisClass::OnActorEndOverlapCB);

		UpdateOverlaps();
	}
#endif
}

void AAreaVolume::OnActorBeginOverlapCB(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor->IsA(AHumanCharacter_Player::StaticClass()))
	{
		auto CharacterPtr = Cast<AHumanCharacter_Player>(OtherActor);
		const auto bIsLocallyControlled = CharacterPtr->IsLocallyControlled();
		if (bIsLocallyControlled)
		{
			UGuideSubSystem::GetInstance()->ActiveTargetGuideThread(GuideBranchThreadClass, true);
		}
		// FActorSpawnParameters SpawnParameters;
		//
		// GuidePtr = GetWorld()->SpawnActor<AGuideThread_Branch>(
		// 	GuideBranchThreadClass, SpawnParameters
		// );
	}
}

void AAreaVolume::OnActorEndOverlapCB(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor->IsA(AHumanCharacter_Player::StaticClass()))
	{
		auto CharacterPtr = Cast<AHumanCharacter_Player>(OtherActor);
		const auto bIsLocallyControlled = CharacterPtr->IsLocallyControlled();
		if (bIsLocallyControlled)
		{
			// UGuideSubSystem::GetInstance()->StopActiveTargetGuideThread(GuideBranchThreadClass);
		}
	}
}
