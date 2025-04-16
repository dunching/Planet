#include "PlanetLocalPlayer.h"

#include "GameMode_Main.h"
#include "PlanetPlayerController.h"
#include "Kismet/GameplayStatics.h"

void UPlanetLocalPlayer::ReceivedPlayerController(
	APlayerController* NewController
)
{
	Super::ReceivedPlayerController(NewController);

	// 仅 游戏中 的PC才会生成
	if (NewController->IsA(APlanetPlayerController::StaticClass()))
	{
	}
}

bool UPlanetLocalPlayer::SpawnPlayActor(
	const FString& URL,
	FString& OutError,
	UWorld* InWorld
)
{
	return Super::SpawnPlayActor(URL, OutError, InWorld);
}

void UPlanetLocalPlayer::PlayerAdded(
	class UGameViewportClient* InViewportClient,
	FPlatformUserId InUserId
)
{
	Super::PlayerAdded(InViewportClient, InUserId);
}
