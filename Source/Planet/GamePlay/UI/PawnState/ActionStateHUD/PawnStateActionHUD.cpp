
#include "PawnStateActionHUD.h"

#include "Kismet/GameplayStatics.h"
#include "Components/HorizontalBox.h"
#include "Components/Border.h"

#include "PlanetPlayerState.h"
#include "Planet.h"
#include "CharacterBase.h"
#include "EquipmentElementComponent.h"
#include "ActionSkillsIcon.h"
#include "CharacterAttibutes.h"
#include "AssetRefMap.h"
#include "State_Talent_NuQi.h"
#include "MyProgressBar.h"
#include "MyBaseProperty.h"

namespace AllocationSkillsMenu
{
	const FName SkillsHorizontal = TEXT("SkillsHorizontal");

	const FName ActiveSkill1 = TEXT("ActiveSkill1");

	const FName ActiveSkill2 = TEXT("ActiveSkill2");

	const FName ActiveSkill3 = TEXT("ActiveSkill3");

	const FName ActiveSkill4 = TEXT("ActiveSkill4");

	const FName WeaponActiveSkill1 = TEXT("WeaponActiveSkill1");

	const FName WeaponActiveSkill2 = TEXT("WeaponActiveSkill2");
	
	const FName TalentStateSocket = TEXT("TalentStateSocket");
	
	const FName AD = TEXT("AD");

	const FName AD_Penetration = TEXT("AD_Penetration");
	
	const FName AD_Resistance = TEXT("AD_Resistance");

	const FName AP = TEXT("AP");

	const FName AP_Penetration = TEXT("AP_Penetration");

	const FName AP_Resistance = TEXT("AP_Resistance");

	const FName MoveSpeed = TEXT("MoveSpeed");

	const FName SkillPerformSpeed = TEXT("SkillPerformSpeed");

	const FName HP = TEXT("HP");

	const FName PP = TEXT("PP");

	const FName Shield = TEXT("Shield");
}

void UPawnStateActionHUD::NativeConstruct()
{
	Super::NativeConstruct();

	ResetUIByData();
}

void UPawnStateActionHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	{
		TArray<FName>Ary
		{
			AllocationSkillsMenu::ActiveSkill1,
			AllocationSkillsMenu::ActiveSkill2,
			AllocationSkillsMenu::ActiveSkill3,
			AllocationSkillsMenu::ActiveSkill4 ,
			AllocationSkillsMenu::WeaponActiveSkill1,
			AllocationSkillsMenu::WeaponActiveSkill2,
		};

		for (const auto& Iter : Ary)
		{
			auto IconPtr = Cast<UActionSkillsIcon>(GetWidgetFromName(Iter));
			if (IconPtr && IconPtr->ToolSPtr)
			{
				IconPtr->UpdateSkillState();
			}
		}
	}
}

void UPawnStateActionHUD::ResetUIByData()
{
	if (!CharacterPtr)
	{
		return;
	}
	{
		auto & CharacterAttributes = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
		{
			auto UIPtr = Cast<UMyProgressBar>(GetWidgetFromName(AllocationSkillsMenu::HP));
			if (!UIPtr)
			{
				return;
			}
			UIPtr->SetDataSource(CharacterAttributes.HP);
		}
		{
			auto UIPtr = Cast<UMyProgressBar>(GetWidgetFromName(AllocationSkillsMenu::PP));
			if (!UIPtr)
			{
				return;
			}
			UIPtr->SetDataSource(CharacterAttributes.PP);
		}
		{
			auto UIPtr = Cast<UMyBaseProperty>(GetWidgetFromName(AllocationSkillsMenu::AD));
			if (!UIPtr)
			{
				return;
			}
			UIPtr->SetDataSource(CharacterAttributes.AD);
		}
		{
			auto UIPtr = Cast<UMyBaseProperty>(GetWidgetFromName(AllocationSkillsMenu::AD_Penetration));
			if (!UIPtr)
			{
				return;
			}
			UIPtr->SetDataSource(CharacterAttributes.AD_Penetration, CharacterAttributes.AD_PercentPenetration);
		}
	}
	InitialTalentUI();
	InitialSkillIcon();
}

void UPawnStateActionHUD::InitialTalentUI()
{
	auto BorderPtr = Cast<UBorder>(GetWidgetFromName(AllocationSkillsMenu::TalentStateSocket));
	if (!BorderPtr)
	{
		return;
	}
	BorderPtr->ClearChildren();

	if (!CharacterPtr)
	{
		return;
	}
	const auto& SkillsMap = CharacterPtr->GetEquipmentItemsComponent()->GetSkills();
	for (auto Iter : SkillsMap)
	{
		bool bIsGiveTalentPassive = false;
		if (Iter.Value.SkillUnit)
		{
			switch (Iter.Value.SkillUnit->SkillType)
			{
			case ESkillType::kTalentPassive:
			{
				switch (Iter.Value.SkillUnit->GetSceneElementType<ESkillUnitType>())
				{
				case ESkillUnitType::kHumanSkill_TalentSkill_NuQi:
				{
					auto UIPtr = CreateWidget<UState_Talent_NuQi>(this, TalentState_NuQi_Class);
					if (UIPtr)
					{
						BorderPtr->AddChild(UIPtr);
						bIsGiveTalentPassive = true;
					}
				}
				break;
				}
			}
			};
			if (bIsGiveTalentPassive)
			{
				break;
			}
		}
	}
}

void UPawnStateActionHUD::InitialSkillIcon()
{
	if (!CharacterPtr)
	{
		return;
	}
	auto SkillsMap = CharacterPtr->GetEquipmentItemsComponent()->GetSkills();
	auto HorizontalBoxPtr = Cast<UHorizontalBox>(GetWidgetFromName(AllocationSkillsMenu::SkillsHorizontal));
	if (HorizontalBoxPtr)
	{
		auto ChildsAry = HorizontalBoxPtr->GetAllChildren();
		for (auto Iter : ChildsAry)
		{
			auto SkillIcon = Cast<UActionSkillsIcon>(Iter);
			if (SkillIcon)
			{
				auto SocketIter = SkillsMap.Find(SkillIcon->IconSocket);
				if (SocketIter)
				{
					SkillIcon->ResetToolUIByData(SocketIter->SkillUnit);
				}
				else
				{
					SkillIcon->ResetToolUIByData(nullptr);
				}
			}
		}
	}
}