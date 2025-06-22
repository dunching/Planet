#include "BackpackMenu.h"

#include <Kismet/GameplayStatics.h>
#include <Components/Button.h>

#include "BackpackIcon.h"
#include "Components/TileView.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/Border.h"
#include "StateTagExtendInfo.h"
#include "AssetRefMap.h"
#include "InventoryComponent.h"
#include "GenerateTypes.h"
#include "BackpackToolIcon.h"
#include "BackpackConsumableIcon.h"
#include "BackpackIconWrapper.h"
#include "GameplayTagsLibrary.h"
#include "UICommon.h"
#include "ItemProxy_Container.h"
#include "CharacterBase.h"
#include "CoinList.h"
#include "ItemProxy_Character.h"
#include "ModifyItemProxyStrategy.h"
#include "PlanetPlayerController.h"

struct FBackpackMenu : public TStructVariable<FBackpackMenu>
{
	const FName BackpackTile = TEXT("BackpackTile");

	const FName WeaponBtn = TEXT("WeaponBtn");

	const FName SkillBtn = TEXT("SkillBtn");

	const FName ConsumableBtn = TEXT("ConsumableBtn");

	const FName ShowAllBtn = TEXT("ShowAllBtn");

	const FName CoinList = TEXT("CoinList");
};

void UBackpackMenu::NativeConstruct()
{
	Super::NativeConstruct();

	BindEvent();
}

void UBackpackMenu::EnableMenu()
{
	auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
	auto InventoryComponentPtr = PCPtr->GetInventoryComponent();
	{
		auto ModifyItemProxyStrategySPtr = InventoryComponentPtr->GetModifyItemProxyStrategy<
			FModifyItemProxyStrategy_Weapon>();
		if (ModifyItemProxyStrategySPtr)
		{
			OnWeaponProxyChangedDelagateHandle =
				ModifyItemProxyStrategySPtr->OnWeaponProxyChanged.AddCallback(
				                                                              std::bind(
					                                                               &ThisClass::AddProxy,
					                                                               this,
					                                                               std::placeholders::_1
					                                                              )
				                                                             );
		}
	}
	{
		auto ModifyItemProxyStrategySPtr = InventoryComponentPtr->GetModifyItemProxyStrategy<
			FModifyItemProxyStrategy_Consumable>();
		if (ModifyItemProxyStrategySPtr)
		{
			OnConsumableProxyChangedDelagateHandle =
				ModifyItemProxyStrategySPtr->OnConsumableProxyChanged.AddCallback(
					 std::bind(
					           &ThisClass::AddConsumableProxy,
					           this,
					           std::placeholders::_1
					          )
					);
		}
	}
	{
		auto ModifyItemProxyStrategySPtr = InventoryComponentPtr->GetModifyItemProxyStrategy<
			FModifyItemProxyStrategy_ActiveSkill>();
		if (ModifyItemProxyStrategySPtr)
		{
			OnActiveSkillProxyChangedDelagateHandle =
				ModifyItemProxyStrategySPtr->OnSkillProxyChanged.AddCallback(
				                                                             std::bind(
					                                                              &ThisClass::AddSkillProxy,
					                                                              this,
					                                                              std::placeholders::_1
					                                                             )
				                                                            );
		}
	}
	{
		auto ModifyItemProxyStrategySPtr = InventoryComponentPtr->GetModifyItemProxyStrategy<
			FModifyItemProxyStrategy_PassveSkill>();
		if (ModifyItemProxyStrategySPtr)
		{
			OnPassiveSkillProxyChangedDelagateHandle =
				ModifyItemProxyStrategySPtr->OnSkillProxyChanged.AddCallback(
				                                                             std::bind(
					                                                              &ThisClass::AddSkillProxy,
					                                                              this,
					                                                              std::placeholders::_1
					                                                             )
				                                                            );
		}
	}
	{
		auto ModifyItemProxyStrategySPtr = InventoryComponentPtr->GetModifyItemProxyStrategy<
			FModifyItemProxyStrategy_MaterialProxy>();
		if (ModifyItemProxyStrategySPtr)
		{
			OnMaterialProxyChangedDelagateHandle =
				ModifyItemProxyStrategySPtr->OnProxyChanged.AddCallback(
				                                                        std::bind(
					                                                         &ThisClass::AddMaterialProxy,
					                                                         this,
					                                                         std::placeholders::_1,
					                                                         std::placeholders::_2
					                                                        )
				                                                       );
		}
	}

	ResetUIByData_All();

	auto UIPtr = Cast<UCoinList>(GetWidgetFromName(FBackpackMenu::Get().CoinList));
	if (!UIPtr)
	{
		return;
	}
	UIPtr->Enable();
}

void UBackpackMenu::DisEnableMenu()
{
	OnWeaponProxyChangedDelagateHandle.Reset();
	OnMaterialProxyChangedDelagateHandle.Reset();
	OnConsumableProxyChangedDelagateHandle.Reset();
	OnActiveSkillProxyChangedDelagateHandle.Reset();
	OnPassiveSkillProxyChangedDelagateHandle.Reset();

	auto UIPtr = Cast<UCoinList>(GetWidgetFromName(FBackpackMenu::Get().CoinList));
	if (!UIPtr)
	{
		return;
	}
	UIPtr->DisEnable();
}

EMenuType UBackpackMenu::GetMenuType() const
{
	return EMenuType::kRaffle;
}

TArray<TSharedPtr<FBasicProxy>> UBackpackMenu::GetProxys() const
{
	TArray<TSharedPtr<FBasicProxy>> Result;
	if (CurrentProxyPtr && CurrentProxyPtr->GetCharacterActor().IsValid())
	{
		Result = CurrentProxyPtr->GetCharacterActor()->GetInventoryComponent()->GetProxys();
	}
	return Result;
}

void UBackpackMenu::ResetUIByData_Skill()
{
	auto TileViewPtr = Cast<UTileView>(GetWidgetFromName(FBackpackMenu::Get().BackpackTile));
	if (!TileViewPtr)
	{
		return;
	}

	TileViewPtr->ClearListItems();
	auto EntryClass = TileViewPtr->GetEntryWidgetClass();

	auto ItemAryRef = GetProxys();
	for (const auto& Iter : ItemAryRef)
	{
		if (!Iter)
		{
			continue;
		}
		if (
			Iter->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Active) ||
			Iter->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Passve)
		)
		{
			AddProxy(Iter);
		}
	}
}

void UBackpackMenu::AddSkillProxy(
	const TSharedPtr<FSkillProxy>& ProxySPtr
	)
{
	if (BackpackViewFilterFlags.test(EBackpackViewFilter::kSkill))
	{
		AddProxy(ProxySPtr);
	}
}

void UBackpackMenu::ResetUIByData_Weapon()
{
	BackpackViewFilterFlags.reset();
	BackpackViewFilterFlags.set(EBackpackViewFilter::kWeapon);

	auto TileViewPtr = Cast<UTileView>(GetWidgetFromName(FBackpackMenu::Get().BackpackTile));
	if (!TileViewPtr)
	{
		return;
	}

	TileViewPtr->ClearListItems();
	auto ItemAryRef = GetProxys();
	for (const auto& Iter : ItemAryRef)
	{
		if (!Iter)
		{
			continue;
		}
		if (
			Iter->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Weapon)
		)
		{
			AddWeaponProxy(DynamicCastSharedPtr<FWeaponProxy>(Iter));
		}
	}
}

void UBackpackMenu::AddWeaponProxy(
	const TSharedPtr<FWeaponProxy>& ProxySPtr
	)
{
	if (BackpackViewFilterFlags.test(EBackpackViewFilter::kWeapon))
	{
		AddProxy(ProxySPtr);
	}
}

void UBackpackMenu::ResetUIByData_Consumable()
{
	auto TileViewPtr = Cast<UTileView>(GetWidgetFromName(FBackpackMenu::Get().BackpackTile));
	if (!TileViewPtr)
	{
		return;
	}

	TileViewPtr->ClearListItems();
	auto EntryClass = TileViewPtr->GetEntryWidgetClass();
	auto ItemAryRef = GetProxys();
	for (const auto& Iter : ItemAryRef)
	{
		if (!Iter)
		{
			continue;
		}
		if (
			Iter->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Consumables)
		)
		{
			AddProxy(Iter);
		}
	}
}

void UBackpackMenu::AddConsumableProxy(
	const TSharedPtr<FConsumableProxy>& ProxySPtr
	)
{
	if (BackpackViewFilterFlags.test(EBackpackViewFilter::kConsumable))
	{
		AddProxy(ProxySPtr);
	}
}

void UBackpackMenu::ResetUIByData_Material()
{
	BackpackViewFilterFlags.reset();
	BackpackViewFilterFlags.set(EBackpackViewFilter::kMaterial);

	auto TileViewPtr = Cast<UTileView>(GetWidgetFromName(FBackpackMenu::Get().BackpackTile));
	if (!TileViewPtr)
	{
		return;
	}

	TileViewPtr->ClearListItems();
	auto ItemAryRef = GetProxys();
	for (const auto& Iter : ItemAryRef)
	{
		if (!Iter)
		{
			continue;
		}
		if (
			Iter->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Material)
		)
		{
			AddProxy(Iter);
		}
	}
}

void UBackpackMenu::AddMaterialProxy(
	const TSharedPtr<FMaterialProxy>& ProxySPtr,
	EProxyModifyType ProxyModifyType
	)
{
	if (BackpackViewFilterFlags.test(EBackpackViewFilter::kMaterial))
	{
		switch (ProxyModifyType)
		{
		case EProxyModifyType::kPropertyChange:
		case EProxyModifyType::kNumChanged:
			{
				AddProxy(ProxySPtr);
			}
			break;
		case EProxyModifyType::kRemove:
			{
				RemoveProxy(ProxySPtr);
			}
			break;
		}
	}
}

void UBackpackMenu::ResetUIByData_All()
{
	BackpackViewFilterFlags.set();

	auto TileViewPtr = Cast<UTileView>(GetWidgetFromName(FBackpackMenu::Get().BackpackTile));
	if (!TileViewPtr)
	{
		return;
	}

	TileViewPtr->ClearListItems();
	auto EntryClass = TileViewPtr->GetEntryWidgetClass();
	auto ItemAryRef = GetProxys();
	for (const auto& Iter : ItemAryRef)
	{
		if (!Iter)
		{
			continue;
		}
		if (
			Iter->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Weapon) ||
			Iter->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Active) ||
			Iter->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Passve) ||
			Iter->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Tool) ||
			Iter->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Consumables) ||
			Iter->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Material)
		)
		{
			AddProxy(Iter);
		}
	}
}

void UBackpackMenu::AddProxy(
	const TSharedPtr<FBasicProxy>& ProxySPtr
	)
{
	auto TileViewPtr = Cast<UTileView>(GetWidgetFromName(FBackpackMenu::Get().BackpackTile));
	if (!TileViewPtr)
	{
		return;
	}

	auto EntryClass = TileViewPtr->GetEntryWidgetClass();
	auto WidgetPtr = CreateWidget<UBackpackIconWrapper>(this, EntryClass);
	if (WidgetPtr)
	{
		WidgetPtr->TargetBasicProxyPtr = ProxySPtr;
		WidgetPtr->AllocationSkillsMenuPtr = AllocationSkillsMenuPtr;
		TileViewPtr->AddItem(WidgetPtr);
	}
}

void UBackpackMenu::RemoveProxy(
	const TSharedPtr<FBasicProxy>& ProxySPtr
	)
{
	auto TileViewPtr = Cast<UTileView>(GetWidgetFromName(FBackpackMenu::Get().BackpackTile));
	if (!TileViewPtr)
	{
		return;
	}

	auto Items = TileViewPtr->GetListItems();
	for (auto Iter : Items)
	{
		auto WrapperPtr = Cast<UBackpackIconWrapper>(Iter);
		if (WrapperPtr && WrapperPtr->TargetBasicProxyPtr == ProxySPtr)
		{
			TileViewPtr->RemoveItem(WrapperPtr);
			return;
		}
	}
}

void UBackpackMenu::BindEvent()
{
	{
		auto BtnPtr = Cast<UButton>(GetWidgetFromName(FBackpackMenu::Get().WeaponBtn));
		if (BtnPtr)
		{
			BtnPtr->OnClicked.AddDynamic(this, &ThisClass::OnWeaponBtnCliked);
		}
	}
	{
		auto BtnPtr = Cast<UButton>(GetWidgetFromName(FBackpackMenu::Get().SkillBtn));
		if (BtnPtr)
		{
			BtnPtr->OnClicked.AddDynamic(this, &ThisClass::OnSkillBtnCliked);
		}
	}
	{
		auto BtnPtr = Cast<UButton>(GetWidgetFromName(FBackpackMenu::Get().ConsumableBtn));
		if (BtnPtr)
		{
			BtnPtr->OnClicked.AddDynamic(this, &ThisClass::OnConsumableBtnCliked);
		}
	}
	{
		if (MaterialBtn)
		{
			MaterialBtn->OnClicked.AddDynamic(this, &ThisClass::OnMaterialBtnCliked);
		}
	}
	{
		auto BtnPtr = Cast<UButton>(GetWidgetFromName(FBackpackMenu::Get().ShowAllBtn));
		if (BtnPtr)
		{
			BtnPtr->OnClicked.AddDynamic(this, &ThisClass::OnShowAllBtnCliked);
		}
	}
}

void UBackpackMenu::OnSkillBtnCliked()
{
	ResetUIByData_Skill();
}

void UBackpackMenu::OnConsumableBtnCliked()
{
	ResetUIByData_Consumable();
}

void UBackpackMenu::OnMaterialBtnCliked()
{
	ResetUIByData_Material();
}

void UBackpackMenu::OnShowAllBtnCliked()
{
	ResetUIByData_All();
}

void UBackpackMenu::OnWeaponBtnCliked()
{
	ResetUIByData_Weapon();
}
