
#include "ResourceBoxBase.h"

#include <Components/SceneComponent.h>

#include "KismetGravityLibrary.h"

#include "HoldingItemsComponent.h"
#include "CharacterBase.h"
#include "PlanetPlayerState.h"

AResourceBoxBase::AResourceBoxBase(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));

	HoldingItemsComponentPtr = CreateDefaultSubobject<UHoldingItemsComponent>(UHoldingItemsComponent::ComponentName);
}

void AResourceBoxBase::BeginPlay()
{
	Super::BeginPlay();

	FVector Start = RootComponent->GetComponentLocation();
	FVector End = Start + (UKismetGravityLibrary::GetGravity(RootComponent->GetComponentLocation()) * 1000.f);

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	FHitResult OutHit;
	if (GetWorld()->LineTraceSingleByObjectType(OutHit, Start, End, ObjectQueryParams, Params))
	{
		if (FVector::Distance(OutHit.ImpactPoint, Start) > 10.f)
		{
			SetActorLocation(OutHit.ImpactPoint);
		}
	}

	for (const auto Iter : SkillUnitMap)
	{
		HoldingItemsComponentPtr->GetHoldItemProperty().AddUnit_Skill(Iter.Key);
	}
	for (const auto Iter : SkillUnitMap)
	{
		HoldingItemsComponentPtr->GetHoldItemProperty().AddUnit_Skill(Iter.Key);
	}
}

void AResourceBoxBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AResourceBoxBase::Interaction(ACharacterBase* InCharacterPtr)
{
	TargetCharacterPtr = InCharacterPtr;
}

void AResourceBoxBase::AddItemsToTarget()
{
	if (TargetCharacterPtr)
	{
		{
			auto& HoldItemPropertyRef = TargetCharacterPtr->GetHoldingItemsComponent()->GetHoldItemProperty();
			for (const auto Iter : ConsumableUnitMap)
			{
				HoldItemPropertyRef.AddUnit_Consumable(Iter.Key, Iter.Value);
			}
			for (const auto Iter : ToolUnitMap)
			{
			}
			for (const auto Iter : WeaponUnitMap)
			{
			}

#if WITH_EDITORONLY_DATA
			for (const auto Iter : SkillUnitMap)
			{
				HoldItemPropertyRef.AddUnit_Skill(Iter.Key);
			}
#endif
		}
		{
			auto& HoldItemPropertyRef = TargetCharacterPtr->GetPlayerState<APlanetPlayerState>()->GetHoldingItemsComponent()->GetHoldItemProperty();
			for (const auto Iter : CoinUnitMap)
			{
				HoldItemPropertyRef.AddUnit_Coin(Iter.Key, Iter.Value);
			}
		}
	}
}

