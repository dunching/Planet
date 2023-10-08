
#include "DestroyProgress.h"

#include <Components/Image.h>
#include <Blueprint/WidgetLayoutLibrary.h>
#include <Kismet/GameplayStatics.h>
#include <Components/CanvasPanel.h>
#include "Components/CanvasPanelSlot.h"
#include "Materials/MaterialInstanceDynamic.h"

#include <SceneObj/Building/BuildingBase.h>
#include <AssetRefMap.h>

void UDestroyProgress::SetLookAtObject(ABuildingBase* BuildingPtr)
{
	BuildingItemPtr = BuildingPtr;
}

void UDestroyProgress::SetProgress(uint8 Progress)
{
	auto ImagePtr = Cast<UImage>(GetWidgetFromName(TEXT("Image")));
	if (ImagePtr)
	{
		auto MatInstPtr = ImagePtr->GetDynamicMaterial();
		if (MatInstPtr)
		{
			const static auto Name = UAssetRefMap::GetInstance()->MatParamNameMap[EMaterialParamNameType::kDestroyProgressColorNameParam];
			MatInstPtr->SetScalarParameterValue(
				Name,
				Progress / 100.f
			);
		}
	}
}

void UDestroyProgress::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!BuildingItemPtr)
	{
		return;
	}

	FVector2D Pos;
	UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(
		UGameplayStatics::GetPlayerController(this, 0),
		BuildingItemPtr->GetActorLocation(),
		Pos,
		true
	);

	auto ImagePtr = Cast<UImage>(GetWidgetFromName(TEXT("Image")));
	if (ImagePtr)
	{
		ImagePtr->SetRenderTranslation(Pos);
	}
}

