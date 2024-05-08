
#include "PlanetGameInstance.h"

#include "GameFramework/PlayerInput.h"

#include "LogHelper/LogWriter.h"

#include "CacheAssetManager.h"
#include "ArticleBase.h"
#include "ArticleSharedData.h"
#include "AssetRefMap.h"
#include "Planet.h"

void UPlanetGameInstance::Init()
{
	Super::Init();

	GGameInstancePtr = this;
}

void UPlanetGameInstance::OnStart()
{
	Super::OnStart();

	bIsExiting = false;
}

void UPlanetGameInstance::Shutdown()
{
	bIsExiting = true;

	Super::Shutdown();
	PRINTFUNC();
}

void UPlanetGameInstance::FinishDestroy()
{
	GGameInstancePtr = nullptr;
	AssetRefMapPtr = nullptr;

	Super::FinishDestroy();
	PRINTFUNC();
}

UPlanetGameInstance::~UPlanetGameInstance()
{
	PRINTFUNC();
}

UAssetRefMap* UPlanetGameInstance::GetAssetRefMapInstance()
{
	if (!AssetRefMapPtr)
	{
		AssetRefMapPtr = NewObject<UAssetRefMap>(GetWorld(), AssetRefMapClass);
	}

	return AssetRefMapPtr;
}
