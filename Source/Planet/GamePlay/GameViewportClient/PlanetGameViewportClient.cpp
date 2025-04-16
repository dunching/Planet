#include "PlanetGameViewportClient.h"

#include "InputProcessorSubSystem.h"

void UPlanetGameViewportClient::Init(
	struct FWorldContext& WorldContext,
	UGameInstance* OwningGameInstance,
	bool bCreateNewAudioDevice
)
{
	Super::Init(WorldContext, OwningGameInstance, bCreateNewAudioDevice);

	InputProcessorSubSystemPtr = UInputProcessorSubSystem::GetInstance();
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
	FViewport* InViewport,
	FInputDeviceId InputDevice,
	FKey Key,
	float Delta,
	float DeltaTime,
	int32 NumSamples,
	bool bGamepad
)
{
	if (InputProcessorSubSystemPtr->InputAxis(InViewport, InputDevice, Key, Delta, DeltaTime, NumSamples, bGamepad))
	{
		return Super::InputAxis(InViewport, InputDevice, Key, Delta, DeltaTime, NumSamples, bGamepad);
	}
	return false;
}
