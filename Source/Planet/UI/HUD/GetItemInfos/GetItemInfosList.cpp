
#include "GetItemInfosList.h"

#include <Components/VerticalBox.h>

#include "SceneElement.h"
#include "GetItemInfosItem.h"
#include "TemplateHelper.h"

struct FGetItemInfosList : public TStructVariable<FGetItemInfosList>
{
	const FName VerticalBox = TEXT("VerticalBox");
};

#if WITH_EDITOR
void UGetItemInfosList::NativeConstruct()
#else
void UGetItemInfosList::NativePreConstruct()
#endif
{
#if WITH_EDITOR
	Super::NativeConstruct();
#else
	Super::NativePreConstruct();
#endif

	auto UIPtr = Cast<UVerticalBox>(GetWidgetFromName(FGetItemInfosList::Get().VerticalBox));
	if (UIPtr)
	{
		UIPtr->ClearChildren();
	}
}

void UGetItemInfosList::ResetUIByData()
{
}

void UGetItemInfosList::OnSkillUnitChanged(const TSharedPtr < FSkillProxy>& UnitPtr, bool bIsAdd)
{
	auto UIPtr = Cast<UVerticalBox>(GetWidgetFromName(FGetItemInfosList::Get().VerticalBox));
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

void UGetItemInfosList::OnCoinUnitChanged(const TSharedPtr < FCoinProxy>& UnitPtr, bool bIsAdd, int32 Num)
{
	auto UIPtr = Cast<UVerticalBox>(GetWidgetFromName(FGetItemInfosList::Get().VerticalBox));
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

void UGetItemInfosList::OnConsumableUnitChanged(const TSharedPtr < FConsumableProxy>& UnitPtr, EProxyModifyType ProxyModifyType)
{
	auto UIPtr = Cast<UVerticalBox>(GetWidgetFromName(FGetItemInfosList::Get().VerticalBox));
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

void UGetItemInfosList::OnGourpmateUnitChanged(const TSharedPtr < FCharacterProxy>& UnitPtr, bool bIsAdd)
{
	auto UIPtr = Cast<UVerticalBox>(GetWidgetFromName(FGetItemInfosList::Get().VerticalBox));
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
