
#include "TaskItem.h"

#include "QuestSubSystem.h"
#include "QuestChain.h"
#include "Components/TextBlock.h"

#include "TemplateHelper.h"
#include "Components/Button.h"

struct FUTaskItem : public TStructVariable<FUTaskItem>
{
	FName Text = TEXT("Text");
	
	FName Btn = TEXT("Btn");
};

void UTaskItem::NativeConstruct()
{
	Super::NativeConstruct();
	
	auto UIPtr = Cast<UButton>(GetWidgetFromName(FUTaskItem::Get().Btn));
	if (UIPtr)
	{
		UIPtr->OnClicked.AddDynamic(this, &ThisClass::OnClicked);
	}
}

void UTaskItem::SetGuideThreadType(
	const TSubclassOf<AQuestChain_MainBase>& GuideThreadClass
)
{
	MainGuideThreadClass = GuideThreadClass;
	
	auto DefaultObjPtr = GuideThreadClass.GetDefaultObject();
	if (DefaultObjPtr )
	{
		auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FUTaskItem::Get().Text));
		if (UIPtr)
		{
			UIPtr->SetText(FText::FromString(DefaultObjPtr->GetGuideThreadTitle()));
		}
	}
}

void UTaskItem::SetGuideThreadType(
	const TSubclassOf<AQuestChain_BranchBase>& GuideThreadClass
)
{
	BrandGuideThreadClass = GuideThreadClass;
	
	auto DefaultObjPtr = GuideThreadClass.GetDefaultObject();
	if (DefaultObjPtr )
	{
		auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FUTaskItem::Get().Text));
		if (UIPtr)
		{
			UIPtr->SetText(FText::FromString(DefaultObjPtr->GetGuideThreadTitle()));
		}
	}
}

inline void UTaskItem::OnClicked()
{
	if (bIsSelected)
	{
		return;
	}
	
	SwitchSelected(true);
	OnSeleted.ExecuteIfBound(this);
}

void UTaskItem::SwitchSelected(
	bool bIsSelected_
)
{
	bIsSelected = bIsSelected_;
	SwitchSelected_Blueprint(bIsSelected);
}
