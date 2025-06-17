#include "Planet_ItemProxy.h"

#include "InventoryComponent.h"
#include "ItemProxy.h"
#include "PlanetWorldSettings.h"
#include "DataTableCollection.h"
#include "ItemProxyVisitorBase.h"
#include "Tools.h"

inline void FPlanet_BasicProxy::SetInventoryComponent(
	const TObjectPtr<UInventoryComponent>& NewInventoryComponentPtr
	)
{
	SetInventoryComponentBase(NewInventoryComponentPtr);
}

TObjectPtr<UInventoryComponent> FPlanet_BasicProxy::GetInventoryComponent() const
{
	return Cast<UInventoryComponent>(GetInventoryComponentBase());
}
