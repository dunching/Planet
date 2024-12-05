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
#include "ProxyProcessComponent.h"
#include "UICommon.h"
#include "GameplayTagsLibrary.h"
#include "BackpackMenu.h"
#include "PlanetPlayerState.h"
#include "ConsumableIcon.h"
#include "GroupmateIcon.h"
#include "ItemProxy_Container.h"
#include "PlanetPlayerController.h"
#include "AllocationSkills.h"
#include "ItemProxy_Character.h"

struct FAllocationSkillsMenu : public TStructVariable<FAllocationSkillsMenu>
{
	const FName PlayerBackpack = TEXT("PlayerBackpack");

	const FName TargetBackpack = TEXT("TargetBackpack");

	const FName GroupmateList = TEXT("GroupmateList");

	const FName WeaponSocket_1 = TEXT("WeaponSocket1");

	const FName WeaponSocket_2 = TEXT("WeaponSocket2");

	const FName ActiveSkill1 = TEXT("ActiveSkill1");

	const FName ActiveSkill2 = TEXT("ActiveSkill2");

	const FName ActiveSkill3 = TEXT("ActiveSkill3");

	const FName ActiveSkill4 = TEXT("ActiveSkill4");

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

	SyncData();
}

void UAllocationSkillsMenu::ResetUIByData()
{
	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!CharacterPtr)
	{
		return;
	}

	InitialGroupmateList();

	ResetUI(CharacterPtr->GetCharacterProxy(), CharacterPtr->GetCharacterProxy());
}

void UAllocationSkillsMenu::ResetUIByData_WeaponSkills(
	const TSharedPtr<FCharacterProxy>& TargetCharacterUnitPtr,
	const TSharedPtr<FCharacterProxy>& PlayerCharacterUnitPtr
)
{
	auto CharacterPtr = PlayerCharacterUnitPtr->ProxyCharacterPtr;
	if (!CharacterPtr.IsValid())
	{
		return;
	}

	auto EICPtr = CharacterPtr->GetProxyProcessComponent();
	auto HoldingItemsComponentPtr = CharacterPtr->GetHoldingItemsComponent();

	FMySocket_FASI FirstWeaponSocketInfoSPtr;
	FMySocket_FASI SecondWeaponSocketInfoSPtr;

	TargetCharacterUnitPtr->GetWeaponSocket(FirstWeaponSocketInfoSPtr, SecondWeaponSocketInfoSPtr);

	{
		{
			auto IconPtr = Cast<UWeaponsIcon>(GetWidgetFromName(FAllocationSkillsMenu::Get().WeaponSocket_1));
			if (IconPtr)
			{
				IconPtr->bPaseInvokeOnResetUnitEvent = true;
				IconPtr->ResetToolUIByData(
					HoldingItemsComponentPtr->FindProxy_BySocket(FirstWeaponSocketInfoSPtr)
				);
				IconPtr->bPaseInvokeOnResetUnitEvent = false;
			}
		}
	}

	{
		{
			auto IconPtr = Cast<UWeaponsIcon>(GetWidgetFromName(FAllocationSkillsMenu::Get().WeaponSocket_2));
			if (IconPtr)
			{
				IconPtr->bPaseInvokeOnResetUnitEvent = true;
				IconPtr->ResetToolUIByData(
					HoldingItemsComponentPtr->FindProxy_BySocket(SecondWeaponSocketInfoSPtr)
				);
				IconPtr->bPaseInvokeOnResetUnitEvent = false;
			}
		}
	}
}

void UAllocationSkillsMenu::ResetUIByData_Skills(const TSharedPtr<FCharacterProxy>& PlayerCharacterUnitPtr)
{
	{
		TArray<FName> Ary
		{
			FAllocationSkillsMenu::Get().ActiveSkill1,
			FAllocationSkillsMenu::Get().ActiveSkill2,
			FAllocationSkillsMenu::Get().ActiveSkill3,
			FAllocationSkillsMenu::Get().ActiveSkill4,
			FAllocationSkillsMenu::Get().PassivSkill1,
			FAllocationSkillsMenu::Get().PassivSkill2,
			FAllocationSkillsMenu::Get().PassivSkill3,
			FAllocationSkillsMenu::Get().PassivSkill4,
			FAllocationSkillsMenu::Get().PassivSkill5,
			FAllocationSkillsMenu::Get().TalentPassivSkill,
		};

		auto CharacterPtr = PlayerCharacterUnitPtr->ProxyCharacterPtr;
		if (!CharacterPtr.IsValid())
		{
			return;
		}

		auto EICPtr = CharacterPtr->GetProxyProcessComponent();
		auto HoldingItemsComponentPtr = CharacterPtr->GetHoldingItemsComponent();
		for (const auto& Iter : Ary)
		{
			auto IconPtr = Cast<USkillsIcon>(GetWidgetFromName(Iter));
			if (IconPtr)
			{
				auto Result = EICPtr->FindSocket(IconPtr->IconSocket);

				IconPtr->bPaseInvokeOnResetUnitEvent = true;
				IconPtr->ResetToolUIByData(
					HoldingItemsComponentPtr->FindProxy_BySocket(Result)
				);
				IconPtr->bPaseInvokeOnResetUnitEvent = false;
			}
		}
	}
}

void UAllocationSkillsMenu::ResetUIByData_Consumable(const TSharedPtr<FCharacterProxy>& PlayerCharacterUnitPtr)
{
	TArray<FName> Ary
	{
		FAllocationSkillsMenu::Get().Consumable1,
		FAllocationSkillsMenu::Get().Consumable2,
		FAllocationSkillsMenu::Get().Consumable3,
		FAllocationSkillsMenu::Get().Consumable4,
	};

	auto CharacterPtr = PlayerCharacterUnitPtr->ProxyCharacterPtr;
	if (!CharacterPtr.IsValid())
	{
		return;
	}

	auto ProxyProcessComponentPtr = CharacterPtr->GetProxyProcessComponent();
	auto HoldingItemsComponentPtr = PlayerCharacterUnitPtr->ProxyCharacterPtr->GetHoldingItemsComponent();

	for (const auto& Iter : Ary)
	{
		auto IconPtr = Cast<UConsumableIcon>(GetWidgetFromName(Iter));
		if (IconPtr)
		{
			auto Result = ProxyProcessComponentPtr->FindSocket(IconPtr->IconSocket);

			IconPtr->bPaseInvokeOnResetUnitEvent = true;
			IconPtr->ResetToolUIByData(
				HoldingItemsComponentPtr->FindProxy_BySocket(Result)
			);
			IconPtr->bPaseInvokeOnResetUnitEvent = false;
		}
	}
}

void UAllocationSkillsMenu::SyncData()
{
	auto PCPtr = Cast<APlanetPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (!CurrentUnitPtr || !PCPtr)
	{
		return;
	}

	struct FMyStruct
	{
		FName UIName;

		FGameplayTag Socket;
	};
	{
		TArray<FMyStruct> Ary
		{
			{FAllocationSkillsMenu::Get().WeaponSocket_1, UGameplayTagsLibrary::WeaponSocket_1},
		};

		for (const auto& Iter : Ary)
		{
			auto Socket = CurrentUnitPtr->FindSocket(Iter.Socket);
			auto UIPtr = Cast<UWeaponsIcon>(GetWidgetFromName(Iter.UIName));
			if (UIPtr)
			{
				if (UIPtr->BasicUnitPtr)
				{
					Socket.SkillProxyID = UIPtr->WeaponProxyPtr->GetID();
				}
				else
				{
					Socket.ResetSocket();
				}
				PCPtr->GetHoldingItemsComponent()->UpdateSocket(CurrentUnitPtr, Socket);
			}
		}
	}
	{
		TArray<FMyStruct> Ary
		{
			{FAllocationSkillsMenu::Get().ActiveSkill1, UGameplayTagsLibrary::ActiveSocket_1},
		};

		for (const auto& Iter : Ary)
		{
			auto Socket = CurrentUnitPtr->FindSocket(Iter.Socket);
			auto UIPtr = Cast<USkillsIcon>(GetWidgetFromName(Iter.UIName));
			if (UIPtr)
			{
				if (UIPtr->BasicUnitPtr)
				{
					Socket.SkillProxyID = UIPtr->BasicUnitPtr->GetID();
				}
				else
				{
					Socket.ResetSocket();
				}
				PCPtr->GetHoldingItemsComponent()->UpdateSocket(CurrentUnitPtr, Socket);
			}
		}
	}
}

void UAllocationSkillsMenu::BindEvent()
{
	{
		auto WeaponIconPtr = Cast<UWeaponsIcon>(GetWidgetFromName(FAllocationSkillsMenu::Get().WeaponSocket_1));
		if (WeaponIconPtr)
		{
			MainDelegateHandleSPtr = WeaponIconPtr->OnResetUnit_Weapon.AddCallback(
				std::bind(&ThisClass::OnMainWeaponChanged, this, std::placeholders::_1)
			);
		}
	}
	{
		auto WeaponIconPtr = Cast<UWeaponsIcon>(GetWidgetFromName(FAllocationSkillsMenu::Get().WeaponSocket_2));
		if (WeaponIconPtr)
		{
			SecondaryDelegateHandleSPtr = WeaponIconPtr->OnResetUnit_Weapon.AddCallback(
				std::bind(&ThisClass::OnSecondaryWeaponChanged, this, std::placeholders::_1)
			);
		}
	}
	{
		TArray<FName> Ary
		{
			{FAllocationSkillsMenu::Get().ActiveSkill1},
			{FAllocationSkillsMenu::Get().ActiveSkill2},
			{FAllocationSkillsMenu::Get().ActiveSkill3},
			{FAllocationSkillsMenu::Get().ActiveSkill4},
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
				{
					auto Result = UIPtr->OnResetProxy.AddCallback(
						std::bind(&ThisClass::OnSkillUnitChanged, this, std::placeholders::_1, std::placeholders::_2
						)
					);
					Result->bIsAutoUnregister = false;
				}
				{
					auto Result = UIPtr->OnResetData.AddCallback(
						std::bind(&ThisClass::OnResetData, this, std::placeholders::_1
						)
					);
					Result->bIsAutoUnregister = false;
				}
			}
		}
	}
	{
		TArray<FName> Ary
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
				auto Result = UIPtr->OnResetProxy.AddCallback(
					std::bind(&ThisClass::OnConsumableUnitChanged, this, std::placeholders::_1, std::placeholders::_2
					)
				);
				Result->bIsAutoUnregister = false;
			}
			{
				auto Result = UIPtr->OnResetData.AddCallback(
					std::bind(&ThisClass::OnResetData, this, std::placeholders::_1
					)
				);
				Result->bIsAutoUnregister = false;
			}
		}
	}
	{
		TArray<FName> Ary
		{
			{FAllocationSkillsMenu::Get().WeaponSocket_1},
			{FAllocationSkillsMenu::Get().WeaponSocket_2},
		};

		for (const auto& Iter : Ary)
		{
			auto UIPtr = Cast<UWeaponsIcon>(GetWidgetFromName(Iter));
			if (UIPtr)
			{
				{
					auto Result = UIPtr->OnResetProxy.AddCallback(
						std::bind(&ThisClass::OnWeaponUnitChanged, this, std::placeholders::_1, std::placeholders::_2
						)
					);
					Result->bIsAutoUnregister = false;
				}
				{
					auto Result = UIPtr->OnResetData.AddCallback(
						std::bind(&ThisClass::OnResetData, this, std::placeholders::_1
						)
					);
					Result->bIsAutoUnregister = false;
				}
			}
		}
	}
}

void UAllocationSkillsMenu::ResetUI(
	const TSharedPtr<FCharacterProxy>& TargetCharacterUnitPtr,
	const TSharedPtr<FCharacterProxy>& PlayerCharacterUnitPtr
)
{
	CurrentUnitPtr = TargetCharacterUnitPtr;
	ResetUIByData_WeaponSkills(TargetCharacterUnitPtr, PlayerCharacterUnitPtr);
	if (TargetCharacterUnitPtr == PlayerCharacterUnitPtr)
	{
		ResetBackpack(nullptr, PlayerCharacterUnitPtr);
	}
	else
	{
		ResetBackpack(nullptr, PlayerCharacterUnitPtr);
	}
	ResetUIByData_Skills(CurrentUnitPtr);
	ResetUIByData_Consumable(CurrentUnitPtr);
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

		auto HICPtr = CharacterPtr->GetHoldingItemsComponent();
		auto GroupmateUnitAry = HICPtr->GetCharacterProxyAry();

		for (int32 Index = 0; Index < GroupmateUnitAry.Num(); Index++)
		{
			auto WidgetPtr = CreateWidget<UGroupmateIcon>(this, GroupmateIconClass);
			if (WidgetPtr)
			{
				WidgetPtr->ResetToolUIByData(GroupmateUnitAry[Index]);
				{
					auto Handle = WidgetPtr->OnSelected.AddCallback(
						std::bind(&ThisClass::OnSelectedCharacterUnit, this, std::placeholders::_1));
					Handle->bIsAutoUnregister = false;
				}
				UIPtr->AddChild(WidgetPtr);

				if (Index == 0)
				{
					WidgetPtr->SwitchSelectState(true);
				}
			}
		}
	}
}

void UAllocationSkillsMenu::ResetBackpack(
	const TSharedPtr<FCharacterProxy>& AICharacterUnitPtr,
	const TSharedPtr<FCharacterProxy>& PlayerCharacterUnitPtr
)
{
	if (AICharacterUnitPtr)
	{
	}
	{
		auto UIPtr = Cast<UBackpackMenu>(GetWidgetFromName(FAllocationSkillsMenu::Get().TargetBackpack));

		UIPtr->CurrentUnitPtr = AICharacterUnitPtr;

		{
			auto Delegate =
				UIPtr->OnDragIconDelegate.AddCallback(
					std::bind(&ThisClass::OnDragIcon, this, std::placeholders::_1, std::placeholders::_2));
			Delegate->bIsAutoUnregister = false;
		}

		UIPtr->ResetUIByData();
	}
	if (PlayerCharacterUnitPtr)
	{
	}
	{
		auto UIPtr = Cast<UBackpackMenu>(GetWidgetFromName(FAllocationSkillsMenu::Get().PlayerBackpack));

		UIPtr->CurrentUnitPtr = PlayerCharacterUnitPtr;

		{
			auto Delegate =
				UIPtr->OnDragIconDelegate.AddCallback(
					std::bind(&ThisClass::OnDragIcon, this, std::placeholders::_1, std::placeholders::_2));
			Delegate->bIsAutoUnregister = false;
		}

		UIPtr->ResetUIByData();
	}
}

void UAllocationSkillsMenu::OnMainWeaponChanged(const TSharedPtr<FWeaponProxy>& ToolSPtr)
{
}

void UAllocationSkillsMenu::OnSecondaryWeaponChanged(const TSharedPtr<FWeaponProxy>& ToolSPtr)
{
}

void UAllocationSkillsMenu::OnResetData(UAllocationIconBase* UAllocationIconPtr)
{
	if (!UAllocationIconPtr)
	{
		return;
	}

	auto SocketSPtr = CurrentUnitPtr->FindSocket(UAllocationIconPtr->IconSocket);
	SocketSPtr.SkillProxyID = UAllocationIconPtr->BasicUnitPtr->GetID();
}

void UAllocationSkillsMenu::OnDragIcon(bool bIsDragging, const TSharedPtr<FBasicProxy>& UnitPtr)
{
	{
		TArray<FName> Ary
		{
			{FAllocationSkillsMenu::Get().ActiveSkill1},
			{FAllocationSkillsMenu::Get().ActiveSkill2},
			{FAllocationSkillsMenu::Get().ActiveSkill3},
			{FAllocationSkillsMenu::Get().ActiveSkill4},
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
		TArray<FName> Ary
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
		TArray<FName> Ary
		{
			{FAllocationSkillsMenu::Get().WeaponSocket_1},
			{FAllocationSkillsMenu::Get().WeaponSocket_2},
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

void UAllocationSkillsMenu::OnSelectedCharacterUnit(const TSharedPtr<FCharacterProxy>& UnitPtr)
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

				SyncData();

				ResetUI(UnitPtr, CharacterPtr->GetCharacterProxy());

				continue;
			}
			else
			{
				CharacterIconPtr->SwitchSelectState(false);
			}
		}
	}
}

void UAllocationSkillsMenu::OnSkillUnitChanged(
	const TSharedPtr<FBasicProxy>& PreviousUnitPtr,
	const TSharedPtr<FBasicProxy>& NewUnitPtr
)
{
	TArray<FName> Ary
	{
		{FAllocationSkillsMenu::Get().ActiveSkill1},
		{FAllocationSkillsMenu::Get().ActiveSkill2},
		{FAllocationSkillsMenu::Get().ActiveSkill3},
		{FAllocationSkillsMenu::Get().ActiveSkill4},
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

void UAllocationSkillsMenu::OnWeaponUnitChanged(
	const TSharedPtr<FBasicProxy>& PreviousUnitPtr,
	const TSharedPtr<FBasicProxy>& NewUnitPtr
)
{
	TArray<FName> Ary
	{
		{FAllocationSkillsMenu::Get().WeaponSocket_1},
		{FAllocationSkillsMenu::Get().WeaponSocket_2},
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

void UAllocationSkillsMenu::OnConsumableUnitChanged(
	const TSharedPtr<FBasicProxy>& PreviousUnitPtr,
	const TSharedPtr<FBasicProxy>& NewUnitPtr
)
{
	TArray<FName> Ary
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

void UAllocationSkillsMenu::SetAllocation(
	const TSharedPtr<FBasicProxy>& PreviousUnitPtr
	, const TSharedPtr<FBasicProxy>& NewUnitPtr,
	bool bIsReplaced
)
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
