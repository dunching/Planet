
#include "AllocationSkillsMenu.h"

#include <Kismet/GameplayStatics.h>
#include "Components/Button.h"

#include "BackpackIcon.h"
#include "Components/TileView.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/Border.h"
#include <AssetRefrencePath.h>
#include "AssetRefMap.h"
#include "SkillsIcon.h"
#include "WeaponsIcon.h"
#include "CharacterBase.h"
#include "EquipmentElementComponent.h"

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

void UAllocationSkillsMenu::NativeConstruct()
{
	Super::NativeConstruct();

	BindEvent();

	ResetUIByData();
}

void UAllocationSkillsMenu::NativeDestruct()
{
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

	auto EICPtr = CharacterPtr->GetEquipmentItemsComponent();

	{
		auto IconPtr = Cast<UWeaponsIcon>(GetWidgetFromName(MainWeapon));
		if (IconPtr)
		{
			EICPtr->SetMainWeapon(IconPtr->WeaponUnitPtr);
		}
	}
	{
		auto IconPtr = Cast<UWeaponsIcon>(GetWidgetFromName(SecondaryWeapon));
		if (IconPtr)
		{
			EICPtr->SetSecondaryWeapon(IconPtr->WeaponUnitPtr);
		}
	}
	struct FHelper
	{
		FName Name;
		FKey Key;
	};

	TArray<FHelper>Ary
	{
		{WeaponActiveSkill1,WeaponActiveSkills_1},
		{WeaponActiveSkill2,WeaponActiveSkills_2},
		{ActiveSkill1,ActiveSkills_1},
		{ActiveSkill2,ActiveSkills_2},
		{ActiveSkill3,ActiveSkills_3},
		{ActiveSkill4,ActiveSkills_4},
		{PassivSkill1,EKeys::Invalid},
		{PassivSkill2,EKeys::Invalid},
		{PassivSkill3,EKeys::Invalid},
		{PassivSkill4,EKeys::Invalid},
		{PassivSkill5,EKeys::Invalid},
		{TalentPassivSkill,EKeys::Invalid},
	};

	TMap<FGameplayTag, FSkillsSocketInfo> SkillsMap;
	for (auto Iter : Ary)
	{
		auto IconPtr = Cast<USkillsIcon>(GetWidgetFromName(Iter.Name));
		if (IconPtr && IconPtr->SkillUnitPtr)
		{
			FSkillsSocketInfo SkillsSocketInfo;

			SkillsSocketInfo.SkillSocket = IconPtr->IconSocket;
			SkillsSocketInfo.SkillUnit = IconPtr->SkillUnitPtr;
			SkillsSocketInfo.Key = Iter.Key;

			SkillsMap.Add(IconPtr->IconSocket, SkillsSocketInfo);
		}
	}
	EICPtr->RegisterMultiGAs(SkillsMap);

	Super::NativeDestruct();
}

void UAllocationSkillsMenu::ResetUIByData_Skills()
{
	{
		auto TileViewPtr = Cast<UTileView>(GetWidgetFromName(WeaponsTileView));
		if (!TileViewPtr)
		{
			return;
		}
		TileViewPtr->SetVisibility(ESlateVisibility::Collapsed);
	}

	auto TileViewPtr = Cast<UTileView>(GetWidgetFromName(SkillsTileView));
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
				TileViewPtr->AddItem(WidgetPtr); 
				WidgetPtr->bIsInBackpakc = true;
				WidgetPtr->ResetToolUIByData(Iter);
			}
		}
		break;
		};
	}
}

void UAllocationSkillsMenu::ResetUIByData_Weapons()
{
	{
		auto TileViewPtr = Cast<UTileView>(GetWidgetFromName(SkillsTileView));
		if (!TileViewPtr)
		{
			return;
		}
		TileViewPtr->SetVisibility(ESlateVisibility::Collapsed);
	}

	auto TileViewPtr = Cast<UTileView>(GetWidgetFromName(WeaponsTileView));
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
				TileViewPtr->AddItem(WidgetPtr);
				WidgetPtr->bIsInBackpakc = true;
				WidgetPtr->ResetToolUIByData(Iter);
			}
		}
	}
}

void UAllocationSkillsMenu::BindEvent()
{
	{
		auto WeaponIconPtr = Cast<UWeaponsIcon>(GetWidgetFromName(MainWeapon));
		if (WeaponIconPtr)
		{
			MainDelegateHandleSPtr = WeaponIconPtr->OnDroped.AddCallback(std::bind(&ThisClass::OnMainWeaponChanged, this, std::placeholders::_1));
		}
	}
	{
		auto WeaponIconPtr = Cast<UWeaponsIcon>(GetWidgetFromName(SecondaryWeapon));
		if (WeaponIconPtr)
		{
			SecondaryDelegateHandleSPtr = WeaponIconPtr->OnDroped.AddCallback(std::bind(&ThisClass::OnSecondaryWeaponChanged, this, std::placeholders::_1));
		}
	}
	{
		auto BtnPtr = Cast<UButton>(GetWidgetFromName(WeaponBtn));
		if (BtnPtr)
		{
			BtnPtr->OnClicked.AddDynamic(this, &ThisClass::OnWeaponsBtnCliked);
		}
	}
	{
		auto BtnPtr = Cast<UButton>(GetWidgetFromName(SkillBtn));
		if (BtnPtr)
		{
			BtnPtr->OnClicked.AddDynamic(this, &ThisClass::OnSkillsBtnCliked);
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

	auto EICPtr = CharacterPtr->GetEquipmentItemsComponent();

	{
		auto IconPtr = Cast<UWeaponsIcon>(GetWidgetFromName(MainWeapon));
		if (IconPtr)
		{
			IconPtr->ResetToolUIByData(CharacterPtr->GetEquipmentItemsComponent()->GetMainWeaponUnit());
		}
	}

	{
		auto IconPtr = Cast<UWeaponsIcon>(GetWidgetFromName(SecondaryWeapon));
		if (IconPtr)
		{
			IconPtr->ResetToolUIByData(CharacterPtr->GetEquipmentItemsComponent()->GetSecondaryWeaponUnit());
		}
	}

	{
		TArray<FName>Ary
		{
			ActiveSkill1,
			ActiveSkill2,
			ActiveSkill3,
			ActiveSkill4 ,
			WeaponActiveSkill1,
			WeaponActiveSkill2,
			PassivSkill1,
			PassivSkill2,
			PassivSkill3,
			PassivSkill4,
			PassivSkill5,
			TalentPassivSkill,
		};

		for (const auto& Iter : Ary)
		{
			auto IconPtr = Cast<USkillsIcon>(GetWidgetFromName(Iter));
			if (IconPtr)
			{
				auto Result = EICPtr->FindSkill(IconPtr->IconSocket);
				IconPtr->ResetToolUIByData(Result.SkillUnit);
			}
		}
	}

	ResetUIByData_Skills();
}

void UAllocationSkillsMenu::OnMainWeaponChanged(UWeaponUnit* ToolSPtr)
{
	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	{
		auto IconPtr = Cast<USkillsIcon>(GetWidgetFromName(WeaponActiveSkill1));
		if (IconPtr)
		{
			if (!ToolSPtr->FirstSkill)
			{
				ToolSPtr->FirstSkill = CharacterPtr->GetHoldingItemsComponent()->GetHoldItemProperty().AddUnit(ToolSPtr->FirstSkillClass);
			}

			IconPtr->ResetToolUIByData(ToolSPtr->FirstSkill);
		}
	}
}

void UAllocationSkillsMenu::OnSecondaryWeaponChanged(UWeaponUnit* ToolSPtr)
{
	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	{
		auto IconPtr = Cast<USkillsIcon>(GetWidgetFromName(WeaponActiveSkill2));
		if (IconPtr)
		{
			if (!ToolSPtr->FirstSkill)
			{
				ToolSPtr->FirstSkill = CharacterPtr->GetHoldingItemsComponent()->GetHoldItemProperty().AddUnit(ToolSPtr->FirstSkillClass);
			}

			IconPtr->ResetToolUIByData(ToolSPtr->FirstSkill);
		}
	}
}