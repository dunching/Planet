#include "GuideItem.h"

#include "PawnStateConsumablesHUD.h"

#include <Components/GridPanel.h>
#include <Kismet/GameplayStatics.h>
#include "Components/TextBlock.h"

#include "ActionConsumablesIcon.h"
#include "CharacterBase.h"
#include "QuestsActorBase.h"
#include "QuestSubSystem.h"
#include "QuestChain.h"
#include "PlanetRichTextBlock.h"
#include "ProxyProcessComponent.h"
#include "STT_QuestChain.h"

struct FUGuideItem : public TStructVariable<FUGuideItem>
{
	const FName Name = TEXT("Name");

	const FName Description = TEXT("Description");
};

void UGuideItem::NativeConstruct()
{
	Super::NativeConstruct();
}

void UGuideItem::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UGuideItem::NativeDestruct()
{
	UQuestSubSystem::GetInstance()->GetOnStopGuide().Remove(OnStartGuideHandle);
	
	Super::NativeDestruct();
}

void UGuideItem::BindGuide(AQuestChainBase* NewGuidePtr)
{

	CurrentLineGuidePtr = NewGuidePtr;
	if (NewGuidePtr)
	{
		SetVisibility(ESlateVisibility::Visible);
		
		NewGuidePtr->OnGuideThreadNameChagned.AddUObject(this, &ThisClass::OnGuideThreadPropertyChagned);
		
		OnGuideThreadPropertyChagned(NewGuidePtr->GetGuideThreadTitle());
		
		NewGuidePtr->OnCurrentTaskNodeChanged.AddUObject(this, &ThisClass::OnCurrentTaskNodeChanged);

		OnCurrentTaskNodeChanged(NewGuidePtr->GetCurrentTaskNodeDescript());
	}
	else
	{
	}
}

void UGuideItem::Enable()
{
	ILayoutItemInterfacetion::Enable();
	
	UQuestSubSystem::GetInstance()->GetOnStopGuide().AddUObject(this, &ThisClass::OnStopGuide);
}

void UGuideItem::DisEnable()
{
	ILayoutItemInterfacetion::DisEnable();
}

void UGuideItem::OnStopGuide(AQuestChainBase* NewGuidePtr)
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

void UGuideItem::OnGuideThreadPropertyChagned(const FString& NewTaskName)
{
	auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FUGuideItem::Get().Name));
	if (!UIPtr)
	{
		return;
	}
	
	UIPtr->SetText(FText::FromString(NewTaskName));
}
