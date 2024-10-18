
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
		const auto ChildNum = UIPtr->GetChildrenCount();
		if (ChildNum >= MaxDisplayNum)
		{
			OrderAry.Add(UnitPtr);
			SkillPendingAry.Add({ UnitPtr, bIsAdd });
		}
		else
		{
			auto WidgetPtr = CreateWidget<UGetItemInfosItem>(this, GetItemInfosClass);
			if (WidgetPtr)
			{
				WidgetPtr->OnFinished.BindUObject(this, &ThisClass::OnRemovedItem);
				WidgetPtr->ResetToolUIByData(UnitPtr, bIsAdd);

				UIPtr->AddChild(WidgetPtr);
			}
		}
	}
}

void UGetItemInfosList::OnCoinUnitChanged(const TSharedPtr < FCoinProxy>& UnitPtr, bool bIsAdd, int32 Num)
{
	auto UIPtr = Cast<UVerticalBox>(GetWidgetFromName(FGetItemInfosList::Get().VerticalBox));
	if (UIPtr)
	{
		const auto ChildNum = UIPtr->GetChildrenCount();
		if (ChildNum >= MaxDisplayNum)
		{
			OrderAry.Add(UnitPtr);
			CoinPendingAry.Add({ UnitPtr, bIsAdd ,Num });
		}
		else
		{
			auto WidgetPtr = CreateWidget<UGetItemInfosItem>(this, GetItemInfosClass);
			if (WidgetPtr)
			{
				WidgetPtr->OnFinished.BindUObject(this, &ThisClass::OnRemovedItem);
				WidgetPtr->ResetToolUIByData(UnitPtr, bIsAdd, Num);

				UIPtr->AddChild(WidgetPtr);
			}
		}
	}
}

void UGetItemInfosList::OnConsumableUnitChanged(const TSharedPtr < FConsumableProxy>& UnitPtr, EProxyModifyType ProxyModifyType)
{
	auto UIPtr = Cast<UVerticalBox>(GetWidgetFromName(FGetItemInfosList::Get().VerticalBox));
	if (UIPtr)
	{
		const auto ChildNum = UIPtr->GetChildrenCount();
		if (ChildNum >= MaxDisplayNum)
		{
			OrderAry.Add(UnitPtr);
			ConsumablePendingAry.Add({ UnitPtr, ProxyModifyType });
		}
		else
		{
			auto WidgetPtr = CreateWidget<UGetItemInfosItem>(this, GetItemInfosClass);
			if (WidgetPtr)
			{
				WidgetPtr->OnFinished.BindUObject(this, &ThisClass::OnRemovedItem);
				WidgetPtr->ResetToolUIByData(UnitPtr, ProxyModifyType);

				UIPtr->AddChild(WidgetPtr);
			}
		}
	}
}

void UGetItemInfosList::OnGourpmateUnitChanged(const TSharedPtr < FCharacterProxy>& UnitPtr, bool bIsAdd)
{
	auto UIPtr = Cast<UVerticalBox>(GetWidgetFromName(FGetItemInfosList::Get().VerticalBox));
	if (UIPtr)
	{
		const auto ChildNum = UIPtr->GetChildrenCount();
		if (ChildNum >= MaxDisplayNum)
		{
			OrderAry.Add(UnitPtr);
			CharacterPendingAry.Add({ UnitPtr, bIsAdd });
		}
		else
		{
			auto WidgetPtr = CreateWidget<UGetItemInfosItem>(this, GetItemInfosClass);
			if (WidgetPtr)
			{
				WidgetPtr->OnFinished.BindUObject(this, &ThisClass::OnRemovedItem);
				WidgetPtr->ResetToolUIByData(UnitPtr, bIsAdd);

				UIPtr->AddChild(WidgetPtr);
			}
		}
	}
}

void UGetItemInfosList::OnRemovedItem()
{
	for (int32 Index = OrderAry.Num() - 1; Index >= 0; Index--)
	{
		for (int32 SecondIndex = 0; SecondIndex < SkillPendingAry.Num(); SecondIndex++)
		{
			if (OrderAry[Index] == SkillPendingAry[SecondIndex].Get<0>())
			{
				OnSkillUnitChanged(SkillPendingAry[SecondIndex].Get<0>().Pin(), SkillPendingAry[SecondIndex].Get<1>());

				OrderAry.RemoveAt(Index);
				SkillPendingAry.RemoveAt(SecondIndex);
				return;
			}
		}
		
		for (int32 SecondIndex = 0; SecondIndex < CoinPendingAry.Num(); SecondIndex++)
		{
			if (OrderAry[Index] == CoinPendingAry[SecondIndex].Get<0>())
			{
				OnCoinUnitChanged(CoinPendingAry[
					SecondIndex].Get<0>().Pin(), CoinPendingAry[SecondIndex].Get<1>(), CoinPendingAry[SecondIndex].Get<2>()
						);

				OrderAry.RemoveAt(Index);
				SkillPendingAry.RemoveAt(SecondIndex);
				return;
			}
		}
		
		for (int32 SecondIndex = 0; SecondIndex < ConsumablePendingAry.Num(); SecondIndex++)
		{
			if (OrderAry[Index] == ConsumablePendingAry[SecondIndex].Get<0>())
			{
				OnConsumableUnitChanged(ConsumablePendingAry[SecondIndex].Get<0>().Pin(), ConsumablePendingAry[SecondIndex].Get<1>());

				OrderAry.RemoveAt(Index);
				ConsumablePendingAry.RemoveAt(SecondIndex);
				return;
			}
		}

		for (int32 SecondIndex = 0; SecondIndex < CharacterPendingAry.Num(); SecondIndex++)
		{
			if (OrderAry[Index] == CharacterPendingAry[SecondIndex].Get<0>())
			{
				OnGourpmateUnitChanged(CharacterPendingAry[SecondIndex].Get<0>().Pin(), CharacterPendingAry[SecondIndex].Get<1>());

				OrderAry.RemoveAt(Index);
				CharacterPendingAry.RemoveAt(SecondIndex);
				return;
			}
		}
	}
}
