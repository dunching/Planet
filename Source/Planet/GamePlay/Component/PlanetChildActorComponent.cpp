
#include "PlanetChildActorComponent.h"

#include "CharacterBase.h"
#include "BuildingBase.h"
#include "BuildingArea.h"
#include "HumanCharacter.h"
#include "HumanAIController.h"

void UPlanetChildActorComponent::CreateChildActor(TFunction<void(AActor*)> CustomizerFunc /*= nullptr*/)
{
	Super::CreateChildActor(CustomizerFunc);
	if (GetChildActor() != nullptr)
	{
		if (USceneComponent* ChildRoot = GetChildActor()->GetRootComponent())
		{
			TGuardValue<TEnumAsByte<EComponentMobility::Type>> MobilityGuard(ChildRoot->Mobility, Mobility);
			ChildRoot->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		}

		if (GetChildActor()->IsA(AHumanCharacter::StaticClass()))
		{
		}
		else if (GetChildActor()->IsA(ABuildingArea::StaticClass()))
		{
			FHitResult Result;

			FCollisionObjectQueryParams ObjectQueryParams;
			ObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic);
			ObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldDynamic);

			FCollisionQueryParams Params;
			Params.bTraceComplex = false;
			Params.AddIgnoredActor(GetChildActor());

			auto StartPt = GetChildActor()->GetActorLocation();
			auto StopPt = GetChildActor()->GetActorLocation() + (FVector::DownVector * 1000.f);

			if (GetWorld()->LineTraceSingleByObjectType(
				Result,
				StartPt,
				StopPt,
				ObjectQueryParams,
				Params)
				)
			{
				GetChildActor()->SetActorLocation(Result.ImpactPoint);
			}
		}
		else if (GetChildActor()->IsA(ABuildingBase::StaticClass()))
		{

		}
	}
}

