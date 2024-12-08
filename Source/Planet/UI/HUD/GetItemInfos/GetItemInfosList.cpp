#include "GetItemInfosList.h"

#include <Components/VerticalBox.h>

#include "ItemProxy_Minimal.h"
#include "GetItemInfosItem.h"
#include "TemplateHelper.h"
#include "ItemProxy_Character.h"

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

void UGetItemInfosList::OnSkillProxyChanged(const TSharedPtr<FSkillProxy>& ProxyPtr, bool bIsAdd)
{
	auto UIPtr = Cast<UVerticalBox>(GetWidgetFromName(FGetItemInfosList::Get().VerticalBox));
	if (UIPtr)
	{
		const auto ChildNum = UIPtr->GetChildrenCount();
		if (ChildNum >= MaxDisplayNum)
		{
			OrderAry.Add(ProxyPtr);
			SkillPendingAry.Add({ProxyPtr, bIsAdd});
		}
		else
		{
			auto WidgetPtr = CreateWidget<UGetItemInfosItem>(this, GetItemInfosClass);
			if (WidgetPtr)
			{
				WidgetPtr->OnFinished.BindUObject(this, &ThisClass::OnRemovedItem);
				WidgetPtr->ResetToolUIByData(ProxyPtr, bIsAdd);

				UIPtr->AddChild(WidgetPtr);
			}
		}
	}
}

void UGetItemInfosList::OnCoinProxyChanged(const TSharedPtr<FCoinProxy>& ProxyPtr, bool bIsAdd, int32 Num)
{
	auto UIPtr = Cast<UVerticalBox>(GetWidgetFromName(FGetItemInfosList::Get().VerticalBox));
	if (UIPtr)
	{
		const auto ChildNum = UIPtr->GetChildrenCount();
		if (ChildNum >= MaxDisplayNum)
		{
			OrderAry.Add(ProxyPtr);
			CoinPendingAry.Add({ProxyPtr, bIsAdd, Num});
		}
		else
		{
			auto WidgetPtr = CreateWidget<UGetItemInfosItem>(this, GetItemInfosClass);
			if (WidgetPtr)
			{
				WidgetPtr->OnFinished.BindUObject(this, &ThisClass::OnRemovedItem);
				WidgetPtr->ResetToolUIByData(ProxyPtr, bIsAdd, Num);

				UIPtr->AddChild(WidgetPtr);
			}
		}
	}
}

void UGetItemInfosList::OnConsumableProxyChanged(const TSharedPtr<FConsumableProxy>& ProxyPtr,
                                                EProxyModifyType ProxyModifyType)
{
	auto UIPtr = Cast<UVerticalBox>(GetWidgetFromName(FGetItemInfosList::Get().VerticalBox));
	if (UIPtr)
	{
		const auto ChildNum = UIPtr->GetChildrenCount();
		if (ChildNum >= MaxDisplayNum)
		{
			OrderAry.Add(ProxyPtr);
			ConsumablePendingAry.Add({ProxyPtr, ProxyModifyType});
		}
		else
		{
			auto WidgetPtr = CreateWidget<UGetItemInfosItem>(this, GetItemInfosClass);
			if (WidgetPtr)
			{
				WidgetPtr->OnFinished.BindUObject(this, &ThisClass::OnRemovedItem);
				WidgetPtr->ResetToolUIByData(ProxyPtr, ProxyModifyType);

				UIPtr->AddChild(WidgetPtr);
			}
		}
	}
}

void UGetItemInfosList::OnGourpmateProxyChanged(const TSharedPtr<FCharacterProxy>& ProxyPtr, bool bIsAdd)
{
	auto UIPtr = Cast<UVerticalBox>(GetWidgetFromName(FGetItemInfosList::Get().VerticalBox));
	if (UIPtr)
	{
		const auto ChildNum = UIPtr->GetChildrenCount();
		if (ChildNum >= MaxDisplayNum)
		{
			OrderAry.Add(ProxyPtr);
			CharacterPendingAry.Add({ProxyPtr, bIsAdd});
		}
		else
		{
			auto WidgetPtr = CreateWidget<UGetItemInfosItem>(this, GetItemInfosClass);
			if (WidgetPtr)
			{
				WidgetPtr->OnFinished.BindUObject(this, &ThisClass::OnRemovedItem);
				WidgetPtr->ResetToolUIByData(ProxyPtr, bIsAdd);

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
				OnSkillProxyChanged(SkillPendingAry[SecondIndex].Get<0>().Pin(), SkillPendingAry[SecondIndex].Get<1>());

				OrderAry.RemoveAt(Index);
				SkillPendingAry.RemoveAt(SecondIndex);
				return;
			}
		}

		for (int32 SecondIndex = 0; SecondIndex < CoinPendingAry.Num(); SecondIndex++)
		{
			if (OrderAry[Index] == CoinPendingAry[SecondIndex].Get<0>())
			{
				OnCoinProxyChanged(CoinPendingAry[
					                  SecondIndex].Get<0>().Pin(), CoinPendingAry[SecondIndex].Get<1>(),
				                  CoinPendingAry[SecondIndex].Get<2>()
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
				OnConsumableProxyChanged(ConsumablePendingAry[SecondIndex].Get<0>().Pin(),
				                        ConsumablePendingAry[SecondIndex].Get<1>());

				OrderAry.RemoveAt(Index);
				ConsumablePendingAry.RemoveAt(SecondIndex);
				return;
			}
		}

		for (int32 SecondIndex = 0; SecondIndex < CharacterPendingAry.Num(); SecondIndex++)
		{
			if (OrderAry[Index] == CharacterPendingAry[SecondIndex].Get<0>())
			{
				OnGourpmateProxyChanged(CharacterPendingAry[SecondIndex].Get<0>().Pin(),
				                       CharacterPendingAry[SecondIndex].Get<1>());

				OrderAry.RemoveAt(Index);
				CharacterPendingAry.RemoveAt(SecondIndex);
				return;
			}
		}
	}
}
