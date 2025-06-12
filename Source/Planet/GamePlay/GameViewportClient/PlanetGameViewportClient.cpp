#include "PlanetGameViewportClient.h"

#include "CharacterAbilitySystemComponent.h"

#include "WidgetScreenLayer.h"

#include "InputProcessorSubSystemBase.h"
#include "InputProcessorSubSystem_Imp.h"

void UPlanetGameViewportClient::Init(
	struct FWorldContext& WorldContext,
	UGameInstance* OwningGameInstance,
	bool bCreateNewAudioDevice
	)
{
	Super::Init(WorldContext, OwningGameInstance, bCreateNewAudioDevice);

	FTSTicker::GetCoreTicker().AddTicker(
	                                     FTickerDelegate::CreateLambda(
	                                                                   [this, OwningGameInstance](
	                                                                   float
	                                                                   )
	                                                                   {
		                                                                   TSharedPtr<IGameLayerManager> LayerManager =
			                                                                   GetGameLayerManager();
		                                                                   if (LayerManager.IsValid())
		                                                                   {
			                                                                   auto PlayerAry = OwningGameInstance->
				                                                                   GetLocalPlayers();
			                                                                   if (PlayerAry.IsValidIndex(0))
			                                                                   {
				                                                                   auto NewScreenLayer = MakeShareable(
					                                                                    new FHoverWidgetScreenLayer(
						                                                                     PlayerAry[0]
						                                                                    )
					                                                                   );
				                                                                   LayerManager->AddLayerForPlayer(
					                                                                    PlayerAry[0],
					                                                                    TargetPointSharedLayerName,
					                                                                    NewScreenLayer,
					                                                                    -100
					                                                                   );
				                                                                   return false;
			                                                                   }
		                                                                   }
		                                                                   return true;
	                                                                   }
	                                                                  ),
	                                     1.f
	                                    );
}

void UPlanetGameViewportClient::NotifyPlayerAdded(
	int32 PlayerIndex,
	class ULocalPlayer* AddedPlayer
	)
{
	Super::NotifyPlayerAdded(PlayerIndex, AddedPlayer);

	InputProcessorSubSystemPtr = UInputProcessorSubSystem_Imp::GetInstance();
}

ULocalPlayer* UPlanetGameViewportClient::SetupInitialLocalPlayer(
	FString& OutError
	)
{
	auto Result = Super::SetupInitialLocalPlayer(OutError);

	return Result;
}

inline bool UPlanetGameViewportClient::InputKey(
	const FInputKeyEventArgs& EventArgs
	)
{
	if (InputProcessorSubSystemPtr->InputKey(EventArgs))
	{
		return Super::InputKey(EventArgs);
	}
	return false;
}

inline bool UPlanetGameViewportClient::InputAxis(
	const FInputKeyEventArgs& EventArgs
	)
{
	if (InputProcessorSubSystemPtr->InputAxis(EventArgs))
	{
		return Super::InputAxis(EventArgs);
	}
	return false;
}
