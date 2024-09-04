
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
#include "GroupMnaggerComponent.h"
#include "InteractiveConsumablesComponent.h"

struct FAllocationSkillsMenu : public TStructVariable<FAllocationSkillsMenu>
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

UAllocationSkillsMenu::UAllocationSkillsMenu(const FObjectInitializer& ObjectInitializer) :
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

	ResetUIByData();

	BindEvent();
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

	SyncAllocation2Character();
}

void UAllocationSkillsMenu::ResetUIByData_WeaponSkills(UCharacterUnit* PlayerCharacterUnitPtr)
{
	auto CharacterPtr = PlayerCharacterUnitPtr->ProxyCharacterPtr;
	if (!CharacterPtr)
	{
		return;
	}

	auto EICPtr = CharacterPtr->GetInteractiveSkillComponent();

	TSharedPtr<FWeaponSocketInfo > FirstWeaponSocketInfoSPtr;
	TSharedPtr<FWeaponSocketInfo > SecondWeaponSocketInfoSPtr;

	EICPtr->GetWeapon(FirstWeaponSocketInfoSPtr, SecondWeaponSocketInfoSPtr);

	{
		{
			auto IconPtr = Cast<UWeaponsIcon>(GetWidgetFromName(FAllocationSkillsMenu::Get().MainWeapon));
			if (IconPtr)
			{
				IconPtr->bPaseInvokeOnResetUnitEvent = true;
				IconPtr->ResetToolUIByData(
					FirstWeaponSocketInfoSPtr ?
					FirstWeaponSocketInfoSPtr->WeaponUnitPtr :
					nullptr
				);
				IconPtr->bPaseInvokeOnResetUnitEvent = false;
			}
		}
		auto IconPtr = Cast<USkillsIcon>(GetWidgetFromName(FAllocationSkillsMenu::Get().WeaponActiveSkill1));
		if (IconPtr)
		{
			IconPtr->bPaseInvokeOnResetUnitEvent = true;
			IconPtr->ResetToolUIByData(
				FirstWeaponSocketInfoSPtr && FirstWeaponSocketInfoSPtr->WeaponUnitPtr ?
				FirstWeaponSocketInfoSPtr->WeaponUnitPtr->FirstSkill :
				nullptr
			);
			IconPtr->bPaseInvokeOnResetUnitEvent = false;
		}
	}

	{
		{
			auto IconPtr = Cast<UWeaponsIcon>(GetWidgetFromName(FAllocationSkillsMenu::Get().SecondaryWeapon));
			if (IconPtr)
			{
				IconPtr->bPaseInvokeOnResetUnitEvent = true;
				IconPtr->ResetToolUIByData(
					SecondWeaponSocketInfoSPtr ?
					SecondWeaponSocketInfoSPtr->WeaponUnitPtr :
					nullptr
				);
				IconPtr->bPaseInvokeOnResetUnitEvent = false;
			}
		}
		auto IconPtr = Cast<USkillsIcon>(GetWidgetFromName(FAllocationSkillsMenu::Get().WeaponActiveSkill2));
		if (IconPtr)
		{
			IconPtr->bPaseInvokeOnResetUnitEvent = true;
			IconPtr->ResetToolUIByData(
				SecondWeaponSocketInfoSPtr && SecondWeaponSocketInfoSPtr->WeaponUnitPtr ?
				SecondWeaponSocketInfoSPtr->WeaponUnitPtr->FirstSkill :
				nullptr
			);
			IconPtr->bPaseInvokeOnResetUnitEvent = false;
		}
	}
}

void UAllocationSkillsMenu::ResetUIByData_Skills(UCharacterUnit* PlayerCharacterUnitPtr)
{
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

		auto EICPtr = PlayerCharacterUnitPtr->ProxyCharacterPtr->GetInteractiveSkillComponent();
		for (const auto& Iter : Ary)
		{
			auto IconPtr = Cast<USkillsIcon>(GetWidgetFromName(Iter));
			if (IconPtr)
			{
				auto Result = EICPtr->FindSkill(IconPtr->IconSocket);

				IconPtr->bPaseInvokeOnResetUnitEvent = true;
				IconPtr->ResetToolUIByData(Result ? Result->SkillUnitPtr : nullptr);
				IconPtr->bPaseInvokeOnResetUnitEvent = false;
			}
		}
	}
}

void UAllocationSkillsMenu::ResetUIByData_Consumable(UCharacterUnit* PlayerCharacterUnitPtr)
{
	TArray<FName>Ary
	{
		FAllocationSkillsMenu::Get().Consumable1, 
		FAllocationSkillsMenu::Get().Consumable2, 
		FAllocationSkillsMenu::Get().Consumable3, 
		FAllocationSkillsMenu::Get().Consumable4, 
	};

	auto EICPtr = PlayerCharacterUnitPtr->ProxyCharacterPtr->GetInteractiveConsumablesComponent();
	for (const auto& Iter : Ary)
	{
		auto IconPtr = Cast<UConsumableIcon>(GetWidgetFromName(Iter));
		if (IconPtr)
		{
			auto Result = EICPtr->FindConsumable(IconPtr->IconSocket);

			IconPtr->bPaseInvokeOnResetUnitEvent = true;
			IconPtr->ResetToolUIByData(Result ? Result->UnitPtr : nullptr);
			IconPtr->bPaseInvokeOnResetUnitEvent = false;
		}
	}
}

void UAllocationSkillsMenu::SyncAllocation2Character()
{
	if (!CurrentUnitPtr)
	{
		return;
	}

	auto CharacterPtr = CurrentUnitPtr->ProxyCharacterPtr;
	if (!CharacterPtr)
	{
		return;
	}

	// 武器
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
		auto EICPtr = CharacterPtr->GetInteractiveSkillComponent();
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
				if (IconPtr->BasicUnitPtr)
				{
					TSharedPtr<FSkillSocketInfo >SkillsSocketInfo = MakeShared<FSkillSocketInfo>();

					SkillsSocketInfo->SkillSocket = IconPtr->IconSocket;
					SkillsSocketInfo->SkillUnitPtr = Cast<USkillUnit>(IconPtr->BasicUnitPtr);
					SkillsSocketInfo->Key = Iter.Key;

					SkillsMap.Add(IconPtr->IconSocket, SkillsSocketInfo);
				}
			}
		}
		auto EICPtr = CharacterPtr->GetInteractiveSkillComponent();
		EICPtr->RegisterMultiGAs(SkillsMap);
	}

	// 消耗品
	{
		struct FHelper
		{
			FName Name;
			FKey Key;
		};

		TArray<FHelper>Ary
		{
			{FAllocationSkillsMenu::Get().Consumable1,Consumable_1_Key},
			{FAllocationSkillsMenu::Get().Consumable2,Consumable_2_Key},
			{FAllocationSkillsMenu::Get().Consumable3,Consumable_3_Key},
			{FAllocationSkillsMenu::Get().Consumable4,Consumable_4_Key},
		};

		TMap<FGameplayTag, TSharedPtr<FConsumableSocketInfo>> SkillsMap;

		for (auto Iter : Ary)
		{
			auto IconPtr = Cast<UConsumableIcon>(GetWidgetFromName(Iter.Name));
			if (IconPtr)
			{
				if (IconPtr->BasicUnitPtr)
				{
					TSharedPtr<FConsumableSocketInfo >SkillsSocketInfo = MakeShared<FConsumableSocketInfo>();

					SkillsSocketInfo->SkillSocket = IconPtr->IconSocket;
					SkillsSocketInfo->UnitPtr = Cast<UConsumableUnit>(IconPtr->BasicUnitPtr);
					SkillsSocketInfo->Key = Iter.Key;

					SkillsMap.Add(IconPtr->IconSocket, SkillsSocketInfo);
				}
			}
		}
		auto EICPtr = CharacterPtr->GetInteractiveConsumablesComponent();
		EICPtr->RegisterConsumable(SkillsMap);
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

		for (const auto& Iter : Ary)
		{
			auto UIPtr = Cast<USkillsIcon>(GetWidgetFromName(Iter));
			if (UIPtr)
			{
				auto Result = UIPtr->OnResetUnit.AddCallback(
					std::bind(&ThisClass::OnSkillUnitChanged, this, std::placeholders::_1, std::placeholders::_2
					)
				);
				Result->bIsAutoUnregister = false;
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

		for (const auto& Iter : Ary)
		{
			auto UIPtr = Cast<UConsumableIcon>(GetWidgetFromName(Iter));
			if (UIPtr)
			{
				auto Result = UIPtr->OnResetUnit.AddCallback(
					std::bind(&ThisClass::OnConsumableUnitChanged, this, std::placeholders::_1, std::placeholders::_2
					)
				);
				Result->bIsAutoUnregister = false;
			}
		}
	}
	{
		TArray<FName>Ary
		{
			{FAllocationSkillsMenu::Get().MainWeapon},
			{FAllocationSkillsMenu::Get().SecondaryWeapon},
		};

		for (const auto& Iter : Ary)
		{
			auto UIPtr = Cast<UWeaponsIcon>(GetWidgetFromName(Iter));
			if (UIPtr)
			{
				auto Result = UIPtr->OnResetUnit.AddCallback(
					std::bind(&ThisClass::OnWeaponUnitChanged, this, std::placeholders::_1, std::placeholders::_2
					)
				);
				Result->bIsAutoUnregister = false;
			}
		}
	}
}

void UAllocationSkillsMenu::ResetUI(UCharacterUnit* TargetCharacterUnitPtr, UCharacterUnit* PlayerCharacterUnitPtr)
{
	CurrentUnitPtr = TargetCharacterUnitPtr;
	ResetUIByData_WeaponSkills(CurrentUnitPtr);
	if (TargetCharacterUnitPtr == PlayerCharacterUnitPtr)
	{
		ResetBackpack(nullptr, PlayerCharacterUnitPtr);
	}
	else
	{
		ResetBackpack(TargetCharacterUnitPtr, PlayerCharacterUnitPtr);
	}
	ResetUIByData_Skills(CurrentUnitPtr);
	ResetUIByData_Consumable(CurrentUnitPtr);
}

void UAllocationSkillsMenu::ResetUIByData()
{
	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!CharacterPtr)
	{
		return;
	}

	InitialGroupmateList();

	ResetUI(CharacterPtr->GetCharacterUnit(), CharacterPtr->GetCharacterUnit());
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
		{
			auto WidgetPtr = CreateWidget<UGroupmateIcon>(this, GroupmateIconClass);
			if (WidgetPtr)
			{
				WidgetPtr->ResetToolUIByData(CharacterPtr->GetCharacterUnit());
				{
					auto Handle = WidgetPtr->OnSelected.AddCallback(std::bind(&ThisClass::OnSelectedCharacterUnit, this, std::placeholders::_1));
					Handle->bIsAutoUnregister = false;
				}
				UIPtr->AddChild(WidgetPtr);

				WidgetPtr->SwitchSelectState(true);
			}
		}
		auto GroupHelperSPtr = CharacterPtr->GetGroupMnaggerComponent()->GetGroupHelper();

		auto GroupmateUnitAry = GroupHelperSPtr->MembersSet;

		for (auto Iter : GroupmateUnitAry)
		{
			auto WidgetPtr = CreateWidget<UGroupmateIcon>(this, GroupmateIconClass);
			if (WidgetPtr)
			{
				WidgetPtr->ResetToolUIByData(Iter);
				{
					auto Handle = WidgetPtr->OnSelected.AddCallback(std::bind(&ThisClass::OnSelectedCharacterUnit, this, std::placeholders::_1));
					Handle->bIsAutoUnregister = false;
				}
				UIPtr->AddChild(WidgetPtr);
			}
		}
	}
}

void UAllocationSkillsMenu::ResetBackpack(UCharacterUnit* AICharacterUnitPtr, UCharacterUnit* PlayerCharacterUnitPtr)
{
	if (AICharacterUnitPtr)
	{
		auto UIPtr = Cast<UBackpackMenu>(GetWidgetFromName(FAllocationSkillsMenu::Get().TargetBackpack));

		UIPtr->SetHoldItemProperty(
			AICharacterUnitPtr->SceneUnitContainer
		);
		UIPtr->ResetUIByData();

		{
			auto Delegate =
				UIPtr->OnDragIconDelegate.AddCallback(std::bind(&ThisClass::OnDragIcon, this, std::placeholders::_1, std::placeholders::_2));
			Delegate->bIsAutoUnregister = false;
		}

		UIPtr->ResetUIByData();
	}
	if (PlayerCharacterUnitPtr)
	{
		auto UIPtr = Cast<UBackpackMenu>(GetWidgetFromName(FAllocationSkillsMenu::Get().PlayerBackpack));

		UIPtr->SetHoldItemProperty(
			PlayerCharacterUnitPtr->SceneUnitContainer
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

void UAllocationSkillsMenu::OnSelectedCharacterUnit(UCharacterUnit* UnitPtr)
{
	if (CurrentUnitPtr == UnitPtr)
	{
		return;
	}

	auto UIPtr = Cast<UScrollBox>(GetWidgetFromName(FAllocationSkillsMenu::Get().GroupmateList));
	if (!UIPtr)
	{
		return;
	}
	auto ChildrensAry = UIPtr->GetAllChildren();
	for (auto Iter : ChildrensAry)
	{
		auto CharacterIconPtr = Cast<UGroupmateIcon>(Iter);
		if (CharacterIconPtr)
		{
			if (CharacterIconPtr->UnitPtr == UnitPtr)
			{
				auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
				if (!CharacterPtr)
				{
					return;
				}

				SyncAllocation2Character();

				ResetUI(UnitPtr, CharacterPtr->GetCharacterUnit());

				continue;
			}
			else
			{
				CharacterIconPtr->SwitchSelectState(false);
			}
		}
	}
}

void UAllocationSkillsMenu::OnSkillUnitChanged(UBasicUnit* PreviousUnitPtr, UBasicUnit* NewUnitPtr)
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

	bool bIsReplaced = false;
	for (const auto& Iter : Ary)
	{
		auto UIPtr = Cast<USkillsIcon>(GetWidgetFromName(Iter));
		if (UIPtr)
		{
			if (NewUnitPtr && (NewUnitPtr == UIPtr->BasicUnitPtr))
			{
				UIPtr->bPaseInvokeOnResetUnitEvent = true;
				UIPtr->ResetToolUIByData(PreviousUnitPtr);
				UIPtr->bPaseInvokeOnResetUnitEvent = false;

				bIsReplaced = true;

				break;
			}
			else
			{
			}
		}
	}

	SetAllocation(PreviousUnitPtr, NewUnitPtr, bIsReplaced);
}

void UAllocationSkillsMenu::OnWeaponUnitChanged(UBasicUnit* PreviousUnitPtr, UBasicUnit* NewUnitPtr)
{
	TArray<FName>Ary
	{
		{FAllocationSkillsMenu::Get().MainWeapon},
		{FAllocationSkillsMenu::Get().SecondaryWeapon},
	};

	bool bIsReplaced = false;
	for (const auto& Iter : Ary)
	{
		auto UIPtr = Cast<UWeaponsIcon>(GetWidgetFromName(Iter));
		if (UIPtr)
		{
			if (NewUnitPtr && (NewUnitPtr == UIPtr->BasicUnitPtr))
			{
				UIPtr->bPaseInvokeOnResetUnitEvent = true;
				UIPtr->ResetToolUIByData(PreviousUnitPtr);
				UIPtr->bPaseInvokeOnResetUnitEvent = false;

				bIsReplaced = true;

				break;
			}
			else
			{
			}
		}
	}

	SetAllocation(PreviousUnitPtr, NewUnitPtr, bIsReplaced);
}

void UAllocationSkillsMenu::OnConsumableUnitChanged(UBasicUnit* PreviousUnitPtr, UBasicUnit* NewUnitPtr)
{
	TArray<FName>Ary
	{
		{FAllocationSkillsMenu::Get().Consumable1},
		{FAllocationSkillsMenu::Get().Consumable2},
		{FAllocationSkillsMenu::Get().Consumable3},
		{FAllocationSkillsMenu::Get().Consumable4},
	};

	bool bIsReplaced = false;
	for (const auto& Iter : Ary)
	{
		auto UIPtr = Cast<UConsumableIcon>(GetWidgetFromName(Iter));
		if (UIPtr)
		{
			if (NewUnitPtr && (NewUnitPtr == UIPtr->BasicUnitPtr))
			{
				UIPtr->bPaseInvokeOnResetUnitEvent = true;
				UIPtr->ResetToolUIByData(PreviousUnitPtr);
				UIPtr->bPaseInvokeOnResetUnitEvent = false;

				bIsReplaced = true;

				break;
			}
			else
			{
			}
		}
	}

	SetAllocation(PreviousUnitPtr, NewUnitPtr, bIsReplaced);
}

void UAllocationSkillsMenu::SetAllocation(UBasicUnit* PreviousUnitPtr, UBasicUnit* NewUnitPtr, bool bIsReplaced)
{
	if (NewUnitPtr)
	{
		NewUnitPtr->SetAllocationCharacterUnit(CurrentUnitPtr);
	}

	if (PreviousUnitPtr)
	{
		if (bIsReplaced)
		{
			PreviousUnitPtr->SetAllocationCharacterUnit(CurrentUnitPtr);
		}
		else
		{
			PreviousUnitPtr->SetAllocationCharacterUnit(nullptr);
		}
	}
}
