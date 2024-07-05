
#include "PlanetChildActorComponent.h"

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
	}
}

