
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

void UGetItemInfos::OnSkillUnitChanged(const TSharedPtr < FSkillProxy>& UnitPtr, bool bIsAdd)
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

void UGetItemInfos::OnCoinUnitChanged(const TSharedPtr < FCoinProxy>& UnitPtr, bool bIsAdd, int32 Num)
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

void UGetItemInfos::OnConsumableUnitChanged(const TSharedPtr < FConsumableProxy>& UnitPtr, EProxyModifyType ProxyModifyType)
{
	auto UIPtr = Cast<UVerticalBox>(GetWidgetFromName(GetItemInfos::VerticalBox));
	if (UIPtr)
	{
		auto WidgetPtr = CreateWidget<UGetItemInfosItem>(this, GetItemInfosClass);
		if (WidgetPtr)
		{
			WidgetPtr->ResetToolUIByData(UnitPtr, ProxyModifyType);

			UIPtr->AddChild(WidgetPtr);
		}
	}
}

void UGetItemInfos::OnGourpmateUnitChanged(const TSharedPtr < FCharacterProxy>& UnitPtr, bool bIsAdd)
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
