
#include "ShiYuGameInstance.h"

#include "GameFramework/PlayerInput.h"

#include "LogHelper/LogWriter.h"

#include "CacheAssetManager.h"
#include "ArticleBase.h"
#include "ArticleSharedData.h"
#include "AssetRefMap.h"
#include "ShiYu.h"

void UShiYuGameInstance::Init()
{
	Super::Init();

	GGameInstancePtr = this;
}

void UShiYuGameInstance::OnStart()
{
	Super::OnStart();
}

void UShiYuGameInstance::Shutdown()
{
	Super::Shutdown();
	PRINTFUNC();
}

void UShiYuGameInstance::FinishDestroy()
{
	GGameInstancePtr = nullptr;
	AssetRefMapPtr = nullptr;

	Super::FinishDestroy();
	PRINTFUNC();
}

UShiYuGameInstance::~UShiYuGameInstance()
{
	PRINTFUNC();
}

UAssetRefMap* UShiYuGameInstance::GetAssetRefMapInstance()
{
	if (!AssetRefMapPtr)
	{
		AssetRefMapPtr = NewObject<UAssetRefMap>(GetWorld(), AssetRefMapClass);
	}

	return AssetRefMapPtr;
}
