
#include "CharacterAttibutes.h"

#include "AssetRefMap.h"
#include "FightingTips.h"
#include "Planet.h"
#include "UICommon.h"
#include "GameplayTagsSubSystem.h"
#include "LogWriter.h"

static TAutoConsoleVariable<int32> DebugPrintCAB(
	TEXT("DebugPrintCAB"),
	0,
	TEXT("")
	TEXT(" default: 0"));

void FBasePropertySet::AddCurrentValue(int32 NewValue, const FGameplayTag& DataSource)
{
	if (ValueMap.Contains(DataSource))
	{
		ValueMap[DataSource] += NewValue;
	}
	else
	{
		ValueMap.Add(DataSource, NewValue);
	}
	Update();
}

void FBasePropertySet::SetCurrentValue(int32 NewValue, const FGameplayTag& DataSource)
{
	ValueMap.Add(DataSource, NewValue);
	Update();
}

void FBasePropertySet::RemoveCurrentValue(const FGameplayTag& DataSource)
{
	ValueMap.Remove(DataSource);
	Update();
}

int32 FBasePropertySet::GetCurrentValue() const
{
	return CurrentValue.GetCurrentValue();
}

const FBaseProperty& FBasePropertySet::GetCurrentProperty()const
{
	return CurrentValue;
}

FBaseProperty& FBasePropertySet::GetCurrentProperty()
{
	return CurrentValue;
}

int32 FBasePropertySet::GetMaxValue() const
{
	return MaxValue.GetCurrentValue();
}

const FBaseProperty& FBasePropertySet::GetMaxProperty()const
{
	return MaxValue;
}

void FBasePropertySet::Update()
{
	int32 TatolValue = 0;
	for (const auto& Iter : ValueMap)
	{
		TatolValue += Iter.Value;
	}

	CurrentValue.SetCurrentValue(FMath::Clamp(TatolValue, MinValue.GetCurrentValue(), MaxValue.GetCurrentValue()));
}

const FBaseProperty& FBasePropertySet::GetMinProperty()const
{
	return MinValue;
}

FBaseProperty& FBasePropertySet::GetMaxProperty()
{
	return MaxValue;
}

FCharacterAttributes::FCharacterAttributes()
{
	InitialData();
}

FCharacterAttributes::~FCharacterAttributes()
{

}

void FCharacterAttributes::InitialData()
{
	Name = TEXT("Player");

	LiDao.GetMaxProperty().SetCurrentValue(6);

	GenGu.GetMaxProperty().SetCurrentValue(6);

	ShenFa.GetMaxProperty().SetCurrentValue(6);

	DongCha.GetMaxProperty().SetCurrentValue(6);

	TianZi.GetMaxProperty().SetCurrentValue(6);

	Element.GoldElement.GetMaxProperty().SetCurrentValue(9);
	Element.GoldElement.WuXingType = EWuXingType::kGold;

	Element.WoodElement.GetMaxProperty().SetCurrentValue(9);
	Element.WoodElement.WuXingType = EWuXingType::kWood;

	Element.WaterElement.GetMaxProperty().SetCurrentValue(9);
	Element.WaterElement.WuXingType = EWuXingType::kWater;

	Element.FireElement.GetMaxProperty().SetCurrentValue(9);
	Element.FireElement.WuXingType = EWuXingType::kFire;

	Element.SoilElement.GetMaxProperty().SetCurrentValue(9);
	Element.SoilElement.WuXingType = EWuXingType::kSoil;

	const auto DataSource = FGameplayTag::RequestGameplayTag(FName(TEXT("DataSource.Character")));
	//UGameplayTagsSubSystem::GetInstance()->DataSource_Character;

	BaseAttackPower.GetMaxProperty().SetCurrentValue(3000);
	BaseAttackPower.SetCurrentValue(100, DataSource);

	Penetration.GetMaxProperty().SetCurrentValue(1000);
	Penetration.SetCurrentValue(0, DataSource);

	PercentPenetration.GetMaxProperty().SetCurrentValue(100);
	PercentPenetration.SetCurrentValue(0, DataSource);

	Resistance.GetMaxProperty().SetCurrentValue(1000);
	Resistance.SetCurrentValue(20, DataSource);

	GAPerformSpeed.GetMaxProperty().SetCurrentValue(500);
	GAPerformSpeed.SetCurrentValue(100, DataSource);

	HP.GetMaxProperty().SetCurrentValue(5000);
	HP.SetCurrentValue(100, DataSource);

	HPReplay.GetMaxProperty().SetCurrentValue(1000);
	HPReplay.SetCurrentValue(1, DataSource);

	PP.GetMaxProperty().SetCurrentValue(1000);
	PP.SetCurrentValue(100, DataSource);

	PPReplay.GetMaxProperty().SetCurrentValue(1000);
	PPReplay.SetCurrentValue(1, DataSource);

	Evade.GetMaxProperty().SetCurrentValue(100);
	Evade.SetCurrentValue(20, DataSource);

	HitRate.GetMaxProperty().SetCurrentValue(100);
	HitRate.SetCurrentValue(50, DataSource);

	Toughness.GetMaxProperty().SetCurrentValue(100);
	Toughness.SetCurrentValue(0, DataSource);

	CriticalHitRate.GetMaxProperty().SetCurrentValue(100);
	CriticalHitRate.SetCurrentValue(0, DataSource);

	CriticalDamage.GetMaxProperty().SetCurrentValue(100);
	CriticalDamage.SetCurrentValue(50, DataSource);

	MoveSpeed.GetMaxProperty().SetCurrentValue(800);
	MoveSpeed.SetCurrentValue(250, DataSource);

	RunningSpeedOffset.GetMaxProperty().SetCurrentValue(100);
	RunningSpeedOffset.SetCurrentValue(100, DataSource);

	RunningConsume.GetMaxProperty().SetCurrentValue(100);
	RunningConsume.SetCurrentValue(3, DataSource);
}

void FCharacterAttributes::ProcessGAEVent(const FGameplayAbilityTargetData_GAReceivedEvent& GAEvent)
{
	// 处理数据
	const auto& Ref = GAEvent.Data;

	if (Ref.HitRate <= 0)
	{
		// 未命中
	}

	// 会心伤害
	float CurCriticalDamage = 1.f;
	if (Ref.CriticalHitRate >= 100)
	{
		CurCriticalDamage = (100 + Ref.CriticalDamage) / 100.f;
	}

	// HP
	{
		FScoped_BaseProperty_SaveUpdate Scoped_BaseProperty_SaveUpdate(HP.GetCurrentProperty());
		HP.AddCurrentValue(Ref.HP, Ref.DataSource);

		if (Ref.HitRate > 0)
		{
			if (Ref.ElementSet.IsEmpty())
			{
				HP.AddCurrentValue(-Ref.BaseDamage * CurCriticalDamage, Ref.DataSource);
			}
			else
			{
				for (const auto& Iter : Ref.ElementSet)
				{
					HP.AddCurrentValue(-Iter.Get<2>() * CurCriticalDamage, Ref.DataSource);
				}
			}

			HP.AddCurrentValue(Ref.TrueDamage, Ref.DataSource);
		}
	}

	// PP
	{
		FScoped_BaseProperty_SaveUpdate Scoped_BaseProperty_SaveUpdate(PP.GetCurrentProperty());
		PP.AddCurrentValue(Ref.PP, Ref.DataSource);

		if (Ref.HitRate > 0)
		{
		}
	}

	// 
	{
		for (const auto Iter : Ref.DataModify)
		{
			switch (Iter.Key)
			{
			case ECharacterPropertyType::GAPerformSpeed:
			{
				if (Ref.bIsClearData)
				{
					GAPerformSpeed.RemoveCurrentValue(Ref.DataSource);
				}
				else
				{
					GAPerformSpeed.AddCurrentValue(Iter.Value.GetCurrentValue(), Ref.DataSource);
				}
			}
			break;
			case ECharacterPropertyType::MoveSpeed:
			{
				if (Ref.bIsClearData)
				{
					MoveSpeed.RemoveCurrentValue(Ref.DataSource);
				}
				else
				{
					MoveSpeed.AddCurrentValue(Iter.Value.GetCurrentValue(), Ref.DataSource);
				}
			}
			break;
			}
		}
	}
#ifdef WITH_EDITOR
	if (DebugPrintCAB.GetValueOnGameThread())
	{
		PRINTINVOKEWITHSTR(
			FString::Printf(TEXT("MoveSpeed %d GAPerformSpeed %d"),
			MoveSpeed.GetCurrentValue(),
			GAPerformSpeed.GetCurrentValue()
			));
	}
#endif

	ProcessedGAEvent(GAEvent);
}

const FElement& FCharacterAttributes::GetElement() const
{
	return Element;
}

const FBasePropertySet& FCharacterAttributes::GetHPReply() const
{
	return HPReplay;
}

const FBasePropertySet& FCharacterAttributes::GetPPReply() const
{
	return PPReplay;
}

FScopeCharacterAttributes::FScopeCharacterAttributes(FCharacterAttributes& CharacterAttributesSPtr)
{

}
