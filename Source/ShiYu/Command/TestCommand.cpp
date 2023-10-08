
#include "TestCommand.h"

#include "Kismet/GameplayStatics.h"

#include "HumanCharacter.h"
#include "GameInstance/ShiYuGameInstance.h"
#include "Pawn/HoldItemComponent.h"
#include "AssetRefMap.h"
#include "Engine/StreamableManager.h"
#include "Blueprint/UserWidget.h"
#include "Engine/AssetManager.h"
#include <AssetRefrencePath.h>
#include <Subsystems/SubsystemBlueprintLibrary.h>
#include "ShiYu.h"
#include "HumanCharacter.h"

void TestCommand::AddCahracterTestData()
{
	auto CharacterPtr = Cast<AHumanCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorldImp(), 0));
	if(!CharacterPtr)
	{
		return;
	}
	AddCharacterTestDataImp(CharacterPtr);
}

void TestCommand::AddCharacterTestDataImp(AHumanCharacter* CharacterPtr)
{
	auto HoldItemComponent = CharacterPtr->GetHoldItemComponent()->GetHoldItemProperty();

	FItemAry NewItemMap;
	{
		FItemNum ItemNum;
		ItemNum.ItemType.Enumtype = EEnumtype::kBuilding;
		ItemNum.ItemType.ItemType = EBuildingType::kStoneGround;
		ItemNum.Num = 30;
		NewItemMap.ItemsAry.Add(ItemNum);
	}
	{
		FItemNum ItemNum;
		ItemNum.ItemType.Enumtype = EEnumtype::kBuilding;
		ItemNum.ItemType.ItemType = EBuildingType::kWoodGroundStyle1;
		ItemNum.Num = 30;
		NewItemMap.ItemsAry.Add(ItemNum);
	}
	{
		FItemNum ItemNum;
		ItemNum.ItemType.Enumtype = EEnumtype::kBuilding;
		ItemNum.ItemType.ItemType = EBuildingType::kWoodGroundStyle2;
		ItemNum.Num = 30;
		NewItemMap.ItemsAry.Add(ItemNum);
	}
	{
		FItemNum ItemNum;
		ItemNum.ItemType.Enumtype = EEnumtype::kBuilding;
		ItemNum.ItemType.ItemType = EBuildingType::kWoodWall;
		ItemNum.Num = 30;
		NewItemMap.ItemsAry.Add(ItemNum);
	}
	{
		FItemNum ItemNum;
		ItemNum.ItemType.Enumtype = EEnumtype::kBuilding;
		ItemNum.ItemType.ItemType = EBuildingType::kWoodCell;
		ItemNum.Num = 30;
		NewItemMap.ItemsAry.Add(ItemNum);
	}
	HoldItemComponent->InitItemMap(NewItemMap);
}

void TestCommand::TestAsyncAssetLoad()
{
}

void TestCommand::TestSubSystem()
{
// 	auto Class = LoadClass<UAssetRefMap>(nullptr, *AssetRefMapClass.ToString());
// 	auto CacheAssetManagerPtr = USubsystemBlueprintLibrary::GetEngineSubsystem(Class);

	UE_LOG(LogTemp, Warning, TEXT("123"));
}

