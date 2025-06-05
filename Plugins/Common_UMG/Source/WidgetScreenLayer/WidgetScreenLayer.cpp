#include "WidgetScreenLayer.h"

#include "Blueprint/SlateBlueprintLibrary.h"

#include "HoverWidgetBase.h"

static int32 GSlateTargetPointWidgetZOrder = 2;

FComponentEntry_Hover::FComponentEntry_Hover() :
                                               Slot(nullptr)
{
}

FComponentEntry_Hover::~FComponentEntry_Hover()
{
	Widget.Reset();
	ContainerWidget.Reset();
}

void SHoverWidgetScreenLayer::RemoveEntryFromCanvas(
	FComponentEntry_Hover& Entry
	)
{
	// Mark the component was being removed, so we ignore any other remove requests for this component.
	Entry.bRemoving = true;

	if (TSharedPtr<SWidget> ContainerWidget = Entry.ContainerWidget)
	{
		Canvas->RemoveSlot(ContainerWidget.ToSharedRef());
	}
}

void SHoverWidgetScreenLayer::Construct(
	const FArguments& InArgs,
	const FLocalPlayerContext& InPlayerContext
	)
{
	PlayerContext = InPlayerContext;

	bCanSupportFocus = false;
	DrawSize = FVector2D(0, 0);
	Pivot = FVector2D(0.5f, 0.5f);

	ChildSlot
	[
		SAssignNew(Canvas, SConstraintCanvas)
	];
}

void SHoverWidgetScreenLayer::SetWidgetDrawSize(
	FVector2D InDrawSize
	)
{
	DrawSize = InDrawSize;
}

void SHoverWidgetScreenLayer::SetWidgetPivot(
	FVector2D InPivot
	)
{
	Pivot = InPivot;
}

void SHoverWidgetScreenLayer::AddComponent(
	UHoverWidgetBase* HoverWidgetPtr,
	TSharedRef<SWidget> Widget
	)
{
	if (HoverWidgetPtr)
	{
		auto& Entry = HoverWidgetMapSPtr.Pin()->FindOrAdd(HoverWidgetPtr);
		Entry.HoverWidgetPtr = HoverWidgetPtr;
		Entry.Widget = Widget;

		Canvas->AddSlot()
		      .Expose(Entry.Slot)
		[
			SAssignNew(Entry.ContainerWidget, SBox)
			[
				Widget
			]
		];
	}
}

void SHoverWidgetScreenLayer::RemoveComponent(
	UHoverWidgetBase* HoverWidgetPtr
	)
{
	if (ensure(HoverWidgetPtr))
	{
		if (auto EntryPtr = HoverWidgetMapSPtr.Pin()->Find(HoverWidgetPtr))
		{
			if (!EntryPtr->bRemoving)
			{
				RemoveEntryFromCanvas(*EntryPtr);
				HoverWidgetMapSPtr.Pin()->Remove(HoverWidgetPtr);
			}
		}
	}
}

void SHoverWidgetScreenLayer::Tick(
	const FGeometry& AllottedGeometry,
	const double InCurrentTime,
	const float InDeltaTime
	)
{
	QUICK_SCOPE_CYCLE_COUNTER(SWorldWidgetScreenLayer_Tick);

	if (APlayerController* PlayerController = PlayerContext.GetPlayerController())
	{
		if (UGameViewportClient* ViewportClient = PlayerController->GetWorld()->GetGameViewport())
		{
			const FGeometry& ViewportGeometry = ViewportClient->GetGameLayerManager()->GetViewportWidgetHostGeometry();

			// cache projection data here and avoid calls to UWidgetLayoutLibrary.ProjectWorldLocationToWidgetPositionWithDistance
			FSceneViewProjectionData ProjectionData;
			FMatrix ViewProjectionMatrix;
			bool bHasProjectionData = false;

			ULocalPlayer const* const LP = PlayerController->GetLocalPlayer();
			if (LP && LP->ViewportClient)
			{
				bHasProjectionData = LP->GetProjectionData(ViewportClient->Viewport, /*out*/ ProjectionData);
				if (bHasProjectionData)
				{
					ViewProjectionMatrix = ProjectionData.ComputeViewProjectionMatrix();
				}
			}

			if (!HoverWidgetMapSPtr.IsValid())
			{
				return;
			}

			for (auto Iter = HoverWidgetMapSPtr.Pin()->CreateIterator(); Iter; ++Iter)
			{
				if (auto HoverWidgetPtr = Iter->Value.HoverWidgetPtr)
				{
					FVector WorldLocation = HoverWidgetPtr->GetHoverPosition();

					FVector2D ScreenPosition2D = FVector2D::ZeroVector;

					if (GSlateTargetPointWidgetZOrder &&
					    Iter->Value.HoverWidgetPtr &&
					    (
						    (Iter->Value.HoverWidgetPtr->GetVisibility() == ESlateVisibility::Collapsed) ||
						    (Iter->Value.HoverWidgetPtr->GetVisibility() == ESlateVisibility::Hidden)
					    ))
					{
						return;
					}

					const auto bIsInsideView = FSceneView::ProjectWorldToScreen(
					                                                            WorldLocation,
					                                                            ProjectionData.GetConstrainedViewRect(),
					                                                            ViewProjectionMatrix,
					                                                            ScreenPosition2D,
					                                                            Iter->Value.HoverWidgetPtr->
					                                                            bShouldCalcOutsideViewPosition
					                                                           );
					if (bIsInsideView || Iter->Value.HoverWidgetPtr->bShouldCalcOutsideViewPosition)
					{
						const double ViewportDist = FVector::Dist(ProjectionData.ViewOrigin, WorldLocation);
						const FVector2D RoundedPosition2D(
						                                  FMath::RoundToDouble(ScreenPosition2D.X),
						                                  FMath::RoundToDouble(ScreenPosition2D.Y)
						                                 );

						// If the root widget has pixel snapping disabled, then don't pixel snap the screen coordinates either otherwise
						// it'll always jump between pixels. This saves needing an explicit flag on the widget component, and is probably 
						// a better delegation of responsibility anyway, since changing the widget type can change the snapping as it wants
						bool bDisablePixelSnapping = Iter->Value.Widget->GetPixelSnapping() ==
						                             EWidgetPixelSnapping::Disabled;
						const FVector2D ScreenPositionToUse = bDisablePixelSnapping ?
							                                      ScreenPosition2D :
							                                      RoundedPosition2D;

						FVector2D ViewportPosition2D;
						USlateBlueprintLibrary::ScreenToViewport(
						                                         PlayerController,
						                                         ScreenPositionToUse,
						                                         OUT ViewportPosition2D
						                                        );

						const FVector ViewportPosition(ViewportPosition2D.X, ViewportPosition2D.Y, ViewportDist);

						Iter->Value.ContainerWidget->SetVisibility(EVisibility::SelfHitTestInvisible);

						if (SConstraintCanvas::FSlot* CanvasSlot = Iter->Value.Slot)
						{
							FVector2D AbsoluteProjectedLocation = ViewportGeometry.LocalToAbsolute(
								 FVector2D(ViewportPosition.X, ViewportPosition.Y)
								);
							FVector2D LocalPosition = AllottedGeometry.AbsoluteToLocal(AbsoluteProjectedLocation);

							if (Iter->Value.HoverWidgetPtr)
							{
								LocalPosition = Iter->Value.HoverWidgetPtr->ModifyProjectedLocalPosition(
									 ViewportGeometry,
									 LocalPosition
									);

								FVector2D ComponentDrawSize = Iter->Value.HoverWidgetPtr->DrawSize;
								FVector2D ComponentPivot = Iter->Value.HoverWidgetPtr->Pivot;

								CanvasSlot->SetAutoSize(
								                        Iter->Value.HoverWidgetPtr->bDrawAtDesiredSize
								                       );
								CanvasSlot->SetOffset(
								                      FMargin(
								                              LocalPosition.X,
								                              LocalPosition.Y,
								                              ComponentDrawSize.X,
								                              ComponentDrawSize.Y
								                             )
								                     );
								CanvasSlot->SetAnchors(FAnchors(0, 0, 0, 0));
								CanvasSlot->SetAlignment(ComponentPivot);

								if (GSlateTargetPointWidgetZOrder != 0)
								{
									CanvasSlot->SetZOrder(static_cast<float>(-ViewportPosition.Z));
								}
							}
							else
							{
								CanvasSlot->SetAutoSize(DrawSize.IsZero());
								CanvasSlot->SetOffset(
								                      FMargin(LocalPosition.X, LocalPosition.Y, DrawSize.X, DrawSize.Y)
								                     );
								CanvasSlot->SetAnchors(FAnchors(0, 0, 0, 0));
								CanvasSlot->SetAlignment(Pivot);

								if (GSlateTargetPointWidgetZOrder != 0)
								{
									CanvasSlot->SetZOrder(static_cast<float>(-ViewportPosition.Z));
								}
							}
						}
					}
					else
					{
						Iter->Value.ContainerWidget->SetVisibility(EVisibility::Collapsed);
					}
				}
				else
				{
					RemoveEntryFromCanvas(Iter->Value);
					Iter.RemoveCurrent();
					continue;
				}
			}

			// Done
			return;
		}
	}

	if (GSlateIsOnFastUpdatePath)
	{
		// Hide everything if we are unable to do any of the work.
		for (const auto& Iter : *HoverWidgetMapSPtr.Pin())
		{
			Iter.Value.ContainerWidget->SetVisibility(EVisibility::Collapsed);
		}
	}
}

FVector2D SHoverWidgetScreenLayer::ComputeDesiredSize(
	float X
	) const
{
	return FVector2D(0, 0);
}

FHoverWidgetScreenLayer::FHoverWidgetScreenLayer(
	const FLocalPlayerContext& PlayerContext
	)
{
	OwningPlayer = PlayerContext;
}

FHoverWidgetScreenLayer::~FHoverWidgetScreenLayer()
{
	// empty virtual destructor to help clang warning
}

void FHoverWidgetScreenLayer::AddHoverWidget(
	UHoverWidgetBase* HoverWidgetPtr
	)
{
	if (!HoverWidgetMapSPtr)
	{
		HoverWidgetMapSPtr = MakeShared<decltype(HoverWidgetMapSPtr)::ElementType>();
		ScreenLayerPtr.Pin()->HoverWidgetMapSPtr = HoverWidgetMapSPtr;
	}

	if (HoverWidgetMapSPtr)
	{
		if (TSharedPtr<SHoverWidgetScreenLayer> ScreenLayer = ScreenLayerPtr.Pin())
		{
			ScreenLayer->AddComponent(HoverWidgetPtr, HoverWidgetPtr->TakeWidget());
		}
	}
}

void FHoverWidgetScreenLayer::RemoveHoverWidget(
	UHoverWidgetBase* HoverWidgetPtr
	)
{
	if (HoverWidgetPtr)
	{
		if (TSharedPtr<SHoverWidgetScreenLayer> ScreenLayer = ScreenLayerPtr.Pin())
		{
			ScreenLayer->RemoveComponent(HoverWidgetPtr);
		}
	}
}

TSharedRef<SWidget> FHoverWidgetScreenLayer::AsWidget()
{
	if (TSharedPtr<SHoverWidgetScreenLayer> ScreenLayer = ScreenLayerPtr.Pin())
	{
		return ScreenLayer.ToSharedRef();
	}

	TSharedRef<SHoverWidgetScreenLayer> NewScreenLayer = SNew(SHoverWidgetScreenLayer, OwningPlayer);
	ScreenLayerPtr = NewScreenLayer;
	ScreenLayerPtr.Pin()->HoverWidgetMapSPtr = HoverWidgetMapSPtr;

	return NewScreenLayer;
}
