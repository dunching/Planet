#include "PropertyEntryDescription.h"

#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"

#include "GameplayTagsLibrary.h"
#include "Regions.h"
#include "TemplateHelper.h"

void UPropertyEntryDescription::SetDta(
	const FGeneratedPropertyEntryInfo& GeneratedPropertyEntryInfo
	)
{
	if (PropertyName)
	{
		PropertyName->SetText(
		                      FText::FromString(
		                                        UGameplayTagsLibrary::GetModifyItemTagName(
			                                         GeneratedPropertyEntryInfo.PropertyTag
			                                        )
		                                       )
		                     );
	}
	if (Value)
	{
		if (GeneratedPropertyEntryInfo.bIsValue)
		{
			Value->SetText(FText::FromString(FString::Printf(TEXT("+%d"), GeneratedPropertyEntryInfo.Value)));
		}
		else
		{
			Value->SetText(FText::FromString(FString::Printf(TEXT("+%d%%"), GeneratedPropertyEntryInfo.Percent)));
		}
	}
	if (WidgetSwitcher)
	{
		const auto LevelOffset = GeneratedPropertyEntryInfo.Level - 1;
		if (LevelOffset >= 0 && WidgetSwitcher->GetNumWidgets() > LevelOffset)
		{
			WidgetSwitcher->SetActiveWidgetIndex(LevelOffset);
		}
	}
}
