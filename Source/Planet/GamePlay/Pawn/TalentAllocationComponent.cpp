
#include "TalentAllocationComponent.h"

#include "Net/UnrealNetwork.h"

#include "CharacterBase.h"
#include "ItemProxy_Minimal.h"
#include "InventoryComponent.h"
#include "CharacterAttributesComponent.h"
#include "CharacterAttibutes.h"
#include "ProxyProcessComponent.h"
#include "AssetRefMap.h"
#include "GameplayTagsLibrary.h"
#include "ItemProxy_Container.h"
#include "PlanetControllerInterface.h"
#include "CharacterAbilitySystemComponent.h"
#include "DataTableCollection.h"
#include "TalentInfo.h"

FName UTalentAllocationComponent::ComponentName = TEXT("TalentAllocationComponent");
