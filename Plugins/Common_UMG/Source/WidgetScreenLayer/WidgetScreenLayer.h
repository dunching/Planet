// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Kismet/GameplayStatics.h"
#include "Slate/SGameLayerManager.h"
#include "Slate/SWorldWidgetScreenLayer.h"


#include "WidgetScreenLayer.generated.h"

class UHoverWidgetBase;

class COMMON_UMG_API FComponentEntry_Hover
{
public:
	FComponentEntry_Hover();

	~FComponentEntry_Hover();

public:
	bool bRemoving = false;

	UHoverWidgetBase* HoverWidgetPtr;

	TSharedPtr<SWidget> ContainerWidget;

	TSharedPtr<SWidget> Widget;

	SConstraintCanvas::FSlot* Slot;
};

class COMMON_UMG_API SHoverWidgetScreenLayer : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SHoverWidgetScreenLayer)
		{
			_Visibility = EVisibility::SelfHitTestInvisible;
		}

	SLATE_END_ARGS()

public:
	void Construct(
		const FArguments& InArgs,
		const FLocalPlayerContext& InPlayerContext
		);

	virtual void Tick(
		const FGeometry& AllottedGeometry,
		const double InCurrentTime,
		const float InDeltaTime
		) override;

	virtual FVector2D ComputeDesiredSize(
		float
		) const override;

	void SetWidgetDrawSize(
		FVector2D DrawSize
		);

	void SetWidgetPivot(
		FVector2D Pivot
		);

	void AddComponent(
		UHoverWidgetBase* HoverWidgetPtr,
		TSharedRef<SWidget> Widget
		);

	void RemoveComponent(
		UHoverWidgetBase* HoverWidgetPtr
		);

	TWeakPtr<TMap<UHoverWidgetBase*, FComponentEntry_Hover>> HoverWidgetMapSPtr;

private:
	void RemoveEntryFromCanvas(
		FComponentEntry_Hover& Entry
		);

	FLocalPlayerContext PlayerContext;

	FVector2D DrawSize = FVector2D::ZeroVector;

	FVector2D Pivot = FVector2D(.5f, .5f);

	TSharedPtr<SConstraintCanvas> Canvas;
};

class COMMON_UMG_API FHoverWidgetScreenLayer : public IGameLayer
{
public:
	FHoverWidgetScreenLayer(
		const FLocalPlayerContext& PlayerContext
		);

	virtual ~FHoverWidgetScreenLayer();

	void AddHoverWidget(
		UHoverWidgetBase* HoverWidgetPtr
		);

	void RemoveHoverWidget(
		UHoverWidgetBase* HoverWidgetPtr
		);

	virtual TSharedRef<SWidget> AsWidget() override;

private:
	FLocalPlayerContext OwningPlayer;

	TWeakPtr<SHoverWidgetScreenLayer> ScreenLayerPtr;

	TSharedPtr<TMap<UHoverWidgetBase*, FComponentEntry_Hover>> HoverWidgetMapSPtr;
};

const FName TargetPointSharedLayerName = TEXT("TargetPointSharedLayer");

UCLASS()
class COMMON_UMG_API UKismetGameLayerManagerLibrary
	: public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	template <typename GameLayerType>
	static TSharedPtr<GameLayerType> GetGameLayer(
		UWorld* ThisWorld,
		const FName& LayerName
		);
};

template <typename GameLayerType>
TSharedPtr<GameLayerType> UKismetGameLayerManagerLibrary::GetGameLayer(
	UWorld* ThisWorld,
	const FName& LayerName
	)
{
	TSharedPtr<GameLayerType> GameLayerSPtr = nullptr;

	if (UGameViewportClient* ViewportClient = ThisWorld->GetGameViewport())
	{
		TSharedPtr<IGameLayerManager> LayerManager = ViewportClient->GetGameLayerManager();
		if (LayerManager.IsValid())
		{
			auto LocalPlayers = UGameplayStatics::GetGameInstance(ThisWorld)->GetLocalPlayers();
			if (LocalPlayers.IsValidIndex(0))
			{
				ULocalPlayer* TargetPlayer = LocalPlayers[0];

				TSharedPtr<IGameLayer> Layer = LayerManager->FindLayerForPlayer(
				                                                                TargetPlayer,
				                                                                TargetPointSharedLayerName
				                                                               );
				if (Layer.IsValid())
				{
					GameLayerSPtr = StaticCastSharedPtr<FHoverWidgetScreenLayer>(Layer);
				}
			}
		}
	}

	return GameLayerSPtr;
}
