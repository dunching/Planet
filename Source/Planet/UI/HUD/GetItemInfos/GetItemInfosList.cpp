#include "GetItemInfosList.h"

#include <Components/VerticalBox.h>
#include "Kismet/GameplayStatics.h"

#include "CharacterBase.h"
#include "ModifyItemProxyStrategy.h"
#include "GameplayTagsLibrary.h"
#include "ItemProxy_Minimal.h"
#include "GetItemInfosItem.h"
#include "InventoryComponent.h"
#include "TemplateHelper.h"
#include "ItemProxy_Character.h"
#include "ItemProxy_Weapon.h"
#include "ItemProxy_Coin.h"
#include "ItemProxy_Consumable.h"
#include "ItemProxy_Skills.h"
#include "ItemProxy_Material.h"

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

void UGetItemInfosList::SetPlayerCharacter(
	ACharacterBase* PlayeyCharacterPtr
	)
{
	if (!PlayeyCharacterPtr)
	{
		return;
	}

	{
		auto ModifyItemProxyStrategySPtr = PlayeyCharacterPtr->GetInventoryComponent()->GetModifyItemProxyStrategy<
			FModifyItemProxyStrategy_Weapon>();
		if (ModifyItemProxyStrategySPtr)
		{
			auto Handle =
				ModifyItemProxyStrategySPtr->OnWeaponProxyChanged.AddCallback(
				                                                              std::bind(
					                                                               &UGetItemInfosList::OnWeaponProxyChanged,
					                                                               this,
					                                                               std::placeholders::_1,
					                                                               std::placeholders::_2
					                                                              )
				                                                             );
			Handle->bIsAutoUnregister = false;
		}
	}
	{
		auto ModifyItemProxyStrategySPtr = PlayeyCharacterPtr->GetInventoryComponent()->GetModifyItemProxyStrategy<
			FModifyItemProxyStrategy_WeaponSkill>();
		if (ModifyItemProxyStrategySPtr)
		{
			auto Handle =
				ModifyItemProxyStrategySPtr->OnSkillProxyChanged.AddCallback(
					 std::bind(
							   &UGetItemInfosList::OnSkillProxyChanged,
							   this,
							   std::placeholders::_1,
							   std::placeholders::_2
							  )
					);
			Handle->bIsAutoUnregister = false;
		}
	}
	{
		auto ModifyItemProxyStrategySPtr = PlayeyCharacterPtr->GetInventoryComponent()->GetModifyItemProxyStrategy<
			FModifyItemProxyStrategy_ActiveSkill>();
		if (ModifyItemProxyStrategySPtr)
		{
			auto Handle =
				ModifyItemProxyStrategySPtr->OnSkillProxyChanged.AddCallback(
					 std::bind(
							   &UGetItemInfosList::OnSkillProxyChanged,
							   this,
							   std::placeholders::_1,
							   std::placeholders::_2
							  )
					);
			Handle->bIsAutoUnregister = false;
		}
	}
	{
		auto ModifyItemProxyStrategySPtr = PlayeyCharacterPtr->GetInventoryComponent()->GetModifyItemProxyStrategy<
			FModifyItemProxyStrategy_PassveSkill>();
		if (ModifyItemProxyStrategySPtr)
		{
			auto Handle =
				ModifyItemProxyStrategySPtr->OnSkillProxyChanged.AddCallback(
					 std::bind(
							   &UGetItemInfosList::OnSkillProxyChanged,
							   this,
							   std::placeholders::_1,
							   std::placeholders::_2
							  )
					);
			Handle->bIsAutoUnregister = false;
		}
	}
	{
		auto ModifyItemProxyStrategySPtr = PlayeyCharacterPtr->GetInventoryComponent()->GetModifyItemProxyStrategy<
			FModifyItemProxyStrategy_Coin>();
		if (ModifyItemProxyStrategySPtr)
		{
			auto Handle =
				ModifyItemProxyStrategySPtr->OnCoinProxyChanged.AddCallback(
					 std::bind(
							   &UGetItemInfosList::OnCoinProxyChanged,
							   this,
							   std::placeholders::_1,
							   std::placeholders::_2,
							   std::placeholders::_3
							  )
					);
			Handle->bIsAutoUnregister = false;
		}
	}
	{
		auto ModifyItemProxyStrategySPtr = PlayeyCharacterPtr->GetInventoryComponent()->GetModifyItemProxyStrategy<
			FModifyItemProxyStrategy_Consumable>();
		if (ModifyItemProxyStrategySPtr)
		{
			auto Handle =
				ModifyItemProxyStrategySPtr->OnConsumableProxyChanged.AddCallback(
					 std::bind(
							   &UGetItemInfosList::OnConsumableProxyChanged,
							   this,
							   std::placeholders::_1,
							   std::placeholders::_2
							  )
					);
			Handle->bIsAutoUnregister = false;
		}
	}
	{
		auto ModifyItemProxyStrategySPtr = PlayeyCharacterPtr->GetInventoryComponent()->GetModifyItemProxyStrategy<
			FModifyItemProxyStrategy_Character>();
		if (ModifyItemProxyStrategySPtr)
		{
			auto Handle =
				ModifyItemProxyStrategySPtr->OnGroupmateProxyChanged.AddCallback(
					 std::bind(
							   &UGetItemInfosList::OnGourpmateProxyChanged,
							   this,
							   std::placeholders::_1,
							   std::placeholders::_2
							  )
					);
			Handle->bIsAutoUnregister = false;
		}
	}
	{
		auto ModifyItemProxyStrategySPtr = PlayeyCharacterPtr->GetInventoryComponent()->GetModifyItemProxyStrategy<
			FModifyItemProxyStrategy_MaterialProxy>();
		if (ModifyItemProxyStrategySPtr)
		{
			auto Handle =
				ModifyItemProxyStrategySPtr->OnProxyChanged.AddCallback(
					 std::bind(
							   &UGetItemInfosList::OnMaterialProxyChanged,
							   this,
							   std::placeholders::_1,
							   std::placeholders::_2,
							   std::placeholders::_3
							  )
					);
			Handle->bIsAutoUnregister = false;
		}
	}
}

void UGetItemInfosList::OnWeaponProxyChanged(
	const TSharedPtr<FWeaponProxy>& ProxyPtr,
	EProxyModifyType ProxyModifyType
	)
{
	switch (ProxyModifyType)
	{
	case EProxyModifyType::kPropertyChange:
		{
		}
		break;
	default:
		{
			return;
		};
	}

	auto UIPtr = Cast<UVerticalBox>(GetWidgetFromName(FGetItemInfosList::Get().VerticalBox));
	if (UIPtr)
	{
		const auto ChildNum = UIPtr->GetChildrenCount();
		if (ChildNum >= MaxDisplayNum)
		{
			OrderAry.Add(ProxyPtr);
			WeaponPendingAry.Add({ProxyPtr, ProxyModifyType});
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

void UGetItemInfosList::OnSkillProxyChanged(
	const TSharedPtr<FSkillProxy>& ProxyPtr,
	EProxyModifyType ProxyModifyType
	)
{
	switch (ProxyModifyType)
	{
	case EProxyModifyType::kPropertyChange:
		{
			if (ProxyPtr)
			{
				if (ProxyPtr->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Weapon))
				{
					return;
				}
			}
			else
			{
				return;
			}
		}
		break;
	default:
		{
			return;
		};
	}

	auto UIPtr = Cast<UVerticalBox>(GetWidgetFromName(FGetItemInfosList::Get().VerticalBox));
	if (UIPtr)
	{
		const auto ChildNum = UIPtr->GetChildrenCount();
		if (ChildNum >= MaxDisplayNum)
		{
			OrderAry.Add(ProxyPtr);
			SkillPendingAry.Add({ProxyPtr, ProxyModifyType});
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

void UGetItemInfosList::OnCoinProxyChanged(
	const TSharedPtr<FCoinProxy>& ProxyPtr,
	EProxyModifyType ProxyModifyType,
	int32 Num
	)
{
	switch (ProxyModifyType)
	{
	case EProxyModifyType::kPropertyChange:
		{
			if (Num == 0)
			{
				return;
			}
		}
		break;
	default:
		{
			return;
		};
	}

	auto UIPtr = Cast<UVerticalBox>(GetWidgetFromName(FGetItemInfosList::Get().VerticalBox));
	if (UIPtr)
	{
		const auto ChildNum = UIPtr->GetChildrenCount();
		if (ChildNum >= MaxDisplayNum)
		{
			OrderAry.Add(ProxyPtr);
			CoinPendingAry.Add({ProxyPtr, ProxyModifyType, Num});
		}
		else
		{
			auto WidgetPtr = CreateWidget<UGetItemInfosItem>(this, GetItemInfosClass);
			if (WidgetPtr)
			{
				WidgetPtr->OnFinished.BindUObject(this, &ThisClass::OnRemovedItem);
				WidgetPtr->ResetToolUIByData(ProxyPtr, ProxyModifyType, Num);

				UIPtr->AddChild(WidgetPtr);

				//
				UGameplayStatics::SpawnSound2D(
				                               this,
				                               GetCoinsSoundRef.LoadSynchronous()
				                              );
			}
		}
	}
}

void UGetItemInfosList::OnConsumableProxyChanged(
	const TSharedPtr<FConsumableProxy>& ProxyPtr,
	EProxyModifyType ProxyModifyType
	)
{
	switch (ProxyModifyType)
	{
	case EProxyModifyType::kPropertyChange:
		{
		}
		break;
	default:
		{
			return;
		};
	}

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

void UGetItemInfosList::OnGourpmateProxyChanged(
	const TSharedPtr<FCharacterProxy>& ProxyPtr,
	EProxyModifyType ProxyModifyType
	)
{
	switch (ProxyModifyType)
	{
	case EProxyModifyType::kPropertyChange:
		{
		}
		break;
	default:
		{
			return;
		};
	}

	auto UIPtr = Cast<UVerticalBox>(GetWidgetFromName(FGetItemInfosList::Get().VerticalBox));
	if (UIPtr)
	{
		const auto ChildNum = UIPtr->GetChildrenCount();
		if (ChildNum >= MaxDisplayNum)
		{
			OrderAry.Add(ProxyPtr);
			CharacterPendingAry.Add({ProxyPtr, ProxyModifyType});
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

void UGetItemInfosList::OnMaterialProxyChanged(
	const TSharedPtr<FMaterialProxy>& ProxyPtr,
	EProxyModifyType ProxyModifyType,
	int32 Num
	)
{
	switch (ProxyModifyType)
	{
	case EProxyModifyType::kPropertyChange:
		{
		}
		break;
	default:
		{
			return;
		};
	}

	auto UIPtr = Cast<UVerticalBox>(GetWidgetFromName(FGetItemInfosList::Get().VerticalBox));
	if (UIPtr)
	{
		const auto ChildNum = UIPtr->GetChildrenCount();
		if (ChildNum >= MaxDisplayNum)
		{
			OrderAry.Add(ProxyPtr);
			MaterialProxyPendingAry.Add({ProxyPtr, ProxyModifyType});
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
				OnCoinProxyChanged(
				                   CoinPendingAry[
					                   SecondIndex].Get<0>().Pin(),
				                   CoinPendingAry[SecondIndex].Get<1>(),
				                   CoinPendingAry[SecondIndex].Get<2>()
				                  );

				OrderAry.RemoveAt(Index);
				CoinPendingAry.RemoveAt(SecondIndex);
				return;
			}
		}

		for (int32 SecondIndex = 0; SecondIndex < ConsumablePendingAry.Num(); SecondIndex++)
		{
			if (OrderAry[Index] == ConsumablePendingAry[SecondIndex].Get<0>())
			{
				OnConsumableProxyChanged(
				                         ConsumablePendingAry[SecondIndex].Get<0>().Pin(),
				                         ConsumablePendingAry[SecondIndex].Get<1>()
				                        );

				OrderAry.RemoveAt(Index);
				ConsumablePendingAry.RemoveAt(SecondIndex);
				return;
			}
		}

		for (int32 SecondIndex = 0; SecondIndex < CharacterPendingAry.Num(); SecondIndex++)
		{
			if (OrderAry[Index] == CharacterPendingAry[SecondIndex].Get<0>())
			{
				OnGourpmateProxyChanged(
				                        CharacterPendingAry[SecondIndex].Get<0>().Pin(),
				                        CharacterPendingAry[SecondIndex].Get<1>()
				                       );

				OrderAry.RemoveAt(Index);
				CharacterPendingAry.RemoveAt(SecondIndex);
				return;
			}
		}
	}
}
