// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <atomic>

#include "CoreMinimal.h"
#include "Engine/World.h"

#include "GenerateType.h"
#include <GameInstance/ShiYuGameInstance.h>
#include "ShiYu.h"

#include "UIManagerSubSystem.generated.h"

class UEquipMenu;
class UInteractionToAIMenu;
class UBackpackMenu;
class UCreateMenu;
class UPromptBox;
class AHumanCharacter;
class UPiginteraction;
class UDestroyProgress;


class UWorld;

UCLASS()
class SHIYU_API UUIManagerSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	template<typename FWidgetType>
	using FBeforeDisplayFunc = std::function<void(FWidgetType*)>;

	static UUIManagerSubSystem* GetInstance();

	UUIManagerSubSystem();

	virtual ~UUIManagerSubSystem();

	UPiginteraction* DisplayPigInteractionUI(
		const FBeforeDisplayFunc<UPiginteraction>& BeforeDisplayFunc,
		AHumanCharacter*TargetCharacterPtr
	);

	void RemovePigInteractionUI();

	UEquipMenu* DisplayEquipMenuUI(
		const FBeforeDisplayFunc<UEquipMenu>& BeforeDisplayFunc = nullptr
	);

	void RemoveEquipMenuUI();

	UBackpackMenu* DisplayBackpackLeftMenuUI(
		const FBeforeDisplayFunc<UBackpackMenu>& BeforeDisplayFunc = nullptr
	);

	void RemoveBackpackLeftMenuUI();

	UBackpackMenu* DisplayBackpackRightMenuUI(
		const FBeforeDisplayFunc<UBackpackMenu>& BeforeDisplayFunc = nullptr
	);

	void RemoveBackpackRightMenuUI();

	UCreateMenu* DisplayCreateQueueMenuUI(
		const FBeforeDisplayFunc<UCreateMenu>& BeforeDisplayFunc = nullptr
	);

	void RemoveCreateQueueMenuUI();

	UInteractionToAIMenu* DisplayInteractionToAIMenuUI();

	void RemoveDestroyProgress();

	UDestroyProgress* DisplayDestroyProgress();

	void RemoveInteractionToAIMenuUI();

	void DisplayPromtBox(const FString & Text);

	void DisplayPromtBox(const TArray<FString>& TextAry);

	void RemovePromtBox();

protected:

	UPROPERTY()
		UPiginteraction* PiginteractionPtr = nullptr;

	UPROPERTY()
		UEquipMenu* EquipMenuPtr = nullptr;

	UPROPERTY()
		UBackpackMenu* LeftBackpackMenuPtr = nullptr;

	UPROPERTY()
		UBackpackMenu* RightBackpackMenuPtr = nullptr;

	UPROPERTY()
		UCreateMenu* CreateQueueMenuPtr = nullptr;

	UPROPERTY()
		UInteractionToAIMenu* InteractionToAIMenuPtr = nullptr;

	UPROPERTY()
		UPromptBox* PromptBoxMenuPtr = nullptr;

	UPROPERTY()
		UDestroyProgress* DestroyProgressPtr = nullptr;

private:

	template<typename FWidgetType>
	void DisplayImp(
		FWidgetType*& WidgetPtr,
		UClass * BPClass, 
		const FBeforeDisplayFunc<FWidgetType>& BeforeDisplayFunc = nullptr
	);

	template<typename FWidgetType>
	void RemoveImp(FWidgetType*& WidgetPtr);

	std::atomic<int32> CheckPromtBoxCount = 0;

	void CheckPromtBox();

};

template<typename FWidgetType>
void UUIManagerSubSystem::DisplayImp(
	FWidgetType*& WidgetPtr,
	UClass* BPClass,
	const FBeforeDisplayFunc<FWidgetType>& BeforeDisplayFunc
)
{
	if (!WidgetPtr)
	{
		WidgetPtr = CreateWidget<FWidgetType>(GetWorldImp(), BPClass);
		if (WidgetPtr)
		{
			if (BeforeDisplayFunc)
			{
				BeforeDisplayFunc(WidgetPtr);
			}
			WidgetPtr->AddToViewport();
		}
	}
}
 
template<typename FWidgetType>
void UUIManagerSubSystem::RemoveImp(FWidgetType*& WidgetPtr)
{
	if (WidgetPtr)
	{
		WidgetPtr->RemoveFromParent();
		WidgetPtr = nullptr;
	}
}
