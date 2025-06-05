#include "UpgradePromt.h"

#include "Regions.h"
#include "TemplateHelper.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

struct FUpgradePromt : public TStructVariable<FUpgradePromt>
{
	const FName Text = TEXT("Text");
};

void UUpgradePromt::NativeConstruct()
{
	Super::NativeConstruct();

	//
	UGameplayStatics::SpawnSound2D(
										  this,
										  UpgradePromtSoundRef.LoadSynchronous()
										 );
}

void UUpgradePromt::SetLevel(
	uint8 Level
	)
{
	auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FUpgradePromt::Get().Text));
	if (UIPtr)
	{
		UIPtr->SetText(FText::FromString(FString::Printf(TEXT("LevelUp:%d"), Level)));
	}
}
