
#include "MarkPoints.h"

#include <Kismet/GameplayStatics.h>
#include <Blueprint/WidgetLayoutLibrary.h>
#include <Components/SizeBox.h>

#include "CharacterBase.h"
#include "LogWriter.h"
#include "TargetPoint_Runtime.h"
#include "Kismet/KismetMathLibrary.h"

void UMarkPoints::NativeConstruct()
{
	Super::NativeConstruct();

	SetAnchorsInViewport(FAnchors(.5f));
	SetAlignmentInViewport(FVector2D(.5f, .5f));
	
	bShouldCalcOutsideViewPosition = true;
}

void UMarkPoints::NativeDestruct()
{
	Super::NativeDestruct();
}

void UMarkPoints::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

FVector2D UMarkPoints::ModifyProjectedLocalPosition(
	const FGeometry& ViewportGeometry,
	const FVector2D& LocalPosition
)
{
	// TODO 这里还有BUG
	
	int32 ViewSizeX  = 0;
	int32 ViewSizeY  = 0;
	
	UGameplayStatics::GetPlayerController(this, 0)->GetViewportSize(ViewSizeX, ViewSizeY);

	FBox2D BoundingBox (FVector2D(ViewSizeX, ViewSizeY) * .1f, FVector2D(ViewSizeX, ViewSizeY) * .9f);

	if (BoundingBox.IsInside(LocalPosition))
	{
		return LocalPosition;
	}
	else
	{
		return BoundingBox.GetClosestPointTo(LocalPosition);
	}
}

FVector UMarkPoints::GetHoverPosition()
{
	return TargetPoint_RuntimePtr->GetActorLocation();
}
