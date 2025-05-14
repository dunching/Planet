
#include "PawnStateBuildingHUD.h"

#include "Kismet/GameplayStatics.h"

#include "PlanetPlayerState.h"
#include "Planet.h"
#include "CharacterAttributesComponent.h"

#include "CharacterBase.h"
#include "ToolsMenu.h"
#include "ToolsIcon.h"
#include "AssetRefMap.h"

namespace PawnStateBuildingHUD
{
	const FName EquipIcon1 = TEXT("EquipIcon1");

	const FName EquipIcon2 = TEXT("EquipIcon2");

	const FName EquipIcon3 = TEXT("EquipIcon3");

	const FName EquipIcon4 = TEXT("EquipIcon4");

	const FName EquipIcon5 = TEXT("EquipIcon5");

	const FName EquipIcon6 = TEXT("EquipIcon6");

	const FName EquipIcon7 = TEXT("EquipIcon7");

	const FName EquipIcon8 = TEXT("EquipIcon8");
}

void UPawnStateBuildingHUD::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPawnStateBuildingHUD::NativeDestruct()
{
	ON_SCOPE_EXIT
	{
		Super::NativeDestruct();
	};

	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!CharacterPtr)
	{
		return;
	}
	{
//		auto EICPtr = CharacterPtr->GetInteractiveToolComponent();
	//	const auto Result = GetTools();
//		EICPtr->RegisterTool(Result);
	}
	{
	//	auto EICPtr = CharacterPtr->GetInteractiveConsumablesComponent();
	//	const auto Result = GetConsumables();
//		EICPtr->RegisterConsumable(Result);
	}
}

void UPawnStateBuildingHUD::Enable()
{
	ILayoutItemInterfacetion::Enable();
	
	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!CharacterPtr)
	{
		return;
	}
	{
		TArray<FName>Ary
		{
			PawnStateBuildingHUD::EquipIcon1,
			PawnStateBuildingHUD::EquipIcon2,
			PawnStateBuildingHUD::EquipIcon3,
			PawnStateBuildingHUD::EquipIcon4,
			PawnStateBuildingHUD::EquipIcon5,
			PawnStateBuildingHUD::EquipIcon6,
			PawnStateBuildingHUD::EquipIcon7,
			PawnStateBuildingHUD::EquipIcon8,
		};

		for (const auto& Iter : Ary)
		{
			auto IconPtr = Cast<UToolIcon>(GetWidgetFromName(Iter));
			if (IconPtr)
			{
				IconPtr->ResetToolUIByData(nullptr);
			}
		}
		{
			// 			auto EICPtr = CharacterPtr->GetInteractiveToolComponent();
			// 			for (const auto& Iter : Ary)
			// 			{
			// 				auto IconPtr = Cast<UToolIcon>(GetWidgetFromName(Iter));
			// 				if (IconPtr)
			// 				{
			// 					auto Result = EICPtr->FindTool(IconPtr->IconSocket);
			// 					if (Result && Result->ProxyPtr)
			// 					{
			// //						IconPtr->ResetToolUIByData(Result->ProxyPtr);
			// 					}
			// 				}
			// 			}
		}
		{
			// 			auto EICPtr = CharacterPtr->GetInteractiveConsumablesComponent();
			// 			for (const auto& Iter : Ary)
			// 			{
			// 				auto IconPtr = Cast<UToolIcon>(GetWidgetFromName(Iter));
			// 				if (IconPtr)
			// 				{
			// 					auto Result = EICPtr->FindConsumable(IconPtr->IconSocket);
			// 					if (Result && Result->ProxyPtr)
			// 					{
			// 		//				IconPtr->ResetToolUIByData(Result->ProxyPtr);
			// 					}
			// 				}
			// 			}
		}
		for (const auto& FirstIter : Ary)
		{
			for (const auto& SecondIter : Ary)
			{
				if (FirstIter == SecondIter)
				{
					continue;
				}
				auto FirstPtr = Cast<UToolIcon>(GetWidgetFromName(FirstIter));
				auto SecondPtr = Cast<UToolIcon>(GetWidgetFromName(SecondIter));
				if (FirstPtr && SecondPtr)
				{
					auto Result = SecondPtr->OnResetProxy.AddCallback(
						std::bind(&UToolIcon::OnSublingIconReset, FirstPtr, std::placeholders::_1));
					Result->bIsAutoUnregister = false;
				}
			}
		}
	}
}

void UPawnStateBuildingHUD::DisEnable()
{
	ILayoutItemInterfacetion::DisEnable();
}

UToolsMenu* UPawnStateBuildingHUD::GetEquipMenu()
{
	return Cast<UToolsMenu>(GetWidgetFromName(ItemMenu));
}
// 
// TMap <FGameplayTag, TSharedPtr<FToolsSocketInfo>> UPawnStateBuildingHUD::GetTools()
// {
// 	TMap <FGameplayTag, TSharedPtr<FToolsSocketInfo>>Result;
// 
// 	TArray<TTuple<FKey, FName>>Ary
// 	{
// 		{ToolSocket1, PawnStateBuildingHUD::EquipIcon1},
// 		{ToolSocket2, PawnStateBuildingHUD::EquipIcon2},
// 		{ToolSocket3, PawnStateBuildingHUD::EquipIcon3},
// 		{ToolSocket4, PawnStateBuildingHUD::EquipIcon4},
// 		{ToolSocket5, PawnStateBuildingHUD::EquipIcon5},
// 		{ToolSocket6, PawnStateBuildingHUD::EquipIcon6},
// 		{ToolSocket7, PawnStateBuildingHUD::EquipIcon7},
// 		{ToolSocket8, PawnStateBuildingHUD::EquipIcon8},
// 	};
// 
// 	for (const auto& Iter : Ary)
// 	{
// 		auto UIPtr = Cast<UToolIcon>(GetWidgetFromName(Iter.Get<1>()));
// 		if (UIPtr)
// 		{
// 			TSharedPtr<FToolsSocketInfo> SocketInfoSPtr = MakeShared<FToolsSocketInfo>();
// 
// // 			SocketInfoSPtr->Key = Iter.Get<0>();
// // 			SocketInfoSPtr->SkillSocket = UIPtr->IconSocket;
// // //			SocketInfoSPtr->ProxyPtr = UIPtr->GetToolProxy();
// // 
// // 			Result.Add(SocketInfoSPtr->SkillSocket, SocketInfoSPtr);
// 		}
// 	}
// 	return Result;
// }
// 
// TMap<FGameplayTag, TSharedPtr<FConsumableSocketInfo>> UPawnStateBuildingHUD::GetConsumables()
// {
// 	TMap <FGameplayTag, TSharedPtr<FConsumableSocketInfo>>Result;
// 
// 	TArray<TTuple<FKey, FName>>Ary
// 	{
// 		{ToolSocket1, PawnStateBuildingHUD::EquipIcon1},
// 		{ToolSocket2, PawnStateBuildingHUD::EquipIcon2},
// 		{ToolSocket3, PawnStateBuildingHUD::EquipIcon3},
// 		{ToolSocket4, PawnStateBuildingHUD::EquipIcon4},
// 		{ToolSocket5, PawnStateBuildingHUD::EquipIcon5},
// 		{ToolSocket6, PawnStateBuildingHUD::EquipIcon6},
// 		{ToolSocket7, PawnStateBuildingHUD::EquipIcon7},
// 		{ToolSocket8, PawnStateBuildingHUD::EquipIcon8},
// 	};
// 
// 	for (const auto& Iter : Ary)
// 	{
// 		auto UIPtr = Cast<UToolIcon>(GetWidgetFromName(Iter.Get<1>()));
// 		if (UIPtr)
// 		{
// 			TSharedPtr<FConsumableSocketInfo> SocketInfoSPtr = MakeShared<FConsumableSocketInfo>();
// 
// // 			SocketInfoSPtr->Key = Iter.Get<0>();
// // 			SocketInfoSPtr->SkillSocket = UIPtr->IconSocket;
// // 			SocketInfoSPtr->ProxyPtr = UIPtr->GetConsumablesProxy();
// // 
// // 			Result.Add(SocketInfoSPtr->SkillSocket, SocketInfoSPtr);
// 		}
// 	}
// 	return Result;
// }
