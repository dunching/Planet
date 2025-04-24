#include "GuideItem.h"

#include "PawnStateConsumablesHUD.h"

#include <Components/GridPanel.h>
#include <Kismet/GameplayStatics.h>
#include "Components/TextBlock.h"

#include "ActionConsumablesIcon.h"
#include "CharacterBase.h"
#include "GuideActor.h"
#include "GuideSubSystem.h"
#include "GuideThread.h"
#include "PlanetRichTextBlock.h"
#include "ProxyProcessComponent.h"
#include "STT_GuideThread.h"

struct FUGuideItem : public TStructVariable<FUGuideItem>
{
	const FName Name = TEXT("Name");

	const FName Description = TEXT("Description");
};

void UGuideItem::NativeConstruct()
{
	Super::NativeConstruct();

	ResetUIByData();
}

void UGuideItem::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UGuideItem::NativeDestruct()
{
	UGuideSubSystem::GetInstance()->GetOnStopGuide().Remove(OnStartGuideHandle);
	
	Super::NativeDestruct();
}

void UGuideItem::ResetUIByData()
{
	UGuideSubSystem::GetInstance()->GetOnStopGuide().AddUObject(this, &ThisClass::OnStopGuide);
}

void UGuideItem::BindGuide(AGuideThread* NewGuidePtr)
{
	auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FUGuideItem::Get().Name));
	if (!UIPtr)
	{
		return;
	}

	CurrentLineGuidePtr = NewGuidePtr;
	if (NewGuidePtr)
	{
		SetVisibility(ESlateVisibility::Visible);
		
		NewGuidePtr->OnCurrentTaskNodeChanged.AddUObject(this, &ThisClass::OnCurrentTaskNodeChanged);

		UIPtr->SetText(FText::FromString(NewGuidePtr->TaskName));

		OnCurrentTaskNodeChanged(FTaskNodeDescript::Refresh);
	}
	else
	{
	}
}

void UGuideItem::OnStopGuide(AGuideThread* NewGuidePtr)
{
	if (CurrentLineGuidePtr == NewGuidePtr)
	{
		RemoveFromParent();
	}
}

void UGuideItem::OnCurrentTaskNodeChanged(const FTaskNodeDescript& CurrentTaskNode)
{
	if (
		CurrentTaskNode.bIsOnlyFresh ||
		(CurrentTaskNode.GetIsValid() && CurrentTaskNode.bIsFreshPreviouDescription)
		)
	{
		auto UIPtr = Cast<URichTextBlock_Button>(GetWidgetFromName(FUGuideItem::Get().Description));
		if (!UIPtr)
		{
			return;
		}

		const auto Description = CurrentTaskNode.Description;
		UIPtr->SetText(FText::FromString(Description));
	}
}
