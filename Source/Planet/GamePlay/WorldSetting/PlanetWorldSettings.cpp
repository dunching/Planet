#include "PlanetWorldSettings.h"

#include "Subsystems/SubsystemBlueprintLibrary.h"

#include "AssetRefMap.h"
#include "RewardsTD.h"
#include "SceneProxyExtendInfo.h"
#include "GuideSubSystem.h"
#include "GuideSubSystem_Imp.h"
#include "ModifyItemProxyStrategy.h"

void APlanetWorldSettings::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	InitialModifyItemProxyStrategies();
}

void APlanetWorldSettings::BeginPlay()
{
	Super::BeginPlay();
}

UPAD_RewardsItems* APlanetWorldSettings::GetTableRow_RewardsTD() const
{
	return TableRow_RewardsTDRef.LoadSynchronous();
}

UAssetRefMap* APlanetWorldSettings::GetAssetRefMapInstance() const
{
	return AssetRefMapRef.LoadSynchronous();
}

USceneProxyExtendInfoMap* APlanetWorldSettings::GetSceneProxyExtendInfoMap() const
{
	return SceneProxyExtendInfoMapPtr.LoadSynchronous();
}

UGuideSubSystem* APlanetWorldSettings::GetGuideSubSystem() const
{
	return Cast<UGuideSubSystem_Imp>(
								 USubsystemBlueprintLibrary::GetWorldSubsystem(
																			   GetWorldImp(),
																			   UGuideSubSystem_Imp::StaticClass()
																			  )
								);
}

const UPAD_ItemProxyCollection* APlanetWorldSettings::GetItemProxyCollection() const
{
	return PAD_ItemProxyCollectionRef.LoadSynchronous();
}

void APlanetWorldSettings::InitialModifyItemProxyStrategies()
{
	//
	ModifyItemProxyStrategiesMap.Empty();
	{
		auto ModifyItemProxyStrategySPtr = MakeShared<FModifyItemProxyStrategy_Character>();
		ModifyItemProxyStrategiesMap.Add(ModifyItemProxyStrategySPtr->GetCanOperationType(), ModifyItemProxyStrategySPtr);
	}
	{
		auto ModifyItemProxyStrategySPtr = MakeShared<FModifyItemProxyStrategy_Weapon>();
		ModifyItemProxyStrategiesMap.Add(ModifyItemProxyStrategySPtr->GetCanOperationType(), ModifyItemProxyStrategySPtr);
	}
	{
		auto ModifyItemProxyStrategySPtr = MakeShared<FModifyItemProxyStrategy_WeaponSkill>();
		ModifyItemProxyStrategiesMap.Add(ModifyItemProxyStrategySPtr->GetCanOperationType(), ModifyItemProxyStrategySPtr);
	}
	{
		auto ModifyItemProxyStrategySPtr = MakeShared<FModifyItemProxyStrategy_ActiveSkill>();
		ModifyItemProxyStrategiesMap.Add(ModifyItemProxyStrategySPtr->GetCanOperationType(), ModifyItemProxyStrategySPtr);
	}
	{
		auto ModifyItemProxyStrategySPtr = MakeShared<FModifyItemProxyStrategy_PassveSkill>();
		ModifyItemProxyStrategiesMap.Add(ModifyItemProxyStrategySPtr->GetCanOperationType(), ModifyItemProxyStrategySPtr);
	}
	{
		auto ModifyItemProxyStrategySPtr = MakeShared<FModifyItemProxyStrategy_Coin>();
		ModifyItemProxyStrategiesMap.Add(ModifyItemProxyStrategySPtr->GetCanOperationType(), ModifyItemProxyStrategySPtr);
	}
	{
		auto ModifyItemProxyStrategySPtr = MakeShared<FModifyItemProxyStrategy_Consumable>();
		ModifyItemProxyStrategiesMap.Add(ModifyItemProxyStrategySPtr->GetCanOperationType(), ModifyItemProxyStrategySPtr);
	}
}
