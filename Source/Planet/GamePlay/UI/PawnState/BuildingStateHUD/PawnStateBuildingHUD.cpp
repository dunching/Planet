
#include "PawnStateBuildingHUD.h"

#include "Kismet/GameplayStatics.h"

#include "PlanetPlayerState.h"
#include "Planet.h"
#include "CharacterAttributesComponent.h"
#include "CharacterBase.h"
#include "ToolsMenu.h"
#include "ToolsIcon.h"
#include "AssetRefMap.h"

const FName Tool1 = TEXT("Tool1");

const FName Tool2 = TEXT("Tool2");

const FName Tool3 = TEXT("Tool3");

const FName Tool4 = TEXT("Tool4");

const FName Tool5 = TEXT("Tool5");

const FName Tool6 = TEXT("Tool6");

void UPawnStateBuildingHUD::NativeConstruct()
{
	Super::NativeConstruct();

	ResetUIByData();
}

void UPawnStateBuildingHUD::NativeDestruct()
{
	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!CharacterPtr)
	{
		return;
	}
	auto EICPtr = CharacterPtr->GetEquipmentItemsComponent();
	{
		auto Ary = GetEquipMenus();
		for (auto& Iter : Ary)
		{
		//	EICPtr->RegisterTool(Iter);
		}
	}

	Super::NativeDestruct();
}

void UPawnStateBuildingHUD::ResetUIByData()
{
	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!CharacterPtr)
	{
		return;
	}
	auto EICPtr = CharacterPtr->GetEquipmentItemsComponent();
	{
		TArray<FName>Ary
		{
			Tool1,
			Tool2,
			Tool3,
			Tool4,
			Tool5,
			Tool6,
		};

		for (const auto& Iter : Ary)
		{
			auto IconPtr = Cast<UToolIcon>(GetWidgetFromName(Iter));
			if (IconPtr)
			{
				auto Result = EICPtr->FindTool(IconPtr->IconSocket);
				if (Result->ToolUnitPtr)
				{
				}
				IconPtr->ResetToolUIByData(Result->ToolUnitPtr);
			}
		}
	}
}

UToolsMenu* UPawnStateBuildingHUD::GetEquipMenu()
{
	return Cast<UToolsMenu>(GetWidgetFromName(ItemMenu));
}

TArray<FToolsSocketInfo> UPawnStateBuildingHUD::GetEquipMenus()
{
	TArray<FToolsSocketInfo>Result;

	auto UIPtr = Cast<UToolIcon>(GetWidgetFromName(Tool1));
	if (UIPtr)
	{
		FToolsSocketInfo ToolsSocketInfo;

		ToolsSocketInfo.Key = ToolSocket1;
		ToolsSocketInfo.SkillSocket = UIPtr->IconSocket;
		ToolsSocketInfo.ToolUnitPtr = UIPtr->GetToolUnit();

		Result.Add(ToolsSocketInfo);
	}

	return Result;
}
