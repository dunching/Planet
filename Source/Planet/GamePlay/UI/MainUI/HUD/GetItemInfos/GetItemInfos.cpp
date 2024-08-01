
#include "GetItemInfos.h"

#include <Components/VerticalBox.h>

#include "SceneElement.h"
#include "GetItemInfosItem.h"

namespace GetItemInfos
{
	const FName VerticalBox = TEXT("VerticalBox");
}

#if WITH_EDITOR
void UGetItemInfos::NativeConstruct()
#else
void UGetItemInfos::NativePreConstruct()
#endif
{
#if WITH_EDITOR
	Super::NativeConstruct();
#else
	Super::NativePreConstruct();
#endif

	auto UIPtr = Cast<UVerticalBox>(GetWidgetFromName(GetItemInfos::VerticalBox));
	if (UIPtr)
	{
		UIPtr->ClearChildren();
	}
}

void UGetItemInfos::ResetUIByData()
{
}

void UGetItemInfos::OnSkillUnitChanged(USkillUnit* UnitPtr, bool bIsAdd)
{
	auto UIPtr = Cast<UVerticalBox>(GetWidgetFromName(GetItemInfos::VerticalBox));
	if (UIPtr)
	{
		auto WidgetPtr = CreateWidget<UGetItemInfosItem>(this, GetItemInfosClass);
		if (WidgetPtr)
		{
			WidgetPtr->ResetToolUIByData(UnitPtr, bIsAdd);

			UIPtr->AddChild(WidgetPtr);
		}
	}
}

void UGetItemInfos::OnCoinUnitChanged(UCoinUnit* UnitPtr, bool bIsAdd, int32 Num)
{
	auto UIPtr = Cast<UVerticalBox>(GetWidgetFromName(GetItemInfos::VerticalBox));
	if (UIPtr)
	{
		auto WidgetPtr = CreateWidget<UGetItemInfosItem>(this, GetItemInfosClass);
		if (WidgetPtr)
		{
			WidgetPtr->ResetToolUIByData(UnitPtr, bIsAdd, Num);

			UIPtr->AddChild(WidgetPtr);
		}
	}
}

void UGetItemInfos::OnConsumableUnitChanged(UConsumableUnit* UnitPtr, bool bIsAdd, int32 Num)
{
	auto UIPtr = Cast<UVerticalBox>(GetWidgetFromName(GetItemInfos::VerticalBox));
	if (UIPtr)
	{
		auto WidgetPtr = CreateWidget<UGetItemInfosItem>(this, GetItemInfosClass);
		if (WidgetPtr)
		{
			WidgetPtr->ResetToolUIByData(UnitPtr, bIsAdd, Num);

			UIPtr->AddChild(WidgetPtr);
		}
	}
}

void UGetItemInfos::OnGourpmateUnitChanged(UCharacterUnit* UnitPtr, bool bIsAdd)
{
	auto UIPtr = Cast<UVerticalBox>(GetWidgetFromName(GetItemInfos::VerticalBox));
	if (UIPtr)
	{
		auto WidgetPtr = CreateWidget<UGetItemInfosItem>(this, GetItemInfosClass);
		if (WidgetPtr)
		{
			WidgetPtr->ResetToolUIByData(UnitPtr, bIsAdd);

			UIPtr->AddChild(WidgetPtr);
		}
	}
}
