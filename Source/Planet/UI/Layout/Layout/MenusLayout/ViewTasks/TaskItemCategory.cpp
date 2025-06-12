#include "TaskItemCategory.h"

#include "Components/TextBlock.h"

#include "TemplateHelper.h"
#include "TextCollect.h"
#include "TextSubSystem.h"

struct FUTaskItemCategory : public TStructVariable<FUTaskItemCategory>
{
	FName Text = TEXT("Text");
};

void UTaskItemCategory::SetTaskType(
	EQuestChainType GuideThreadType
)
{
	auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FUTaskItemCategory::Get().Text));
	if (UIPtr)
	{
		switch (GuideThreadType)
		{
		case EQuestChainType::kMain:
			{
				UIPtr->SetText(FText::FromString(UTextSubSystem::GetInstance()->GetText(TextCollect::MainThread)));
			}
			break;
		case EQuestChainType::kBrand:
			{
				UIPtr->SetText(FText::FromString(UTextSubSystem::GetInstance()->GetText(TextCollect::BrandThread)));
			}
			break;
		default: ;
		}
	}
}
