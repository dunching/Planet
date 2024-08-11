
#include "CameraTrailHelper.h"

#include "Components/SplineComponent.h"
#include <GameFramework/PlayerController.h>

ACameraTrailHelper::ACameraTrailHelper(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	SplineComponentPtr = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	RootComponent = SplineComponentPtr;
}

void ACameraTrailHelper::BecomeViewTarget(APlayerController* PC)
{
	Super::BecomeViewTarget(PC);


}

void ACameraTrailHelper::CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult)
{
	Super::CalcCamera(DeltaTime, OutResult);


}

void ACameraTrailHelper::EndViewTarget(APlayerController* PC)
{
	Super::EndViewTarget(PC);
}
