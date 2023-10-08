
#include "UIManagerSubSystem.h"

#include "Async/Async.h"
#include "Subsystems/SubsystemBlueprintLibrary.h"

#include "GameMode/ShiYuGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Menu/InteractionToAIMenu.h"
#include "Menu/CreateMenu/CreateMenu.h"
#include "Menu/PromptBox.h"
#include "Menu/Backpack/BackpackMenu.h"
#include "Menu/EquipItems/EquipMenu.h"
#include "CharacterInteraction/Piginteraction.h"
#include <AssetRefrencePath.h>
#include "AssetRefMap.h"
#include "Item/Hover/DestroyProgress/DestroyProgress.h"

UUIManagerSubSystem* UUIManagerSubSystem::GetInstance()
{
	return Cast<UUIManagerSubSystem>(USubsystemBlueprintLibrary::GetGameInstanceSubsystem(GetWorldImp(), UUIManagerSubSystem::StaticClass()));
}

UUIManagerSubSystem::UUIManagerSubSystem():
	Super()
{

}

UUIManagerSubSystem::~UUIManagerSubSystem()
{
	PiginteractionPtr = nullptr;
	EquipMenuPtr = nullptr;
	LeftBackpackMenuPtr = nullptr;
	RightBackpackMenuPtr = nullptr;
	CreateQueueMenuPtr = nullptr;
	InteractionToAIMenuPtr = nullptr;
	PromptBoxMenuPtr = nullptr;
}

UPiginteraction* UUIManagerSubSystem::DisplayPigInteractionUI(
	const FBeforeDisplayFunc<UPiginteraction>& BeforeDisplayFunc,
	AHumanCharacter* TargetCharacterPtr
)
{
	DisplayImp<UPiginteraction>(
		PiginteractionPtr,
		UAssetRefMap::GetInstance()->PiginteractionClass,
		BeforeDisplayFunc
		);

	return PiginteractionPtr;
}

void UUIManagerSubSystem::RemovePigInteractionUI()
{
	RemoveImp(PiginteractionPtr);
}

UEquipMenu* UUIManagerSubSystem::DisplayEquipMenuUI(
	const FBeforeDisplayFunc<UEquipMenu>& BeforeDisplayFunc
)
{
	DisplayImp<UEquipMenu>(
		EquipMenuPtr,
		UAssetRefMap::GetInstance()->EquipMenuClass,
		BeforeDisplayFunc
		);

	return EquipMenuPtr;
}

void UUIManagerSubSystem::RemoveEquipMenuUI()
{
	RemoveImp(EquipMenuPtr);
}

UBackpackMenu* UUIManagerSubSystem::DisplayBackpackLeftMenuUI(
	const FBeforeDisplayFunc<UBackpackMenu>& BeforeDisplayFunc 
)
{
	DisplayImp<UBackpackMenu>(
		LeftBackpackMenuPtr,
		UAssetRefMap::GetInstance()->BackpackMenuClass,
		BeforeDisplayFunc
		);

	if (LeftBackpackMenuPtr)
	{
		LeftBackpackMenuPtr->SetPutPostion(UBackpackMenu::EPutPostion::kLeft);
	}

	return LeftBackpackMenuPtr;
}

void UUIManagerSubSystem::RemoveBackpackLeftMenuUI()
{
	RemoveImp(LeftBackpackMenuPtr);
}

UBackpackMenu* UUIManagerSubSystem::DisplayBackpackRightMenuUI(
	const FBeforeDisplayFunc<UBackpackMenu>& BeforeDisplayFunc
)
{
	DisplayImp<UBackpackMenu>(
		RightBackpackMenuPtr,
		UAssetRefMap::GetInstance()->BackpackMenuClass,
		BeforeDisplayFunc
		);

	if (RightBackpackMenuPtr)
	{
		RightBackpackMenuPtr->SetPutPostion(UBackpackMenu::EPutPostion::kRight);
	}

	return RightBackpackMenuPtr;
}

void UUIManagerSubSystem::RemoveBackpackRightMenuUI()
{
	RemoveImp(RightBackpackMenuPtr);
}

UCreateMenu* UUIManagerSubSystem::DisplayCreateQueueMenuUI(
	const FBeforeDisplayFunc<UCreateMenu>& BeforeDisplayFunc
)
{
	DisplayImp<UCreateMenu>(
		CreateQueueMenuPtr,
		UAssetRefMap::GetInstance()->CreateMenuClass,
		BeforeDisplayFunc
		);

	return CreateQueueMenuPtr;
}

void UUIManagerSubSystem::RemoveCreateQueueMenuUI()
{
	RemoveImp(CreateQueueMenuPtr);
}

UInteractionToAIMenu* UUIManagerSubSystem::DisplayInteractionToAIMenuUI()
{
	DisplayImp<UInteractionToAIMenu>(
		InteractionToAIMenuPtr,
		UAssetRefMap::GetInstance()->InteractionToAIMenuClass
		);

	return InteractionToAIMenuPtr;
}

void UUIManagerSubSystem::RemoveDestroyProgress()
{
	RemoveImp(DestroyProgressPtr);
}

UDestroyProgress* UUIManagerSubSystem::DisplayDestroyProgress()
{
	DisplayImp<UDestroyProgress>(
		DestroyProgressPtr,
		UAssetRefMap::GetInstance()->DestroyProgressClass
		);

	return DestroyProgressPtr;
}

void UUIManagerSubSystem::RemoveInteractionToAIMenuUI()
{
	RemoveImp(InteractionToAIMenuPtr);
}

void UUIManagerSubSystem::DisplayPromtBox(const FString& Text)
{
	DisplayPromtBox(TArray<FString>{ Text });
}

void UUIManagerSubSystem::DisplayPromtBox(const TArray<FString>& TextAry)
{
	DisplayImp<UPromptBox>(
		PromptBoxMenuPtr,
		UAssetRefMap::GetInstance()->PromptClass
		);

	if (PromptBoxMenuPtr)
	{
		PromptBoxMenuPtr->SetPromptStr(TextAry);
	}
}

void UUIManagerSubSystem::RemovePromtBox()
{
	RemoveImp(PromptBoxMenuPtr);
}

void UUIManagerSubSystem::CheckPromtBox()
{
	for (; CheckPromtBoxCount > 0; CheckPromtBoxCount--)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

	}

	AsyncTask(ENamedThreads::GameThread, [this]() {
		PromptBoxMenuPtr->RemoveFromParent();
		PromptBoxMenuPtr = nullptr; 
	});
}
