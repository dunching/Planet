#include "GuideList.h"
#include "PawnStateConsumablesHUD.h"

#include <Components/GridPanel.h>
#include <Kismet/GameplayStatics.h>
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"

#include "ActionConsumablesIcon.h"
#include "CharacterBase.h"
#include "GuideActor.h"
#include "GuideSubSystem.h"
#include "GuideThread.h"
#include "PlanetRichTextBlock.h"
#include "ProxyProcessComponent.h"
#include "GuideItem.h"

struct FGuideList : public TStructVariable<FGuideList>
{
	const FName Name = TEXT("Name");

	const FName Description = TEXT("Description");

	const FName VerticalBox = TEXT("VerticalBox");
};

void UGuideList::NativeConstruct()
{
	Super::NativeConstruct();
}

void UGuideList::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UGuideList::Enable()
{
	ILayoutItemInterfacetion::Enable();
	
	auto UIPtr = Cast<UVerticalBox>(GetWidgetFromName(FGuideList::Get().VerticalBox));
	if (UIPtr)
	{
		UIPtr->ClearChildren();
	}
	
	UGuideSubSystem::GetInstance()->GetOnStartGuide().AddUObject(this, &ThisClass::OnStartGuide);
	OnStartGuide(UGuideSubSystem::GetInstance()->GetCurrentGuideThread());
}

void UGuideList::DisEnable()
{
	ILayoutItemInterfacetion::DisEnable();
}

void UGuideList::OnStartGuide(AGuideThread* NewGuidePtr)
{
	if (NewGuidePtr)
	{
		auto UIPtr = Cast<UVerticalBox>(GetWidgetFromName(FGuideList::Get().VerticalBox));
		if (!UIPtr)
		{
			return;
		}

		auto ItemUIPtr = CreateWidget<UGuideItem>(this, GuideItemClass);
		if (ItemUIPtr)
		{
			ItemUIPtr->BindGuide(NewGuidePtr);
			UIPtr->AddChild(ItemUIPtr);
		}
	}
}
