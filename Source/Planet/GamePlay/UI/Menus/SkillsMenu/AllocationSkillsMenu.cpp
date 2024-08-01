
#include "AllocationSkillsMenu.h"

#include <Kismet/GameplayStatics.h>
#include "Components/Button.h"
#include <Components/HorizontalBox.h>
#include "Components/TileView.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/Border.h"
#include <Components/ScrollBar.h>
#include <Components/ScrollBox.h>

#include "BackpackIcon.h"
#include "StateTagExtendInfo.h"
#include "AssetRefMap.h"
#include "SkillsIcon.h"
#include "WeaponsIcon.h"
#include "CharacterBase.h"
#include "InteractiveSkillComponent.h"
#include "UICommon.h"
#include "GameplayTagsSubSystem.h"
#include "BackpackMenu.h"
#include "PlanetPlayerState.h"
#include "ConsumableIcon.h"
#include "GroupmateIcon.h"
#include "SceneUnitContainer.h"

struct FAllocationSkillsMenu : public TGetSocketName<FAllocationSkillsMenu>
{
	const FName PlayerBackpack = TEXT("PlayerBackpack");

	const FName TargetBackpack = TEXT("TargetBackpack");

	const FName GroupmateList = TEXT("GroupmateList");

	const FName MainWeapon = TEXT("MainWeapon");

	const FName SecondaryWeapon = TEXT("SecondaryWeapon");

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

	const FName Consumable1 = TEXT("Consumable1");

	const FName Consumable2 = TEXT("Consumable2");

	const FName Consumable3 = TEXT("Consumable3");

	const FName Consumable4 = TEXT("Consumable4");
};

UAllocationSkillsMenu::UAllocationSkillsMenu(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	ActiveSkills_1_Key = EKeys::Q;
	ActiveSkills_2_Key = EKeys::E;
	ActiveSkills_3_Key = EKeys::R;
	ActiveSkills_4_Key = EKeys::F;
	WeaponActiveSkills_Key = EKeys::LeftMouseButton;
}

void UAllocationSkillsMenu::PostCDOContruct()
{
	Super::PostCDOContruct();
}

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
			auto IconPtr = Cast<UWeaponsIcon>(GetWidgetFromName(FAllocationSkillsMenu::Get().MainWeapon));
			if (IconPtr && IconPtr->UnitPtr)
			{
				FirstWeaponSocketInfoSPtr->WeaponSocket = IconPtr->IconSocket;
				FirstWeaponSocketInfoSPtr->WeaponUnitPtr = IconPtr->UnitPtr;
			}
		}
		TSharedPtr<FWeaponSocketInfo > SecondWeaponSocketInfoSPtr = MakeShared<FWeaponSocketInfo>();
		{
			auto IconPtr = Cast<UWeaponsIcon>(GetWidgetFromName(FAllocationSkillsMenu::Get().SecondaryWeapon));
			if (IconPtr && IconPtr->UnitPtr)
			{
				SecondWeaponSocketInfoSPtr->WeaponSocket = IconPtr->IconSocket;
				SecondWeaponSocketInfoSPtr->WeaponUnitPtr = IconPtr->UnitPtr;
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
			{FAllocationSkillsMenu::Get().ActiveSkill1,ActiveSkills_1_Key},
			{FAllocationSkillsMenu::Get().ActiveSkill2,ActiveSkills_2_Key},
			{FAllocationSkillsMenu::Get().ActiveSkill3,ActiveSkills_3_Key},
			{FAllocationSkillsMenu::Get().ActiveSkill4,ActiveSkills_4_Key},
			{FAllocationSkillsMenu::Get().PassivSkill1,EKeys::Invalid},
			{FAllocationSkillsMenu::Get().PassivSkill2,EKeys::Invalid},
			{FAllocationSkillsMenu::Get().PassivSkill3,EKeys::Invalid},
			{FAllocationSkillsMenu::Get().PassivSkill4,EKeys::Invalid},
			{FAllocationSkillsMenu::Get().PassivSkill5,EKeys::Invalid},
//			{FAllocationSkillsMenu::Get().TalentPassivSkill,EKeys::Invalid},
		};

		TMap<FGameplayTag, TSharedPtr<FSkillSocketInfo>> SkillsMap;

		for (auto Iter : Ary)
		{
			auto IconPtr = Cast<USkillsIcon>(GetWidgetFromName(Iter.Name));
			if (IconPtr)
			{
				if (IconPtr->UnitPtr)
				{
					TSharedPtr<FSkillSocketInfo >SkillsSocketInfo = MakeShared<FSkillSocketInfo>();

					SkillsSocketInfo->SkillSocket = IconPtr->IconSocket;
					SkillsSocketInfo->SkillUnit = IconPtr->UnitPtr;
					SkillsSocketInfo->Key = Iter.Key;

					SkillsMap.Add(IconPtr->IconSocket, SkillsSocketInfo);
				}
				else
				{
					TSharedPtr<FSkillSocketInfo >SkillsSocketInfo;

					SkillsMap.Add(IconPtr->IconSocket, SkillsSocketInfo);
				}
			}
		}
		EICPtr->RegisterMultiGAs(SkillsMap);
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
			auto IconPtr = Cast<UWeaponsIcon>(GetWidgetFromName(FAllocationSkillsMenu::Get().MainWeapon));
			if (IconPtr)
			{
				IconPtr->ResetToolUIByData(
					FirstWeaponSocketInfoSPtr ?
					FirstWeaponSocketInfoSPtr->WeaponUnitPtr : 
					nullptr
				);
			}
		}
		auto IconPtr = Cast<USkillsIcon>(GetWidgetFromName(FAllocationSkillsMenu::Get().WeaponActiveSkill1));
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
			auto IconPtr = Cast<UWeaponsIcon>(GetWidgetFromName(FAllocationSkillsMenu::Get().SecondaryWeapon));
			if (IconPtr)
			{
				IconPtr->ResetToolUIByData(
					SecondWeaponSocketInfoSPtr ?
					SecondWeaponSocketInfoSPtr->WeaponUnitPtr : 
					nullptr
				);
			}
		}
		auto IconPtr = Cast<USkillsIcon>(GetWidgetFromName(FAllocationSkillsMenu::Get().WeaponActiveSkill2));
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

void UAllocationSkillsMenu::BindEvent()
{
	{
		auto WeaponIconPtr = Cast<UWeaponsIcon>(GetWidgetFromName(FAllocationSkillsMenu::Get().MainWeapon));
		if (WeaponIconPtr)
		{
			MainDelegateHandleSPtr = WeaponIconPtr->OnResetUnit_Weapon.AddCallback(
				std::bind(&ThisClass::OnMainWeaponChanged, this, std::placeholders::_1)
			);
		}
	}
	{
		auto WeaponIconPtr = Cast<UWeaponsIcon>(GetWidgetFromName(FAllocationSkillsMenu::Get().SecondaryWeapon));
		if (WeaponIconPtr)
		{
			SecondaryDelegateHandleSPtr = WeaponIconPtr->OnResetUnit_Weapon.AddCallback(
				std::bind(&ThisClass::OnSecondaryWeaponChanged, this, std::placeholders::_1)
			);
		}
	}
	{
		TArray<FName>Ary
		{
			{FAllocationSkillsMenu::Get().ActiveSkill1},
			{FAllocationSkillsMenu::Get().ActiveSkill2},
			{FAllocationSkillsMenu::Get().ActiveSkill3},
			{FAllocationSkillsMenu::Get().ActiveSkill4},
			{FAllocationSkillsMenu::Get().WeaponActiveSkill1},
			{FAllocationSkillsMenu::Get().WeaponActiveSkill2},
			{FAllocationSkillsMenu::Get().PassivSkill1},
			{FAllocationSkillsMenu::Get().PassivSkill2},
			{FAllocationSkillsMenu::Get().PassivSkill3},
			{FAllocationSkillsMenu::Get().PassivSkill4},
			{FAllocationSkillsMenu::Get().PassivSkill5},
			{FAllocationSkillsMenu::Get().TalentPassivSkill},
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
			{FAllocationSkillsMenu::Get().Consumable1},
			{FAllocationSkillsMenu::Get().Consumable2},
			{FAllocationSkillsMenu::Get().Consumable3},
			{FAllocationSkillsMenu::Get().Consumable4},
		};

		for (const auto& FirstIter : Ary)
		{
			for (const auto& SecondIter : Ary)
			{
				if (FirstIter == SecondIter)
				{
					continue;
				}
				auto FirstPtr = Cast<UConsumableIcon>(GetWidgetFromName(FirstIter));
				auto SecondPtr = Cast<UConsumableIcon>(GetWidgetFromName(SecondIter));
				if (FirstPtr && SecondPtr)
				{
					auto Result = SecondPtr->OnResetUnit.AddCallback(
						std::bind(&UConsumableIcon::OnSublingIconReset, FirstPtr, std::placeholders::_1));
					Result->bIsAutoUnregister = false;
				}
			}
		}
	}
	{
		TArray<FName>Ary
		{
			{FAllocationSkillsMenu::Get().MainWeapon},
			{FAllocationSkillsMenu::Get().SecondaryWeapon},
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
	{
		auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
		if (!CharacterPtr)
		{
			return;
		}

		auto UIPtr = Cast<UBackpackMenu>(GetWidgetFromName(FAllocationSkillsMenu::Get().PlayerBackpack));

 		UIPtr->SetHoldItemProperty(
 			CharacterPtr->GetHoldingItemsComponent()->GetSceneUnitContainer()
 		);
		UIPtr->ResetUIByData();

		{
			auto Delegate = 
				UIPtr->OnDragIconDelegate.AddCallback(std::bind(&ThisClass::OnDragIcon, this, std::placeholders::_1, std::placeholders::_2));
			Delegate->bIsAutoUnregister = false;
		}
		
		UIPtr->ResetUIByData();
	}
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
			FAllocationSkillsMenu::Get().ActiveSkill1,
			FAllocationSkillsMenu::Get().ActiveSkill2,
			FAllocationSkillsMenu::Get().ActiveSkill3,
			FAllocationSkillsMenu::Get().ActiveSkill4 ,
			FAllocationSkillsMenu::Get().PassivSkill1,
			FAllocationSkillsMenu::Get().PassivSkill2,
			FAllocationSkillsMenu::Get().PassivSkill3,
			FAllocationSkillsMenu::Get().PassivSkill4,
			FAllocationSkillsMenu::Get().PassivSkill5,
			FAllocationSkillsMenu::Get().TalentPassivSkill,
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

	InitialGroupmateList();
}

void UAllocationSkillsMenu::InitialGroupmateList()
{
	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	{
		auto UIPtr = Cast<UScrollBox>(GetWidgetFromName(FAllocationSkillsMenu::Get().GroupmateList));
		if (!UIPtr)
		{
			return;
		}
		UIPtr->ClearChildren();

		auto SceneUnitContainerRef = CharacterPtr->GetHoldingItemsComponent()->GetSceneUnitContainer();
		
		auto GroupmateUnitAry = SceneUnitContainerRef->GetGourpmateUintAry();
		for (auto Iter : GroupmateUnitAry)
		{
			auto WidgetPtr = CreateWidget<UGroupmateIcon>(this, GroupmateIconClass);
			if (WidgetPtr)
			{
				WidgetPtr->ResetToolUIByData(Iter);
				{
					auto Handle = WidgetPtr->OnSelected.AddCallback(std::bind(&ThisClass::OnSelectedGourpmate, this, std::placeholders::_1)); 
					Handle->bIsAutoUnregister = false;
				}
				UIPtr->AddChild(WidgetPtr);
			}
		}
	}
}

void UAllocationSkillsMenu::OnMainWeaponChanged(UWeaponUnit* ToolSPtr)
{
	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	{
		auto IconPtr = Cast<USkillsIcon>(GetWidgetFromName(FAllocationSkillsMenu::Get().WeaponActiveSkill1));
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
		auto IconPtr = Cast<USkillsIcon>(GetWidgetFromName(FAllocationSkillsMenu::Get().WeaponActiveSkill2));
		if (IconPtr)
		{
			if (ToolSPtr && !ToolSPtr->FirstSkill)
			{
			}

			IconPtr->ResetToolUIByData(ToolSPtr ? ToolSPtr->FirstSkill : nullptr);
		}
	}
}

void UAllocationSkillsMenu::OnDragIcon(bool bIsDragging, UBasicUnit* UnitPtr)
{
	{
		TArray<FName>Ary
		{
			{FAllocationSkillsMenu::Get().ActiveSkill1},
			{FAllocationSkillsMenu::Get().ActiveSkill2},
			{FAllocationSkillsMenu::Get().ActiveSkill3},
			{FAllocationSkillsMenu::Get().ActiveSkill4},
			{FAllocationSkillsMenu::Get().WeaponActiveSkill1},
			{FAllocationSkillsMenu::Get().WeaponActiveSkill2},
			{FAllocationSkillsMenu::Get().PassivSkill1},
			{FAllocationSkillsMenu::Get().PassivSkill2},
			{FAllocationSkillsMenu::Get().PassivSkill3},
			{FAllocationSkillsMenu::Get().PassivSkill4},
			{FAllocationSkillsMenu::Get().PassivSkill5},
			{FAllocationSkillsMenu::Get().TalentPassivSkill},
			{FAllocationSkillsMenu::Get().TalentPassivSkill},
		};

		for (const auto& IconIter : Ary)
		{
			auto IconPtr = Cast<USkillsIcon>(GetWidgetFromName(IconIter));
			if (IconPtr)
			{
				IconPtr->OnDragIcon(bIsDragging, UnitPtr);
			}
		}
	}
	{
		TArray<FName>Ary
		{
			{FAllocationSkillsMenu::Get().Consumable1},
			{FAllocationSkillsMenu::Get().Consumable2},
			{FAllocationSkillsMenu::Get().Consumable3},
			{FAllocationSkillsMenu::Get().Consumable4},
		};

		for (const auto& IconIter : Ary)
		{
			auto IconPtr = Cast<UConsumableIcon>(GetWidgetFromName(IconIter));
			if (IconPtr)
			{
				IconPtr->OnDragIcon(bIsDragging, UnitPtr);
			}
		}
	}
	{
		TArray<FName>Ary
		{
			{FAllocationSkillsMenu::Get().MainWeapon},
			{FAllocationSkillsMenu::Get().SecondaryWeapon},
		};

		for (const auto& IconIter : Ary)
		{
			auto IconPtr = Cast<UWeaponsIcon>(GetWidgetFromName(IconIter));
			if (IconPtr)
			{
				IconPtr->OnDragIcon(bIsDragging, UnitPtr);
			}
		}
	}
}

void UAllocationSkillsMenu::OnSelectedGourpmate(UCharacterUnit* UnitPtr)
{
	auto UIPtr = Cast<UScrollBox>(GetWidgetFromName(FAllocationSkillsMenu::Get().GroupmateList));
	if (!UIPtr)
	{
		return;
	}
	auto ChildrensAry = UIPtr->GetAllChildren();
	for (auto Iter : ChildrensAry)
	{
		auto GroupmateIconPtr = Cast<UGroupmateIcon>(Iter);
		if (GroupmateIconPtr)
		{
			if (GroupmateIconPtr->UnitPtr == UnitPtr)
			{
				continue;
			}
			else
			{
				GroupmateIconPtr->SwitchSelectState(false);
			}
		}
	}
}
