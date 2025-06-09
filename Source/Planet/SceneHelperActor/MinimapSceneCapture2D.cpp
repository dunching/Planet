#include "MinimapSceneCapture2D.h"

#include "Components/SceneCaptureComponent2D.h"
#include "Kismet/KismetMathLibrary.h"

AMinimapSceneCapture2D::AMinimapSceneCapture2D(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(ObjectInitializer)
{
	GetCaptureComponent2D()->bCaptureEveryFrame = false;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = .1f;
}

inline void AMinimapSceneCapture2D::BeginPlay()
{
	Super::BeginPlay();

	GetRootComponent()->SetUsingAbsoluteRotation(true);

	GetCaptureComponent2D()->SetRelativeLocation(FVector(0, 0, Depth));
}

void AMinimapSceneCapture2D::Tick(
	float DeltaTime
	)
{
	Super::Tick(DeltaTime);

	auto PCPtr = GEngine->GetFirstLocalPlayerController(GetWorld());
	if (!PCPtr)
	{
		return;
	}
	auto PlayerCharacterPtr = PCPtr->GetPawn();
	if (!PlayerCharacterPtr)
	{
		return;
	}
	
	const auto ActorForwardVector = PlayerCharacterPtr->GetActorForwardVector();
	const auto PlayerDir = UKismetMathLibrary::MakeRotFromZX(FVector::UpVector, ActorForwardVector);
	
	// 计算点积
	float DotProduct = FVector::DotProduct(PlayerDir.Vector(), FVector::ForwardVector);

	// 计算
	const auto CrossProduct = FVector::CrossProduct(PlayerDir.Vector(), FVector::ForwardVector);

	// 计算角度（弧度）
	float AngleRadians = FMath::Acos(DotProduct);

	// 将弧度转换为角度
	float AngleDegrees = FMath::RadiansToDegrees(CrossProduct.Z > 0 ?  -AngleRadians : AngleRadians);
	
	std::ignore = OnPlayerAngleChangedDelegate.ExecuteIfBound(AngleDegrees);
}
