
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
}

void UPlanetGameInstance::OnStart()
{
	Super::OnStart();

	GetWorldImp()->SetGameInstance(this);

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
	Super::FinishDestroy();
	PRINTFUNC();
}

UPlanetGameInstance::~UPlanetGameInstance()
{
	PRINTFUNC();
}
