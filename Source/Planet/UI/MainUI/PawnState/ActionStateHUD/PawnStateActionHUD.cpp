
#include "PawnStateActionHUD.h"

#include "Kismet/GameplayStatics.h"
#include "Components/HorizontalBox.h"
#include "Components/Border.h"

#include "PlanetPlayerState.h"
#include "Planet.h"
#include "CharacterBase.h"
#include "InteractiveSkillComponent.h"
#include "ActionSkillsIcon.h"
#include "CharacterAttibutes.h"
#include "AssetRefMap.h"
#include "MyProgressBar.h"
#include "MyBaseProperty.h"
#include "LogWriter.h"
#include "State_Talent_NuQi.h"
#include "State_Talent_YinYang.h"
#include "GameplayTagsSubSystem.h"

struct FPawnStateActionHUD : public TStructVariable<FPawnStateActionHUD>
{
	const FName ActiveSkill1 = TEXT("ActiveSkill1");

	const FName ActiveSkill2 = TEXT("ActiveSkill2");

	const FName ActiveSkill3 = TEXT("ActiveSkill3");

	const FName ActiveSkill4 = TEXT("ActiveSkill4");

	const FName WeaponActiveSkill1 = TEXT("WeaponActiveSkill1");

	const FName WeaponActiveSkill2 = TEXT("WeaponActiveSkill2");

	const FName TalentStateSocket = TEXT("TalentStateSocket");

	const FName BaseAttackPower = TEXT("BaseAttackPower");

	const FName Penetration = TEXT("Penetration");

	const FName Resistance = TEXT("Resistance");

	const FName MoveSpeed = TEXT("MoveSpeed");

	const FName GAPerformSpeed = TEXT("GAPerformSpeed");

	const FName HP = TEXT("HP");

	const FName PP = TEXT("PP");

	const FName Shield = TEXT("Shield");

	const FName Gold = TEXT("Gold");

	const FName Wood = TEXT("Wood");

	const FName Water = TEXT("Water");

	const FName Fire = TEXT("Fire");

	const FName Soil = TEXT("Soil");
};

void UPawnStateActionHUD::NativeConstruct()
{
	Super::NativeConstruct();

	ResetUIByData();
}

void UPawnStateActionHUD::NativeDestruct()
{
	if (ActivedWeaponChangedDelegate)
	{
		ActivedWeaponChangedDelegate->UnBindCallback();
	}

	Super::NativeDestruct();
}

void UPawnStateActionHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	{
		TArray<FName>Ary
		{
			FPawnStateActionHUD::Get().ActiveSkill1,
			FPawnStateActionHUD::Get().ActiveSkill2,
			FPawnStateActionHUD::Get().ActiveSkill3,
			FPawnStateActionHUD::Get().ActiveSkill4 ,
			FPawnStateActionHUD::Get().WeaponActiveSkill1,
			FPawnStateActionHUD::Get().WeaponActiveSkill2,
		};

		for (const auto& Iter : Ary)
		{
			auto IconPtr = Cast<UActionSkillsIcon>(GetWidgetFromName(Iter));
			if (IconPtr && IconPtr->UnitPtr)
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
			auto UIPtr = Cast<UMyProgressBar>(GetWidgetFromName(FPawnStateActionHUD::Get().HP));
			if (!UIPtr)
			{
				return;
			}
			UIPtr->SetDataSource(CharacterAttributes.HP);
		}
		{
			auto UIPtr = Cast<UMyProgressBar>(GetWidgetFromName(FPawnStateActionHUD::Get().PP));
			if (!UIPtr)
			{
				return;
			}
			UIPtr->SetDataSource(CharacterAttributes.PP);
		}
		{
			auto UIPtr = Cast<UMyBaseProperty>(GetWidgetFromName(FPawnStateActionHUD::Get().BaseAttackPower));
			if (!UIPtr)
			{
				return;
			}
			UIPtr->SetDataSource(CharacterAttributes.BaseAttackPower);
		}
		{
			auto UIPtr = Cast<UMyBaseProperty>(GetWidgetFromName(FPawnStateActionHUD::Get().Penetration));
			if (!UIPtr)
			{
				return;
			}
			UIPtr->SetDataSource(CharacterAttributes.Penetration, CharacterAttributes.PercentPenetration);
		}
		{
			auto UIPtr = Cast<UMyBaseProperty>(GetWidgetFromName(FPawnStateActionHUD::Get().Resistance));
			if (!UIPtr)
			{
				return;
			}
			UIPtr->SetDataSource(CharacterAttributes.Resistance);
		}
		{
			auto UIPtr = Cast<UMyBaseProperty>(GetWidgetFromName(FPawnStateActionHUD::Get().MoveSpeed));
			if (!UIPtr)
			{
				return;
			}
			UIPtr->SetDataSource(CharacterAttributes.MoveSpeed);
		}
		{
			auto UIPtr = Cast<UMyBaseProperty>(GetWidgetFromName(FPawnStateActionHUD::Get().GAPerformSpeed));
			if (!UIPtr)
			{
				return;
			}
			UIPtr->SetDataSource(CharacterAttributes.GAPerformSpeed);
		}
		{
			auto UIPtr = Cast<UMyBaseProperty>(GetWidgetFromName(FPawnStateActionHUD::Get().Gold));
			if (!UIPtr)
			{
				return;
			}
			UIPtr->SetDataSource(CharacterAttributes.Element.GoldElement);
		}
		{
			auto UIPtr = Cast<UMyBaseProperty>(GetWidgetFromName(FPawnStateActionHUD::Get().Wood));
			if (!UIPtr)
			{
				return;
			}
			UIPtr->SetDataSource(CharacterAttributes.Element.WoodElement);
		}
		{
			auto UIPtr = Cast<UMyBaseProperty>(GetWidgetFromName(FPawnStateActionHUD::Get().Water));
			if (!UIPtr)
			{
				return;
			}
			UIPtr->SetDataSource(CharacterAttributes.Element.WaterElement);
		}
		{
			auto UIPtr = Cast<UMyBaseProperty>(GetWidgetFromName(FPawnStateActionHUD::Get().Fire));
			if (!UIPtr)
			{
				return;
			}
			UIPtr->SetDataSource(CharacterAttributes.Element.FireElement);
		}
		{
			auto UIPtr = Cast<UMyBaseProperty>(GetWidgetFromName(FPawnStateActionHUD::Get().Soil));
			if (!UIPtr)
			{
				return;
			}
			UIPtr->SetDataSource(CharacterAttributes.Element.SoilElement);
		}
	}
	InitialTalentUI();
	InitialSkillIcon();
}

void UPawnStateActionHUD::InitialTalentUI()
{
	auto BorderPtr = Cast<UBorder>(GetWidgetFromName(FPawnStateActionHUD::Get().TalentStateSocket));
	if (!BorderPtr)
	{
		return;
	}
	BorderPtr->ClearChildren();

	if (!CharacterPtr)
	{
		return;
	}
	const auto& SkillsMap = CharacterPtr->GetInteractiveSkillComponent()->GetSkills();
	for (auto Iter : SkillsMap)
	{
		bool bIsGiveTalentPassive = false;
		if (
			Iter.Value->SkillUnit && 
			(Iter.Value->SkillUnit->Level > 0)
			)
		{
			if (Iter.Value->SkillUnit->GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Talent_NuQi))
			{
				auto UIPtr = CreateWidget<UState_Talent_NuQi>(this, State_Talent_NuQi_Class);
				if (UIPtr)
				{
					BorderPtr->AddChild(UIPtr);
					bIsGiveTalentPassive = true;
				}
			}
			else if (Iter.Value->SkillUnit->GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Talent_YinYang))
			{
				auto UIPtr = CreateWidget<UState_Talent_YinYang>(this, Talent_YinYang_Class);
				if (UIPtr)
				{
					UIPtr->TargetCharacterPtr = CharacterPtr;
					BorderPtr->AddChild(UIPtr);
					bIsGiveTalentPassive = true;
				}
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

	auto SkillsMap = CharacterPtr->GetInteractiveSkillComponent()->GetSkills();
	TArray<FName>Ary
	{
		FPawnStateActionHUD::Get().ActiveSkill1,
		FPawnStateActionHUD::Get().ActiveSkill2,
		FPawnStateActionHUD::Get().ActiveSkill3,
		FPawnStateActionHUD::Get().ActiveSkill4 ,
	};

	for (const auto& Iter : Ary)
	{
		auto SkillIcon = Cast<UActionSkillsIcon>(GetWidgetFromName(Iter));
		if (SkillIcon)
		{
			auto SocketIter = SkillsMap.Find(SkillIcon->IconSocket);
			if (SocketIter)
			{
				SkillIcon->ResetToolUIByData((*SocketIter)->SkillUnit);
			}
			else
			{
				SkillIcon->ResetToolUIByData(nullptr);
			}
		}
	}
	auto EICPtr = CharacterPtr->GetInteractiveSkillComponent();

	ActivedWeaponChangedDelegate = EICPtr->OnActivedWeaponChangedContainer.AddCallback(
		std::bind(&ThisClass::OnActivedWeaponChanged, this, std::placeholders::_1)
	);
	OnActivedWeaponChanged(EICPtr->GetActivedWeaponType());
}

void UPawnStateActionHUD::OnActivedWeaponChanged(EWeaponSocket WeaponSocket)
{
	if (!CharacterPtr)
	{
		return;
	}

	auto EICPtr = CharacterPtr->GetInteractiveSkillComponent();

	TSharedPtr<FWeaponSocketInfo > FirstWeaponSocketInfoSPtr;
	TSharedPtr<FWeaponSocketInfo > SecondWeaponSocketInfoSPtr;
	switch (WeaponSocket)
	{
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
		auto IconPtr = Cast<UActionSkillsIcon>(GetWidgetFromName(FPawnStateActionHUD::Get().WeaponActiveSkill1));
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
		auto IconPtr = Cast<UActionSkillsIcon>(GetWidgetFromName(FPawnStateActionHUD::Get().WeaponActiveSkill2));
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
