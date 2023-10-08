
#include "EquipmentSocksComponent.h"

#include <variant>

#include <Subsystems/SubsystemBlueprintLibrary.h>

#include <CacheAssetManager.h>
#include "HumanCharacter.h"
#include "PlacingProcessor.h"
#include <InputProcessor.h>
#include <SceneObj/Building/BuildingBase.h>
#include <SceneObj/Equipment/EquipmentBase.h>
#include "ActionStairBase.h"
#include "PlacingCellProcessor.h"
#include "PlacingWallProcessor.h"
#include "PlacingGroundProcessor.h"
#include "ActionRoofBase.h"
#include "InputProcessorSubSystem.h"

UEquipmentSocksComponent::UEquipmentSocksComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{

}

void UEquipmentSocksComponent::UseItem(const FItemNum& Item)
{
}

void UEquipmentSocksComponent::DoAction(EEquipmentActionType ActionType)
{
	for (auto Iter : ItemsActionMap)
	{
		if (Iter.Value)
		{
			Iter.Value->DoAction(ActionType);
		}
	}
}

FName UEquipmentSocksComponent::ComponentName = TEXT("UEquipmentSocksComponent");

