#include "GuideList.h"
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

struct FGuideList : public TStructVariable<FGuideList>
{
	const FName Name = TEXT("Name");

	const FName Description = TEXT("Description");
};

void UGuideList::NativeConstruct()
{
	Super::NativeConstruct();

	ResetUIByData();
}

void UGuideList::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UGuideList::ResetUIByData()
{
	UGuideSubSystem::GetInstance()->OnCurrentGuideChagned.AddUObject(this, &ThisClass::OnCurrentGuideChagned);
	UGuideSubSystem::GetInstance()->GetOnGuideEnd().AddUObject(this, &ThisClass::OnGuideEnd);
}

void UGuideList::OnCurrentGuideChagned(AGuideThread* NewGuidePtr)
{
	auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FGuideList::Get().Name));
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

void UGuideList::OnGuideEnd(AGuideThread* NewGuidePtr)
{
	if (CurrentLineGuidePtr == NewGuidePtr)
	{
		auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FGuideList::Get().Name));
		if (!UIPtr)
		{
			return;
		}

		SetVisibility(ESlateVisibility::Hidden);
	}
}

void UGuideList::OnCurrentTaskNodeChanged(const FTaskNodeDescript& CurrentTaskNode)
{
	if (
		CurrentTaskNode.bIsOnlyFresh ||
		(CurrentTaskNode.GetIsValid() && CurrentTaskNode.bIsFreshPreviouDescription)
		)
	{
		auto UIPtr = Cast<URichTextBlock_Button>(GetWidgetFromName(FGuideList::Get().Description));
		if (!UIPtr)
		{
			return;
		}

		const auto Description = CurrentTaskNode.Description;
		UIPtr->SetText(FText::FromString(Description));
	}
}
