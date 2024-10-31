
#include "PlanetPlayerCameraManager.h"

static TAutoConsoleVariable<int32> APlanetPlayerCameraManager_Draw(
	TEXT("APlanetPlayerCameraManager.Draw"),
	0,
	TEXT("")
	TEXT(" default: 0"));

void APlanetPlayerCameraManager::UpdateCamera(float DeltaTime)
{
	Super::UpdateCamera(DeltaTime);

#ifdef WITH_EDITOR
	if (APlanetPlayerCameraManager_Draw.GetValueOnGameThread())
	{
		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			const FMinimalViewInfo& CurrentPOV = GetCameraCacheView();

			DrawDebugLine(GetWorld(), CurrentPOV.Location, CurrentPOV.Location + (CurrentPOV.Rotation.Vector() * 500), FColor::Yellow, false, 3);
		}
	}
#endif
}

