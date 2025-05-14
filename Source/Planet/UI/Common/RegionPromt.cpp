#include "RegionPromt.h"

#include "Regions.h"
#include "TemplateHelper.h"
#include "Components/TextBlock.h"

struct FRegionPromt : public TStructVariable<FRegionPromt>
{
	const FName Text = TEXT("Text");
};


inline void URegionPromt::SetRegionPromt(
	FTableRow_Regions* TableRow_RegionsPtr
	)
{
	if (!TableRow_RegionsPtr)
	{
		return;
	}

	auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FRegionPromt::Get().Text));
	if (UIPtr)
	{
		UIPtr->SetText(FText::FromString(TableRow_RegionsPtr->Name));
	}
}
