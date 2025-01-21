#include "GuideList.h"
#include "PawnStateConsumablesHUD.h"

#include <Components/GridPanel.h>
#include <Kismet/GameplayStatics.h>
#include "Components/TextBlock.h"

#include "ActionConsumablesIcon.h"
#include "CharacterBase.h"
#include "GuideActor.h"
#include "GuideSubSystem.h"
#include "GuideThreadActor.h"
#include "ProxyProcessComponent.h"
#include "STT_ExcuteGuideThreadTask.h"
#include "TaskNode_Guide.h"

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
	UGuideSubSystem::GetInstance()->OnGuideEnd.AddUObject(this, &ThisClass::OnGuideEnd);
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
	auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FGuideList::Get().Description));
	if (!UIPtr)
	{
		return;
	}

	const auto Description = CurrentTaskNode.Description;
	UIPtr->SetText(FText::FromString(Description));
}
