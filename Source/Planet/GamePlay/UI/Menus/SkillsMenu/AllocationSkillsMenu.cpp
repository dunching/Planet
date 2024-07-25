
#include "AllocationSkillsMenu.h"

#include <Kismet/GameplayStatics.h>
#include "Components/Button.h"

#include "BackpackIcon.h"
#include "Components/TileView.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/Border.h"
#include "StateTagExtendInfo.h"
#include "AssetRefMap.h"
#include "SkillsIcon.h"
#include "WeaponsIcon.h"
#include "CharacterBase.h"
#include "InteractiveSkillComponent.h"
#include "UICommon.h"

struct AllocationSkillsMenu : public TGetSocketName<AllocationSkillsMenu>
{
	const FName WeaponsTileView = TEXT("WeaponsTileView");

	const FName SkillsTileView = TEXT("SkillsTileView");

	const FName MainWeapon = TEXT("MainWeapon");

	const FName SecondaryWeapon = TEXT("SecondaryWeapon");

	const FName WeaponBtn = TEXT("WeaponBtn");

	const FName SkillBtn = TEXT("SkillBtn");

	const FName ActiveSkill1 = TEXT("ActiveSkill1");

	const FName ActiveSkill2 = TEXT("ActiveSkill2");

	const FName ActiveSkill3 = TEXT("ActiveSkill3");

	const FName ActiveSkill4 = TEXT("ActiveSkill4");

	const FName WeaponActiveSkill1 = TEXT("WeaponActiveSkill1");

	const FName WeaponActiveSkill2 = TEXT("WeaponActiveSkill2");

	const FName PassivSkill1 = TEXT("PassivSkill1");

	const FName PassivSkill2 = TEXT("PassivSkill2");

	const FName PassivSkill3 = TEXT("PassivSkill3");

	const FName PassivSkill4 = TEXT("PassivSkill4");

	const FName PassivSkill5 = TEXT("PassivSkill5");

	const FName TalentPassivSkill = TEXT("TalentPassivSkill");
};

void UAllocationSkillsMenu::NativeConstruct()
{
	Super::NativeConstruct();

	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!CharacterPtr)
	{
		return;
	}

	BindEvent();

	ResetUIByData();
}

void UAllocationSkillsMenu::NativeDestruct()
{
	ON_SCOPE_EXIT
	{
		Super::NativeDestruct();
	};

	if (MainDelegateHandleSPtr)
	{
		MainDelegateHandleSPtr->UnBindCallback();
	}
	if (SecondaryDelegateHandleSPtr)
	{
		SecondaryDelegateHandleSPtr->UnBindCallback();
	}

	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!CharacterPtr)
	{
		return;
	}

	auto EICPtr = CharacterPtr->GetInteractiveSkillComponent();

	// 武器技能
	{
		TSharedPtr<FWeaponSocketInfo > FirstWeaponSocketInfoSPtr = MakeShared<FWeaponSocketInfo>();
		{
			auto IconPtr = Cast<UWeaponsIcon>(GetWidgetFromName(AllocationSkillsMenu::Get().MainWeapon));
			if (IconPtr && IconPtr->WeaponUnitPtr)
			{
				FirstWeaponSocketInfoSPtr->WeaponSocket = IconPtr->IconSocket;
				FirstWeaponSocketInfoSPtr->WeaponUnitPtr = IconPtr->WeaponUnitPtr;
			}
		}
		TSharedPtr<FWeaponSocketInfo > SecondWeaponSocketInfoSPtr = MakeShared<FWeaponSocketInfo>();
		{
			auto IconPtr = Cast<UWeaponsIcon>(GetWidgetFromName(AllocationSkillsMenu::Get().SecondaryWeapon));
			if (IconPtr && IconPtr->WeaponUnitPtr)
			{
				SecondWeaponSocketInfoSPtr->WeaponSocket = IconPtr->IconSocket;
				SecondWeaponSocketInfoSPtr->WeaponUnitPtr = IconPtr->WeaponUnitPtr;
			}
		}
		EICPtr->RegisterWeapon(FirstWeaponSocketInfoSPtr, SecondWeaponSocketInfoSPtr);
	}
	// 技能
	{
		struct FHelper
		{
			FName Name;
			FKey Key;
		};

		TArray<FHelper>Ary
		{
			{AllocationSkillsMenu::Get().ActiveSkill1,ActiveSkills_1},
			{AllocationSkillsMenu::Get().ActiveSkill2,ActiveSkills_2},
			{AllocationSkillsMenu::Get().ActiveSkill3,ActiveSkills_3},
			{AllocationSkillsMenu::Get().ActiveSkill4,ActiveSkills_4},
			{AllocationSkillsMenu::Get().PassivSkill1,EKeys::Invalid},
			{AllocationSkillsMenu::Get().PassivSkill2,EKeys::Invalid},
			{AllocationSkillsMenu::Get().PassivSkill3,EKeys::Invalid},
			{AllocationSkillsMenu::Get().PassivSkill4,EKeys::Invalid},
			{AllocationSkillsMenu::Get().PassivSkill5,EKeys::Invalid},
//			{AllocationSkillsMenu::Get().TalentPassivSkill,EKeys::Invalid},
		};

		TMap<FGameplayTag, TSharedPtr<FSkillSocketInfo>> SkillsMap;

		for (auto Iter : Ary)
		{
			auto IconPtr = Cast<USkillsIcon>(GetWidgetFromName(Iter.Name));
			if (IconPtr && IconPtr->SkillUnitPtr)
			{
				TSharedPtr<FSkillSocketInfo >SkillsSocketInfo = MakeShared<FSkillSocketInfo>();

				SkillsSocketInfo->SkillSocket = IconPtr->IconSocket;
				SkillsSocketInfo->SkillUnit = IconPtr->SkillUnitPtr;
				SkillsSocketInfo->Key = Iter.Key;

				SkillsMap.Add(IconPtr->IconSocket, SkillsSocketInfo);
			}
			else
			{
				TSharedPtr<FSkillSocketInfo >SkillsSocketInfo;

				SkillsMap.Add(IconPtr->IconSocket, SkillsSocketInfo);
			}
		}
		EICPtr->RegisterMultiGAs(SkillsMap);
	}
}

void UAllocationSkillsMenu::ResetUIByData_Skills()
{
	{
		auto TileViewPtr = Cast<UTileView>(GetWidgetFromName(AllocationSkillsMenu::Get().WeaponsTileView));
		if (!TileViewPtr)
		{
			return;
		}
		TileViewPtr->SetVisibility(ESlateVisibility::Collapsed);
	}

	auto TileViewPtr = Cast<UTileView>(GetWidgetFromName(AllocationSkillsMenu::Get().SkillsTileView));
	if (!TileViewPtr)
	{
		return;
	}
	TileViewPtr->SetVisibility(ESlateVisibility::Visible);

	TileViewPtr->ClearListItems();
	auto EntryClass = TileViewPtr->GetEntryWidgetClass();
	const auto& SceneUintAryRef = SPHoldItemPerpertyPtr.GetSceneUintAry();
	for (const auto& Iter : SceneUintAryRef)
	{
		if (Iter->GetSceneToolsType() != ESceneToolsType::kSkill)
		{
			continue;
		}

		auto ToolSPtr = Cast<USkillUnit>(Iter);
		if (!ToolSPtr)
		{
			continue;
		}

		switch (ToolSPtr->SkillType)
		{
		case ESkillType::kActive:
		case ESkillType::kPassive:
		case ESkillType::kTalentPassive:
		{
			auto WidgetPtr = CreateWidget<USkillsIcon>(this, EntryClass);
			if (WidgetPtr)
			{
				WidgetPtr->bIsInBackpakc = true;
				WidgetPtr->SkillType = ToolSPtr->SkillType;
				WidgetPtr->ResetToolUIByData(Iter);

				{
					TArray<FName>Ary
					{
						{AllocationSkillsMenu::Get().ActiveSkill1},
						{AllocationSkillsMenu::Get().ActiveSkill2},
						{AllocationSkillsMenu::Get().ActiveSkill3},
						{AllocationSkillsMenu::Get().ActiveSkill4},
						{AllocationSkillsMenu::Get().WeaponActiveSkill1},
						{AllocationSkillsMenu::Get().WeaponActiveSkill2},
						{AllocationSkillsMenu::Get().PassivSkill1},
						{AllocationSkillsMenu::Get().PassivSkill2},
						{AllocationSkillsMenu::Get().PassivSkill3},
						{AllocationSkillsMenu::Get().PassivSkill4},
						{AllocationSkillsMenu::Get().PassivSkill5},
						{AllocationSkillsMenu::Get().TalentPassivSkill},
					};

					for (const auto& IconIter : Ary)
					{
						auto IconPtr = Cast<USkillsIcon>(GetWidgetFromName(IconIter));
						if (IconPtr)
						{
							auto Result = WidgetPtr->OnDragDelegate.AddCallback(
								std::bind(&USkillsIcon::OnDragSkillIcon, IconPtr, std::placeholders::_1, std::placeholders::_2));
							Result->bIsAutoUnregister = false;
						}
					}
				}
				{
					TArray<FName>Ary
					{
						{AllocationSkillsMenu::Get().MainWeapon},
						{AllocationSkillsMenu::Get().SecondaryWeapon},
					};

					for (const auto& IconIter : Ary)
					{
						auto IconPtr = Cast<UWeaponsIcon>(GetWidgetFromName(IconIter));
						if (IconPtr)
						{
							auto Result = WidgetPtr->OnDragDelegate.AddCallback(
								std::bind(&UWeaponsIcon::OnDragSkillIcon, IconPtr, std::placeholders::_1, std::placeholders::_2));
							Result->bIsAutoUnregister = false;
						}
					}
				}

				TileViewPtr->AddItem(WidgetPtr);
			}
		}
		break;
		};
	}
}

void UAllocationSkillsMenu::ResetUIByData_WeaponSkills(EWeaponSocket WeaponSocket)
{
	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!CharacterPtr)
	{
		return;
	}

	auto EICPtr = CharacterPtr->GetInteractiveSkillComponent();

	TSharedPtr<FWeaponSocketInfo > FirstWeaponSocketInfoSPtr;
	TSharedPtr<FWeaponSocketInfo > SecondWeaponSocketInfoSPtr;
	switch (WeaponSocket)
	{
	case EWeaponSocket::kNone:
	case EWeaponSocket::kMain:
	{
		EICPtr->GetWeapon(FirstWeaponSocketInfoSPtr, SecondWeaponSocketInfoSPtr);
	}
	break;
	case EWeaponSocket::kSecondary:
	{
		EICPtr->GetWeapon(SecondWeaponSocketInfoSPtr, FirstWeaponSocketInfoSPtr);
	}
	break;
	}
	{
		{
			auto IconPtr = Cast<UWeaponsIcon>(GetWidgetFromName(AllocationSkillsMenu::Get().MainWeapon));
			if (IconPtr)
			{
				IconPtr->ResetToolUIByData(
					FirstWeaponSocketInfoSPtr ?
					FirstWeaponSocketInfoSPtr->WeaponUnitPtr : 
					nullptr
				);
			}
		}
		auto IconPtr = Cast<USkillsIcon>(GetWidgetFromName(AllocationSkillsMenu::Get().WeaponActiveSkill1));
		if (IconPtr)
		{
			IconPtr->ResetToolUIByData(
				FirstWeaponSocketInfoSPtr && FirstWeaponSocketInfoSPtr->WeaponUnitPtr ? 
				FirstWeaponSocketInfoSPtr->WeaponUnitPtr->FirstSkill : 
				nullptr
			);
		}
	}

	{
		{
			auto IconPtr = Cast<UWeaponsIcon>(GetWidgetFromName(AllocationSkillsMenu::Get().SecondaryWeapon));
			if (IconPtr)
			{
				IconPtr->ResetToolUIByData(
					SecondWeaponSocketInfoSPtr ?
					SecondWeaponSocketInfoSPtr->WeaponUnitPtr : 
					nullptr
				);
			}
		}
		auto IconPtr = Cast<USkillsIcon>(GetWidgetFromName(AllocationSkillsMenu::Get().WeaponActiveSkill2));
		if (IconPtr)
		{
			IconPtr->ResetToolUIByData(
				SecondWeaponSocketInfoSPtr && SecondWeaponSocketInfoSPtr->WeaponUnitPtr ?
				SecondWeaponSocketInfoSPtr->WeaponUnitPtr->FirstSkill :
				nullptr
			);
		}
	}
}

void UAllocationSkillsMenu::ResetUIByData_Weapons()
{
	{
		auto TileViewPtr = Cast<UTileView>(GetWidgetFromName(AllocationSkillsMenu::Get().SkillsTileView));
		if (!TileViewPtr)
		{
			return;
		}
		TileViewPtr->SetVisibility(ESlateVisibility::Collapsed);
	}

	auto TileViewPtr = Cast<UTileView>(GetWidgetFromName(AllocationSkillsMenu::Get().WeaponsTileView));
	if (!TileViewPtr)
	{
		return;
	}
	TileViewPtr->SetVisibility(ESlateVisibility::Visible);

	TileViewPtr->ClearListItems();
	auto EntryClass = TileViewPtr->GetEntryWidgetClass();
	const auto& ItemsAry = SPHoldItemPerpertyPtr.GetSceneUintAry();
	for (const auto& Iter : ItemsAry)
	{
		if (Iter->GetSceneToolsType() == ESceneToolsType::kWeapon)
		{
			auto WidgetPtr = CreateWidget<UWeaponsIcon>(this, EntryClass);
			if (WidgetPtr)
			{
				WidgetPtr->bIsInBackpakc = true;
				WidgetPtr->ResetToolUIByData(Iter);
				{
					TArray<FName>Ary
					{
						{AllocationSkillsMenu::Get().ActiveSkill1},
						{AllocationSkillsMenu::Get().ActiveSkill2},
						{AllocationSkillsMenu::Get().ActiveSkill3},
						{AllocationSkillsMenu::Get().ActiveSkill4},
						{AllocationSkillsMenu::Get().WeaponActiveSkill1},
						{AllocationSkillsMenu::Get().WeaponActiveSkill2},
						{AllocationSkillsMenu::Get().PassivSkill1},
						{AllocationSkillsMenu::Get().PassivSkill2},
						{AllocationSkillsMenu::Get().PassivSkill3},
						{AllocationSkillsMenu::Get().PassivSkill4},
						{AllocationSkillsMenu::Get().PassivSkill5},
						{AllocationSkillsMenu::Get().TalentPassivSkill},
					};
					for (const auto& IconIter : Ary)
					{
						auto IconPtr = Cast<USkillsIcon>(GetWidgetFromName(IconIter));
						if (IconPtr)
						{
							auto Result = WidgetPtr->OnDragDelegate.AddCallback(
								std::bind(&USkillsIcon::OnDragWeaponIcon, IconPtr, std::placeholders::_1, std::placeholders::_2
								));
							Result->bIsAutoUnregister = false;
						}
					}
				}
				{
					TArray<FName>Ary
					{
						{AllocationSkillsMenu::Get().MainWeapon},
						{AllocationSkillsMenu::Get().SecondaryWeapon},
					};

					for (const auto& IconIter : Ary)
					{
						auto IconPtr = Cast<UWeaponsIcon>(GetWidgetFromName(IconIter));
						if (IconPtr)
						{
							auto Result = WidgetPtr->OnDragDelegate.AddCallback(
								std::bind(&UWeaponsIcon::OnDragWeaponIcon, IconPtr, std::placeholders::_1, std::placeholders::_2));
							Result->bIsAutoUnregister = false;
						}
					}
				}

				TileViewPtr->AddItem(WidgetPtr);
			}
		}
	}
}

void UAllocationSkillsMenu::BindEvent()
{
	{
		auto WeaponIconPtr = Cast<UWeaponsIcon>(GetWidgetFromName(AllocationSkillsMenu::Get().MainWeapon));
		if (WeaponIconPtr)
		{
			MainDelegateHandleSPtr = WeaponIconPtr->OnResetUnit.AddCallback(std::bind(&ThisClass::OnMainWeaponChanged, this, std::placeholders::_1));
		}
	}
	{
		auto WeaponIconPtr = Cast<UWeaponsIcon>(GetWidgetFromName(AllocationSkillsMenu::Get().SecondaryWeapon));
		if (WeaponIconPtr)
		{
			SecondaryDelegateHandleSPtr = WeaponIconPtr->OnResetUnit.AddCallback(std::bind(&ThisClass::OnSecondaryWeaponChanged, this, std::placeholders::_1));
		}
	}
	{
		auto BtnPtr = Cast<UButton>(GetWidgetFromName(AllocationSkillsMenu::Get().WeaponBtn));
		if (BtnPtr)
		{
			BtnPtr->OnClicked.AddDynamic(this, &ThisClass::OnWeaponsBtnCliked);
		}
	}
	{
		auto BtnPtr = Cast<UButton>(GetWidgetFromName(AllocationSkillsMenu::Get().SkillBtn));
		if (BtnPtr)
		{
			BtnPtr->OnClicked.AddDynamic(this, &ThisClass::OnSkillsBtnCliked);
		}
	}
	{
		TArray<FName>Ary
		{
			{AllocationSkillsMenu::Get().ActiveSkill1},
			{AllocationSkillsMenu::Get().ActiveSkill2},
			{AllocationSkillsMenu::Get().ActiveSkill3},
			{AllocationSkillsMenu::Get().ActiveSkill4},
			{AllocationSkillsMenu::Get().WeaponActiveSkill1},
			{AllocationSkillsMenu::Get().WeaponActiveSkill2},
			{AllocationSkillsMenu::Get().PassivSkill1},
			{AllocationSkillsMenu::Get().PassivSkill2},
			{AllocationSkillsMenu::Get().PassivSkill3},
			{AllocationSkillsMenu::Get().PassivSkill4},
			{AllocationSkillsMenu::Get().PassivSkill5},
			{AllocationSkillsMenu::Get().TalentPassivSkill},
		};

		for (const auto& FirstIter : Ary)
		{
			for (const auto& SecondIter : Ary)
			{
				if (FirstIter == SecondIter)
				{
					continue;
				}
				auto FirstPtr = Cast<USkillsIcon>(GetWidgetFromName(FirstIter));
				auto SecondPtr = Cast<USkillsIcon>(GetWidgetFromName(SecondIter));
				if (FirstPtr && SecondPtr)
				{
					auto Result = SecondPtr->OnResetUnit.AddCallback(
						std::bind(&USkillsIcon::OnSublingIconReset, FirstPtr, std::placeholders::_1));
					Result->bIsAutoUnregister = false;
				}
			}
		}
	}
	{
		TArray<FName>Ary
		{
			{AllocationSkillsMenu::Get().MainWeapon},
			{AllocationSkillsMenu::Get().SecondaryWeapon},
		};

		for (const auto& FirstIter : Ary)
		{
			for (const auto& SecondIter : Ary)
			{
				if (FirstIter == SecondIter)
				{
					continue;
				}
				auto FirstPtr = Cast<UWeaponsIcon>(GetWidgetFromName(FirstIter));
				auto SecondPtr = Cast<UWeaponsIcon>(GetWidgetFromName(SecondIter));
				if (FirstPtr && SecondPtr)
				{
					auto Result = SecondPtr->OnResetUnit.AddCallback(
						std::bind(&UWeaponsIcon::OnSublingIconReset, FirstPtr, std::placeholders::_1));
					Result->bIsAutoUnregister = false;
				}
			}
		}
	}
}

void UAllocationSkillsMenu::OnWeaponsBtnCliked()
{
	ResetUIByData_Weapons();
}

void UAllocationSkillsMenu::OnSkillsBtnCliked()
{
	ResetUIByData_Skills();
}

void UAllocationSkillsMenu::SetHoldItemProperty(const FSceneToolsContainer& NewSPHoldItemPerperty)
{
	SPHoldItemPerpertyPtr = NewSPHoldItemPerperty;
}

void UAllocationSkillsMenu::ResetUIByData()
{
	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!CharacterPtr)
	{
		return;
	}

	auto EICPtr = CharacterPtr->GetInteractiveSkillComponent();

	ResetUIByData_WeaponSkills(EICPtr->GetActivedWeaponType());

	{
		TArray<FName>Ary
		{
			AllocationSkillsMenu::Get().ActiveSkill1,
			AllocationSkillsMenu::Get().ActiveSkill2,
			AllocationSkillsMenu::Get().ActiveSkill3,
			AllocationSkillsMenu::Get().ActiveSkill4 ,
			AllocationSkillsMenu::Get().PassivSkill1,
			AllocationSkillsMenu::Get().PassivSkill2,
			AllocationSkillsMenu::Get().PassivSkill3,
			AllocationSkillsMenu::Get().PassivSkill4,
			AllocationSkillsMenu::Get().PassivSkill5,
			AllocationSkillsMenu::Get().TalentPassivSkill,
		};

		for (const auto& Iter : Ary)
		{
			auto IconPtr = Cast<USkillsIcon>(GetWidgetFromName(Iter));
			if (IconPtr)
			{
				auto Result = EICPtr->FindSkill(IconPtr->IconSocket);
				IconPtr->ResetToolUIByData(Result ? Result->SkillUnit : nullptr);
			}
		}
	}

	ResetUIByData_Skills();
}

void UAllocationSkillsMenu::OnMainWeaponChanged(UWeaponUnit* ToolSPtr)
{
	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	{
		auto IconPtr = Cast<USkillsIcon>(GetWidgetFromName(AllocationSkillsMenu::Get().WeaponActiveSkill1));
		if (IconPtr)
		{
			IconPtr->ResetToolUIByData(ToolSPtr ? ToolSPtr->FirstSkill : nullptr);
		}
	}
}

void UAllocationSkillsMenu::OnSecondaryWeaponChanged(UWeaponUnit* ToolSPtr)
{
	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	{
		auto IconPtr = Cast<USkillsIcon>(GetWidgetFromName(AllocationSkillsMenu::Get().WeaponActiveSkill2));
		if (IconPtr)
		{
			if (ToolSPtr && !ToolSPtr->FirstSkill)
			{
				ToolSPtr->FirstSkill = CharacterPtr->GetHoldingItemsComponent()->GetHoldItemProperty().AddUnit(ToolSPtr->FirstSkillClass);
			}

			IconPtr->ResetToolUIByData(ToolSPtr ? ToolSPtr->FirstSkill : nullptr);
		}
	}
}
