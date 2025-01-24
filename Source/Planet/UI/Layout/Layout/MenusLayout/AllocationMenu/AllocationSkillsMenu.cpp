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
	const TSharedPtr<FCharacterProxy>& TargetCharacterProxyPtr,
	const TSharedPtr<FCharacterProxy>& PlayerCharacterProxyPtr
)
{
	auto CharacterPtr = PlayerCharacterProxyPtr->ProxyCharacterPtr;
	if (!CharacterPtr.IsValid())
	{
		return;
	}

	auto EICPtr = CharacterPtr->GetProxyProcessComponent();
	auto HoldingItemsComponentPtr = CharacterPtr->GetInventoryComponent();

	FCharacterSocket FirstWeaponSocketInfoSPtr;
	FCharacterSocket SecondWeaponSocketInfoSPtr;

	TargetCharacterProxyPtr->GetWeaponSocket(FirstWeaponSocketInfoSPtr, SecondWeaponSocketInfoSPtr);

	{
		{
			auto IconPtr = Cast<UWeaponsIcon>(GetWidgetFromName(FAllocationSkillsMenu::Get().WeaponSocket_1));
			if (IconPtr)
			{
				IconPtr->bPaseInvokeOnResetProxyEvent = true;
				IconPtr->ResetToolUIByData(
					HoldingItemsComponentPtr->FindProxy_BySocket(FirstWeaponSocketInfoSPtr)
				);
				IconPtr->bPaseInvokeOnResetProxyEvent = false;
			}
		}
	}

	{
		{
			auto IconPtr = Cast<UWeaponsIcon>(GetWidgetFromName(FAllocationSkillsMenu::Get().WeaponSocket_2));
			if (IconPtr)
			{
				IconPtr->bPaseInvokeOnResetProxyEvent = true;
				IconPtr->ResetToolUIByData(
					HoldingItemsComponentPtr->FindProxy_BySocket(SecondWeaponSocketInfoSPtr)
				);
				IconPtr->bPaseInvokeOnResetProxyEvent = false;
			}
		}
	}
}

void UAllocationSkillsMenu::ResetUIByData_Skills(const TSharedPtr<FCharacterProxy>& PlayerCharacterProxyPtr)
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

		auto CharacterPtr = PlayerCharacterProxyPtr->ProxyCharacterPtr;
		if (!CharacterPtr.IsValid())
		{
			return;
		}

		auto EICPtr = CharacterPtr->GetProxyProcessComponent();
		auto HoldingItemsComponentPtr = CharacterPtr->GetInventoryComponent();
		for (const auto& Iter : Ary)
		{
			auto IconPtr = Cast<USkillsIcon>(GetWidgetFromName(Iter));
			if (IconPtr)
			{
				auto Result = EICPtr->FindSocket(IconPtr->IconSocket);

				IconPtr->bPaseInvokeOnResetProxyEvent = true;
				IconPtr->ResetToolUIByData(
					HoldingItemsComponentPtr->FindProxy_BySocket(Result)
				);
				IconPtr->bPaseInvokeOnResetProxyEvent = false;
			}
		}
	}
}

void UAllocationSkillsMenu::ResetUIByData_Consumable(const TSharedPtr<FCharacterProxy>& PlayerCharacterProxyPtr)
{
	TArray<FName> Ary
	{
		FAllocationSkillsMenu::Get().Consumable1,
		FAllocationSkillsMenu::Get().Consumable2,
		FAllocationSkillsMenu::Get().Consumable3,
		FAllocationSkillsMenu::Get().Consumable4,
	};

	auto CharacterPtr = PlayerCharacterProxyPtr->ProxyCharacterPtr;
	if (!CharacterPtr.IsValid())
	{
		return;
	}

	auto ProxyProcessComponentPtr = CharacterPtr->GetProxyProcessComponent();
	auto HoldingItemsComponentPtr = PlayerCharacterProxyPtr->ProxyCharacterPtr->GetInventoryComponent();

	for (const auto& Iter : Ary)
	{
		auto IconPtr = Cast<UConsumableIcon>(GetWidgetFromName(Iter));
		if (IconPtr)
		{
			auto Result = ProxyProcessComponentPtr->FindSocket(IconPtr->IconSocket);

			IconPtr->bPaseInvokeOnResetProxyEvent = true;
			IconPtr->ResetToolUIByData(
				HoldingItemsComponentPtr->FindProxy_BySocket(Result)
			);
			IconPtr->bPaseInvokeOnResetProxyEvent = false;
		}
	}
}

void UAllocationSkillsMenu::SyncData()
{
	auto PCPtr = Cast<APlanetPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (!CurrentProxyPtr || !PCPtr)
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
			{FAllocationSkillsMenu::Get().WeaponSocket_2, UGameplayTagsLibrary::WeaponSocket_2},
		};

		for (const auto& Iter : Ary)
		{
			auto Socket = CurrentProxyPtr->FindSocket(Iter.Socket);
			auto UIPtr = Cast<UWeaponsIcon>(GetWidgetFromName(Iter.UIName));
			if (UIPtr)
			{
				if (UIPtr->BasicProxyPtr)
				{
					Socket.AllocationedProxyID = UIPtr->WeaponProxyPtr->GetID();
				}
				else
				{
					Socket.ResetSocket();
				}
				PCPtr->GetHoldingItemsComponent()->UpdateSocket(CurrentProxyPtr, Socket);
			}
		}
	}
	{
		TArray<FMyStruct> Ary
		{
			{FAllocationSkillsMenu::Get().ActiveSkill1, UGameplayTagsLibrary::ActiveSocket_1},
			{FAllocationSkillsMenu::Get().ActiveSkill2, UGameplayTagsLibrary::ActiveSocket_2},
			{FAllocationSkillsMenu::Get().ActiveSkill3, UGameplayTagsLibrary::ActiveSocket_3},
			{FAllocationSkillsMenu::Get().ActiveSkill4, UGameplayTagsLibrary::ActiveSocket_4},
		};

		for (const auto& Iter : Ary)
		{
			auto Socket = CurrentProxyPtr->FindSocket(Iter.Socket);
			auto UIPtr = Cast<USkillsIcon>(GetWidgetFromName(Iter.UIName));
			if (UIPtr)
			{
				if (UIPtr->BasicProxyPtr)
				{
					Socket.AllocationedProxyID = UIPtr->BasicProxyPtr->GetID();
				}
				else
				{
					Socket.ResetSocket();
				}
				PCPtr->GetHoldingItemsComponent()->UpdateSocket(CurrentProxyPtr, Socket);
			}
		}
	}
	{
		TArray<FMyStruct> Ary
		{
			{FAllocationSkillsMenu::Get().PassivSkill1, UGameplayTagsLibrary::PassiveSocket_1},
			{FAllocationSkillsMenu::Get().PassivSkill2, UGameplayTagsLibrary::PassiveSocket_2},
			{FAllocationSkillsMenu::Get().PassivSkill3, UGameplayTagsLibrary::PassiveSocket_3},
			{FAllocationSkillsMenu::Get().PassivSkill4, UGameplayTagsLibrary::PassiveSocket_4},
		};

		for (const auto& Iter : Ary)
		{
			auto Socket = CurrentProxyPtr->FindSocket(Iter.Socket);
			auto UIPtr = Cast<USkillsIcon>(GetWidgetFromName(Iter.UIName));
			if (UIPtr)
			{
				if (UIPtr->BasicProxyPtr)
				{
					Socket.AllocationedProxyID = UIPtr->BasicProxyPtr->GetID();
				}
				else
				{
					Socket.ResetSocket();
				}
				PCPtr->GetHoldingItemsComponent()->UpdateSocket(CurrentProxyPtr, Socket);
			}
		}
	}
	{
		TArray<FMyStruct> Ary
		{
			{FAllocationSkillsMenu::Get().Consumable1, UGameplayTagsLibrary::ConsumableSocket_1},
			{FAllocationSkillsMenu::Get().Consumable2, UGameplayTagsLibrary::ConsumableSocket_2},
			{FAllocationSkillsMenu::Get().Consumable3, UGameplayTagsLibrary::ConsumableSocket_3},
			{FAllocationSkillsMenu::Get().Consumable4, UGameplayTagsLibrary::ConsumableSocket_4},
		};

		for (const auto& Iter : Ary)
		{
			auto Socket = CurrentProxyPtr->FindSocket(Iter.Socket);
			auto UIPtr = Cast<UConsumableIcon>(GetWidgetFromName(Iter.UIName));
			if (UIPtr)
			{
				if (UIPtr->BasicProxyPtr)
				{
					Socket.AllocationedProxyID = UIPtr->BasicProxyPtr->GetID();
				}
				else
				{
					Socket.ResetSocket();
				}
				PCPtr->GetHoldingItemsComponent()->UpdateSocket(CurrentProxyPtr, Socket);
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
			MainDelegateHandleSPtr = WeaponIconPtr->OnResetProxy_Weapon.AddCallback(
				std::bind(&ThisClass::OnMainWeaponChanged, this, std::placeholders::_1)
			);
		}
	}
	{
		auto WeaponIconPtr = Cast<UWeaponsIcon>(GetWidgetFromName(FAllocationSkillsMenu::Get().WeaponSocket_2));
		if (WeaponIconPtr)
		{
			SecondaryDelegateHandleSPtr = WeaponIconPtr->OnResetProxy_Weapon.AddCallback(
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
						std::bind(
							&ThisClass::OnSkillProxyChanged, this, std::placeholders::_1, std::placeholders::_2,
							std::placeholders::_3
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
					std::bind(
						&ThisClass::OnConsumableProxyChanged, this, std::placeholders::_1, std::placeholders::_2,
						std::placeholders::_3
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
						std::bind(
							&ThisClass::OnWeaponProxyChanged, this, std::placeholders::_1, std::placeholders::_2,
							std::placeholders::_3
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
	const TSharedPtr<FCharacterProxy>& TargetCharacterProxyPtr,
	const TSharedPtr<FCharacterProxy>& PlayerCharacterProxyPtr
)
{
	CurrentProxyPtr = TargetCharacterProxyPtr;
	ResetUIByData_WeaponSkills(TargetCharacterProxyPtr, PlayerCharacterProxyPtr);
	if (TargetCharacterProxyPtr == PlayerCharacterProxyPtr)
	{
		ResetBackpack(nullptr, PlayerCharacterProxyPtr);
	}
	else
	{
		ResetBackpack(nullptr, PlayerCharacterProxyPtr);
	}
	ResetUIByData_Skills(CurrentProxyPtr);
	ResetUIByData_Consumable(CurrentProxyPtr);
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

		auto HICPtr = CharacterPtr->GetInventoryComponent();
		auto GroupmateProxyAry = HICPtr->GetCharacterProxyAry();

		for (int32 Index = 0; Index < GroupmateProxyAry.Num(); Index++)
		{
			auto WidgetPtr = CreateWidget<UGroupmateIcon>(this, GroupmateIconClass);
			if (WidgetPtr)
			{
				WidgetPtr->ResetToolUIByData(GroupmateProxyAry[Index]);
				{
					auto Handle = WidgetPtr->OnSelected.AddCallback(
						std::bind(&ThisClass::OnSelectedCharacterProxy, this, std::placeholders::_1));
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
	const TSharedPtr<FCharacterProxy>& AICharacterProxyPtr,
	const TSharedPtr<FCharacterProxy>& PlayerCharacterProxyPtr
)
{
	if (AICharacterProxyPtr)
	{
	}
	{
		auto UIPtr = Cast<UBackpackMenu>(GetWidgetFromName(FAllocationSkillsMenu::Get().TargetBackpack));

		UIPtr->CurrentProxyPtr = AICharacterProxyPtr;

		{
			auto Delegate =
				UIPtr->OnDragIconDelegate.AddCallback(
					std::bind(&ThisClass::OnItemProxyDragIcon, this, std::placeholders::_1, std::placeholders::_2));
			Delegate->bIsAutoUnregister = false;
		}

		UIPtr->ResetUIByData();
	}
	if (PlayerCharacterProxyPtr)
	{
	}
	{
		auto UIPtr = Cast<UBackpackMenu>(GetWidgetFromName(FAllocationSkillsMenu::Get().PlayerBackpack));

		UIPtr->CurrentProxyPtr = PlayerCharacterProxyPtr;

		{
			auto Delegate =
				UIPtr->OnDragIconDelegate.AddCallback(
					std::bind(&ThisClass::OnItemProxyDragIcon, this, std::placeholders::_1, std::placeholders::_2));
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

	auto SocketSPtr = CurrentProxyPtr->FindSocket(UAllocationIconPtr->IconSocket);
	if (UAllocationIconPtr->BasicProxyPtr)
	{
		SocketSPtr.AllocationedProxyID = UAllocationIconPtr->BasicProxyPtr->GetID();
	}
}

void UAllocationSkillsMenu::OnItemProxyDragIcon(bool bIsDragging, const TSharedPtr<FBasicProxy>& ProxyPtr)
{
	OnAllocationbableDragIcon(bIsDragging, DynamicCastSharedPtr<FAllocationbleProxy>(ProxyPtr));
}

void UAllocationSkillsMenu::OnAllocationbableDragIcon(bool bIsDragging, const TSharedPtr<FAllocationbleProxy>& ProxyPtr)
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
				IconPtr->OnDragIcon(bIsDragging, ProxyPtr);
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
				IconPtr->OnDragIcon(bIsDragging, ProxyPtr);
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
				IconPtr->OnDragIcon(bIsDragging, ProxyPtr);
			}
		}
	}
}

void UAllocationSkillsMenu::OnSelectedCharacterProxy(const TSharedPtr<FCharacterProxy>& ProxyPtr)
{
	if (CurrentProxyPtr == ProxyPtr)
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
			if (CharacterIconPtr->ProxyPtr == ProxyPtr)
			{
				auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
				if (!CharacterPtr)
				{
					return;
				}

				SyncData();

				ResetUI(ProxyPtr, CharacterPtr->GetCharacterProxy());

				continue;
			}
			else
			{
				CharacterIconPtr->SwitchSelectState(false);
			}
		}
	}
}

void UAllocationSkillsMenu::OnSkillProxyChanged(
	const TSharedPtr<FAllocationbleProxy>& PreviousProxyPtr,
	const TSharedPtr<FAllocationbleProxy>& NewProxyPtr,
	const FGameplayTag& SocketTag
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
			if (NewProxyPtr && (NewProxyPtr == UIPtr->BasicProxyPtr))
			{
				UIPtr->bPaseInvokeOnResetProxyEvent = true;
				UIPtr->ResetToolUIByData(PreviousProxyPtr);
				UIPtr->bPaseInvokeOnResetProxyEvent = false;

				bIsReplaced = true;

				break;
			}
			else
			{
			}
		}
	}

	SetAllocation(PreviousProxyPtr, NewProxyPtr, SocketTag, bIsReplaced);
}

void UAllocationSkillsMenu::OnWeaponProxyChanged(
	const TSharedPtr<FAllocationbleProxy>& PreviousProxyPtr,
	const TSharedPtr<FAllocationbleProxy>& NewProxyPtr,
	const FGameplayTag& SocketTag
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
			if (NewProxyPtr && (NewProxyPtr == UIPtr->BasicProxyPtr))
			{
				UIPtr->bPaseInvokeOnResetProxyEvent = true;
				UIPtr->ResetToolUIByData(PreviousProxyPtr);
				UIPtr->bPaseInvokeOnResetProxyEvent = false;

				bIsReplaced = true;

				break;
			}
			else
			{
			}
		}
	}

	SetAllocation(PreviousProxyPtr, NewProxyPtr, SocketTag, bIsReplaced);
}

void UAllocationSkillsMenu::OnConsumableProxyChanged(
	const TSharedPtr<FAllocationbleProxy>& PreviousProxyPtr,
	const TSharedPtr<FAllocationbleProxy>& NewProxyPtr,
	const FGameplayTag& SocketTag
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
			if (NewProxyPtr && (NewProxyPtr == UIPtr->BasicProxyPtr))
			{
				UIPtr->bPaseInvokeOnResetProxyEvent = true;
				UIPtr->ResetToolUIByData(PreviousProxyPtr);
				UIPtr->bPaseInvokeOnResetProxyEvent = false;

				bIsReplaced = true;

				break;
			}
			else
			{
			}
		}
	}

	SetAllocation(PreviousProxyPtr, NewProxyPtr, SocketTag, bIsReplaced);
}

void UAllocationSkillsMenu::SetAllocation(
	const TSharedPtr<FAllocationbleProxy>& PreviousProxyPtr,
	const TSharedPtr<FAllocationbleProxy>& NewProxyPtr,
	const FGameplayTag& SocketTag,
	bool bIsReplaced
)
{
	if (NewProxyPtr)
	{
		NewProxyPtr->SetAllocationCharacterProxy(CurrentProxyPtr, SocketTag);
	}

	if (PreviousProxyPtr)
	{
		if (bIsReplaced)
		{
			PreviousProxyPtr->SetAllocationCharacterProxy(CurrentProxyPtr, SocketTag);
		}
		else
		{
			PreviousProxyPtr->SetAllocationCharacterProxy(nullptr, SocketTag);
		}
	}
}
