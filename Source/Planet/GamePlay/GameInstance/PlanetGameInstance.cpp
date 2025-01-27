
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

	// 之前的代码，为什么加这句记不起来了
	// GetWorldImp()->SetGameInstance(this);

	bIsExiting = false;
}

void UPlanetGameInstance::Shutdown()
{
	bIsExiting = true;

	// 注意：这里提前释放的会，之后编辑器模式下保存资源会出错
	// GUObjectArray.ShutdownUObjectArray();

#if WITH_EDITOR
	auto TestMap1 = TestGCProxyMap;
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
