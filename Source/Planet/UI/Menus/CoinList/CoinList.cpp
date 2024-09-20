
#include "CoinList.h"

#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include <Blueprint/WidgetTree.h>
#include <Components/HorizontalBox.h>

#include "CharacterBase.h"
#include "TalentAllocationComponent.h"
#include "TalentIcon.h"
#include "CoinInfo.h"

namespace CoinList
{
	const FName HorizontalBox = TEXT("HorizontalBox");
}

void UCoinList::NativeConstruct()
{
	Super::NativeConstruct();
}

void UCoinList::NativeDestruct()
{
	Super::NativeDestruct(); 
}

void UCoinList::ResetUIByData(const TMap<FGameplayTag, TSharedPtr<FCoinProxy>>& CoinMap)
{
	auto UIPtr = Cast<UHorizontalBox>(GetWidgetFromName(CoinList::HorizontalBox));
	if (!UIPtr)
	{
		return;
	}

	UIPtr->ClearChildren();

	for (const auto& Iter : CoinMap)
	{
		auto WidgetPtr = CreateWidget<UCoinInfo>(this, CoinInfoClass);
		if (WidgetPtr)
		{
			WidgetPtr->ResetToolUIByData(Iter.Value);
			UIPtr->AddChild(WidgetPtr);
		}
	}
}
