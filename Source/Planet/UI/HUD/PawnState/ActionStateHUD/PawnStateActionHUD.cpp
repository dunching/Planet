#include "PawnStateActionHUD.h"

#include "PawnStateActionHUD.h"

#include "Kismet/GameplayStatics.h"
#include "Components/HorizontalBox.h"
#include "Components/Border.h"

#include "PlanetPlayerState.h"
#include "Planet.h"
#include "CharacterBase.h"
#include "ProxyProcessComponent.h"
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

	const FName AD = TEXT("AD");

	const FName AD_Penetration = TEXT("AD_Penetration");

	const FName AD_Resistance = TEXT("AD_Resistance");

	const FName MoveSpeed = TEXT("MoveSpeed");

	const FName GAPerformSpeed = TEXT("GAPerformSpeed");

	const FName HP = TEXT("HP");

	const FName PP = TEXT("PP");

	const FName Mana = TEXT("Mana");

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

	BindEvent();
	ResetUIByData();
}

void UPawnStateActionHUD::NativeDestruct()
{
	for (auto Iter : OnAllocationSkillChangedDelegateAry)
	{
		Iter->UnBindCallback();
	}
	
	if (OnCanAciveSkillChangedHandle)
	{
		OnCanAciveSkillChangedHandle->UnBindCallback();
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
		auto CharacterAttributes = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
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
			auto UIPtr = Cast<UMyProgressBar>(GetWidgetFromName(FPawnStateActionHUD::Get().Mana));
			if (!UIPtr)
			{
				return;
			}
			UIPtr->SetDataSource(CharacterAttributes.Mana);
		}
		{
			auto UIPtr = Cast<UMyProgressBar>(GetWidgetFromName(FPawnStateActionHUD::Get().Shield));
			if (!UIPtr)
			{
				return;
			}
			UIPtr->SetDataSource(CharacterAttributes.Shield);
		}
		{
			auto UIPtr = Cast<UMyBaseProperty>(GetWidgetFromName(FPawnStateActionHUD::Get().AD));
			if (!UIPtr)
			{
				return;
			}
			UIPtr->SetDataSource(CharacterAttributes.AD);
		}
		{
			auto UIPtr = Cast<UMyBaseProperty>(GetWidgetFromName(FPawnStateActionHUD::Get().AD_Penetration));
			if (!UIPtr)
			{
				return;
			}
			UIPtr->SetDataSource(CharacterAttributes.AD_Penetration, CharacterAttributes.AD_PercentPenetration);
		}
		{
			auto UIPtr = Cast<UMyBaseProperty>(GetWidgetFromName(FPawnStateActionHUD::Get().AD_Resistance));
			if (!UIPtr)
			{
				return;
			}
			UIPtr->SetDataSource(CharacterAttributes.AD_Resistance);
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
			UIPtr->SetDataSource(CharacterAttributes.GoldElement);
		}
		{
			auto UIPtr = Cast<UMyBaseProperty>(GetWidgetFromName(FPawnStateActionHUD::Get().Wood));
			if (!UIPtr)
			{
				return;
			}
			UIPtr->SetDataSource(CharacterAttributes.WoodElement);
		}
		{
			auto UIPtr = Cast<UMyBaseProperty>(GetWidgetFromName(FPawnStateActionHUD::Get().Water));
			if (!UIPtr)
			{
				return;
			}
			UIPtr->SetDataSource(CharacterAttributes.WaterElement);
		}
		{
			auto UIPtr = Cast<UMyBaseProperty>(GetWidgetFromName(FPawnStateActionHUD::Get().Fire));
			if (!UIPtr)
			{
				return;
			}
			UIPtr->SetDataSource(CharacterAttributes.FireElement);
		}
		{
			auto UIPtr = Cast<UMyBaseProperty>(GetWidgetFromName(FPawnStateActionHUD::Get().Soil));
			if (!UIPtr)
			{
				return;
			}
			UIPtr->SetDataSource(CharacterAttributes.SoilElement);
		}
	}

	InitialTalentUI();
	InitialActiveSkillIcon();
	InitialWeaponSkillIcon();
}

void UPawnStateActionHUD::BindEvent()
{
	if (!CharacterPtr)
	{
		return;
	}

	OnAllocationSkillChangedDelegateAry.Add(CharacterPtr->GetProxyProcessComponent()->OnCurrentWeaponChanged.AddCallback(
		std::bind(&ThisClass::InitialActiveSkillIcon, this)
	));

	OnAllocationSkillChangedDelegateAry.Add(CharacterPtr->GetProxyProcessComponent()->OnCurrentWeaponChanged.AddCallback(
		std::bind(&ThisClass::InitialWeaponSkillIcon, this)
	));

	OnCanAciveSkillChangedHandle = CharacterPtr->GetProxyProcessComponent()->OnCanAciveSkillChanged.AddCallback(
		std::bind(&ThisClass::InitialActiveSkillIcon, this)
	);
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
	const auto& SkillsMap = CharacterPtr->GetProxyProcessComponent()->GetAllSocket();
	for (auto Iter : SkillsMap)
	{
		bool bIsGiveTalentPassive = false;
		if (
			Iter.Value->ProxySPtr
			)
		{
			if (Iter.Value->ProxySPtr->GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Talent_NuQi))
			{
				auto UIPtr = CreateWidget<UState_Talent_NuQi>(this, State_Talent_NuQi_Class);
				if (UIPtr)
				{
					BorderPtr->AddChild(UIPtr);
					bIsGiveTalentPassive = true;
				}
			}
			else if (Iter.Value->ProxySPtr->GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Talent_YinYang))
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

void UPawnStateActionHUD::InitialActiveSkillIcon()
{
	if (!CharacterPtr)
	{
		return;
	}

	auto SkillsMap = CharacterPtr->GetProxyProcessComponent()->GetCanbeActiveSkills();
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
				SkillIcon->ResetToolUIByData((*SocketIter)->ProxySPtr);
			}
			else
			{
				SkillIcon->ResetToolUIByData(nullptr);
			}
		}
	}
}

void UPawnStateActionHUD::InitialWeaponSkillIcon()
{
	if (!CharacterPtr)
	{
		return;
	}

	TSharedPtr<FSocket_FASI> FirstWeaponSocketInfoSPtr;
	TSharedPtr<FSocket_FASI> SecondWeaponSocketInfoSPtr;

	const auto CurrentWeaponSocket = CharacterPtr->GetProxyProcessComponent()->CurrentWeaponSocket;

	if (
		CurrentWeaponSocket ==
		UGameplayTagsSubSystem::GetInstance()->WeaponSocket_1
		)
	{
		CharacterPtr->GetProxyProcessComponent()->GetWeapon(FirstWeaponSocketInfoSPtr, SecondWeaponSocketInfoSPtr);
	}
	else if (
		CurrentWeaponSocket ==
		UGameplayTagsSubSystem::GetInstance()->WeaponSocket_2
		)
	{
		CharacterPtr->GetProxyProcessComponent()->GetWeapon(SecondWeaponSocketInfoSPtr, FirstWeaponSocketInfoSPtr);
	}
	else
	{
		return;
	}

	{
		auto IconPtr = Cast<UActionSkillsIcon>(GetWidgetFromName(FPawnStateActionHUD::Get().WeaponActiveSkill1));
		if (IconPtr)
		{
			IconPtr->ResetToolUIByData(
				FirstWeaponSocketInfoSPtr && FirstWeaponSocketInfoSPtr->ProxySPtr ?
				DynamicCastSharedPtr<FWeaponProxy>(FirstWeaponSocketInfoSPtr->ProxySPtr)->GetWeaponSkill() :
				nullptr
			);
		}
	}
	{
		auto IconPtr = Cast<UActionSkillsIcon>(GetWidgetFromName(FPawnStateActionHUD::Get().WeaponActiveSkill2));
		if (IconPtr)
		{
			IconPtr->ResetToolUIByData(
				SecondWeaponSocketInfoSPtr && SecondWeaponSocketInfoSPtr->ProxySPtr ?
				DynamicCastSharedPtr<FWeaponProxy>(SecondWeaponSocketInfoSPtr->ProxySPtr)->GetWeaponSkill() :
				nullptr
			);
		}
	}
}
