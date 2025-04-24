
#include "HoverWidgetBase.h"

inline FVector2D UHoverWidgetBase::ModifyProjectedLocalPosition(
	const FGeometry& ViewportGeometry,
	const FVector2D& LocalPosition
)
{
	return LocalPosition;
}

FVector UHoverWidgetBase::GetHoverPosition()
{
	return FVector::ZeroVector;
}
