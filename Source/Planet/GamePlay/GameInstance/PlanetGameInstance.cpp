
#include "PlanetGameInstance.h"

#include "GameFramework/PlayerInput.h"

#include "LogHelper/LogWriter.h"

#include "ArticleBase.h"
#include "ArticleSharedData.h"
#include "AssetRefMap.h"
#include "Planet.h"
#include "InputProcessorSubSystem.h"

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

	GUObjectArray.ShutdownUObjectArray();

#if WITH_EDITOR
	auto TestMap1 = TestGCUnitMap;
//	check(TestMap1.IsEmpty());
#endif

	UInputProcessorSubSystem::GetInstance()->ResetProcessor();

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
