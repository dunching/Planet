
#include "TaskItem.h"

#include "GuideSubSystem.h"
#include "GuideThread.h"
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
	const TSubclassOf<AGuideThread_Main>& GuideThreadClass
)
{
	MainGuideThreadClass = GuideThreadClass;
	
	auto DefaultObjPtr = GuideThreadClass.GetDefaultObject();
	if (DefaultObjPtr )
	{
		auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FUTaskItem::Get().Text));
		if (UIPtr)
		{
			UIPtr->SetText(FText::FromString(DefaultObjPtr->TaskName));
		}
	}
}

void UTaskItem::SetGuideThreadType(
	const TSubclassOf<AGuideThread_Branch>& GuideThreadClass
)
{
	BrandGuideThreadClass = GuideThreadClass;
	
	auto DefaultObjPtr = GuideThreadClass.GetDefaultObject();
	if (DefaultObjPtr )
	{
		auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FUTaskItem::Get().Text));
		if (UIPtr)
		{
			UIPtr->SetText(FText::FromString(DefaultObjPtr->TaskName));
		}
	}
}

inline void UTaskItem::OnClicked()
{
	SwitchSeleted(true);
	OnSeleted.ExecuteIfBound(this);
}
