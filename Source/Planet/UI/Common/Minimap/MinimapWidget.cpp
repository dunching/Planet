#include "MinimapWidget.h"

#include "Components/Border.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

#include "HumanViewMapProcessor.h"
#include "InputProcessorSubSystem_Imp.h"
#include "HumanCharacter_Player.h"
#include "PlanetPlayerState.h"
#include "DataTableCollection.h"
#include "Dynamic_SkyBase.h"
#include "MinimapSceneCapture2D.h"
#include "Regions.h"
#include "WeatherSystem.h"
#include "Components/TextBlock.h"

void UMinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	auto MinimapSceneCapture2DPtr = Cast<AMinimapSceneCapture2D>(
	                                                             UGameplayStatics::GetActorOfClass(
		                                                              this,
		                                                              AMinimapSceneCapture2D::StaticClass()
		                                                             )
	                                                            );
	if (PlayerBorder && MinimapSceneCapture2DPtr)
	{
		MinimapSceneCapture2DPtr->OnPlayerAngleChangedDelegate.BindUObject(
		                                                                   this,
		                                                                   &ThisClass::OnPlayerAngleChangedDelegate
		                                                                  );
	}

	if (Button)
	{
		Button->OnClicked.AddDynamic(this, &ThisClass::OnClickedBtn);
	}

	auto PSPtr = GEngine->GetFirstLocalPlayerController(GetWorld())->GetPlayerState<APlanetPlayerState>();
	if (PSPtr)
	{
		auto DelegateHandle = PSPtr->OnRegionChanged.AddCallback(
		                                                         std::bind(
		                                                                   &ThisClass::OnRegionChanged,
		                                                                   this,
		                                                                   std::placeholders::_1
		                                                                  )
		                                                        );
		DelegateHandle->bIsAutoUnregister = false;

		OnRegionChanged(PSPtr->GetRegionTag());
	}

	{
		auto DelegateHandle = UWeatherSystem::GetInstance()->GetDynamicSky()->OnHourChanged.AddCallback(
			 std::bind(
			           &ThisClass::OnHourChanged,
			           this,
			           std::placeholders::_1
			          )
			);
		DelegateHandle->bIsAutoUnregister = false;

		OnHourChanged( UWeatherSystem::GetInstance()->GetDynamicSky()->GetCurrentHour());
	}
}

void UMinimapWidget::OnPlayerAngleChangedDelegate(
	float PlayerAngle
	)
{
	if (PlayerBorder)
	{
		PlayerBorder->SetRenderTransformAngle(PlayerAngle);
	}
}

void UMinimapWidget::OnHourChanged(
	int32 Hour
	)
{
	if (TimeText)
	{
		if (Hour > 12)
		{
			TimeText->SetText(FText::FromString(FString::Printf(TEXT("下午 %d"), Hour - 12)));
		}
		else
		{
			TimeText->SetText(FText::FromString(FString::Printf(TEXT("上午 %d"), Hour)));
		}
	}
}

inline void UMinimapWidget::OnClickedBtn()
{
	UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<HumanProcessor::FHumanViewMapProcessor>();
}

void UMinimapWidget::OnRegionChanged(
	const FGameplayTag& RegionTag
	)
{
	auto TableRow_RegionsPtr = UDataTableCollection::GetInstance()->GetTableRow_Region(RegionTag);
	if (TableRow_RegionsPtr)
	{
		if (RegionNameText)
		{
			RegionNameText->SetText(FText::FromString(TableRow_RegionsPtr->Name));
		}
	}
}
