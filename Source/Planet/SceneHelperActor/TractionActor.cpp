#include "TractionActor.h"

#include "Components/SplineComponent.h"
#include "Engine/OverlapResult.h"
#include "Net/UnrealNetwork.h"

#include "CharacterAbilitySystemComponent.h"
#include "CollisionDataStruct.h"
#include "HumanCharacter.h"
#include "Skill_Active_Traction.h"

static TAutoConsoleVariable<int32> SkillDrawDebugTractionPoint(
	TEXT("Skill.DrawDebug.TractionPoint"),
	0,
	TEXT("")
	TEXT(" default: 0")
);

ATractionPoint::ATractionPoint(
	const FObjectInitializer& ObjectInitializer
):
 Super(ObjectInitializer)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicatingMovement(true);
}

void ATractionPoint::BeginPlay()
{
	Super::BeginPlay();

	if (!ItemProxy_DescriptionPtr)
	{
		return;
	}
	
	SetLifeSpan(ItemProxy_DescriptionPtr->Duration.PerLevelValue[0]);

	CheckTarget();
}

void ATractionPoint::Tick(
	float DeltaTime
)
{
	Super::Tick(DeltaTime);
	
#if UE_EDITOR || UE_SERVER
	if (GetLocalRole() == ROLE_Authority)
	{
		CurrentTime += DeltaTime;
		if (CurrentTime > ItemProxy_DescriptionPtr->Inverval)
		{
			CurrentTime = 0.f;

			CheckTarget();
		}
	}
#endif
}

void ATractionPoint::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, ItemProxy_DescriptionPtr, COND_InitialOnly);
}

void ATractionPoint::CheckTarget()
{
	TArray<FOverlapResult> OutOverlaps;
	FCollisionObjectQueryParams ObjectQueryParams;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	ObjectQueryParams.AddObjectTypesToQuery(Pawn_Object);
	GetWorld()->OverlapMultiByObjectType(
		OutOverlaps,
		GetActorLocation(),
		FQuat::Identity,
		ObjectQueryParams,
		FCollisionShape::MakeSphere(ItemProxy_DescriptionPtr->OuterRadius),
		Params
	);

#ifdef WITH_EDITOR
	if (SkillDrawDebugTractionPoint.GetValueOnGameThread())
	{
		DrawDebugSphere(GetWorld(), GetActorLocation(), ItemProxy_DescriptionPtr->OuterRadius, 20, FColor::Red, false, 3);
	}
#endif

	for (const auto& Iter : OutOverlaps)
	{
		auto TargetCharacterPtr = Cast<AHumanCharacter>(Iter.GetActor());
		if (TargetCharacterPtr)
		{
			auto OwnerCharacterPtr = GetOwner<FOwnerType>();
			if (TargetCharacterPtr == OwnerCharacterPtr)
			{
				return;
			}
			if (OwnerCharacterPtr->IsGroupmate(TargetCharacterPtr))
			{
				return;
			}
			if (TargetCharacterPtr->GetCharacterAbilitySystemComponent()->IsCantBeDamage())
			{
				return;
			}


			TargetCharacterPtr->GetCharacterAbilitySystemComponent()->HasbeenTraction(this);
		}
	}
}
