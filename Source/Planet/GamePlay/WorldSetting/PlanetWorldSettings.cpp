#include "PlanetWorldSettings.h"

#include "Subsystems/SubsystemBlueprintLibrary.h"

#include "AssetRefMap.h"
#include "RewardsTD.h"
#include "DataTableCollection.h"
#include "QuestSubSystem.h"
#include "QuestSystem_Imp.h"
#include "ModifyItemProxyStrategy.h"
#include "InputProcessorSubSystem_Imp.h"

void APlanetWorldSettings::PostInitializeComponents()
{
	Super::PostInitializeComponents();
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

UDataTableCollection* APlanetWorldSettings::GetSceneProxyExtendInfoMap() const
{
	return SceneProxyExtendInfoMapPtr.LoadSynchronous();
}

UQuestSubSystem* APlanetWorldSettings::GetGuideSubSystem() const
{
	return Cast<UQuestSubSystem_Imp>(
	                                 USubsystemBlueprintLibrary::GetWorldSubsystem(
		                                  GetWorldImp(),
		                                  UQuestSubSystem_Imp::StaticClass()
		                                 )
	                                );
}

const UPAD_ItemProxyCollection* APlanetWorldSettings::GetItemProxyCollection() const
{
	return PAD_ItemProxyCollectionRef.LoadSynchronous();
}

TSoftObjectPtr<UDataTable> APlanetWorldSettings::GetPropertyEntrysDT() const
{
	return GetSceneProxyExtendInfoMap()->DataTable_PropertyEntrys;
}
