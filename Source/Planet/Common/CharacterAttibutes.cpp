
#include "CharacterAttibutes.h"

#include "AssetRefMap.h"
#include "FightingTips.h"
#include "Planet.h"
#include "UICommon.h"

void FBasePropertySet::AddCurrentValue(int32 NewValue, FGuid GUID)
{
	if (ValueMap.Contains(GUID))
	{
		ValueMap[GUID] += NewValue;
	}
	else
	{
		ValueMap.Add(GUID, NewValue);
	}
	Update();
}

void FBasePropertySet::SetCurrentValue(int32 NewValue, FGuid GUID)
{
	ValueMap.Add(GUID, NewValue);
	Update();
}

void FBasePropertySet::RemoveCurrentValue(FGuid GUID)
{
	ValueMap.Remove(GUID);
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

	BaseAttackPower.GetMaxProperty().SetCurrentValue(3000);
	BaseAttackPower.SetCurrentValue(100, PropertuModify_GUID);

	Penetration.GetMaxProperty().SetCurrentValue(1000);
	Penetration.SetCurrentValue(0, PropertuModify_GUID);

	PercentPenetration.GetMaxProperty().SetCurrentValue(100);
	PercentPenetration.SetCurrentValue(0, PropertuModify_GUID);

	Resistance.GetMaxProperty().SetCurrentValue(1000);
	Resistance.SetCurrentValue(20, PropertuModify_GUID);

	GAPerformSpeed.GetMaxProperty().SetCurrentValue(500);
	GAPerformSpeed.SetCurrentValue(100, PropertuModify_GUID);

	HP.GetMaxProperty().SetCurrentValue(5000);
	HP.SetCurrentValue(100, PropertuModify_GUID);

	HPReplay.GetMaxProperty().SetCurrentValue(1000);
	HPReplay.SetCurrentValue(1, PropertuModify_GUID);

	PP.GetMaxProperty().SetCurrentValue(1000);
	PP.SetCurrentValue(100, PropertuModify_GUID);

	PPReplay.GetMaxProperty().SetCurrentValue(1000);
	PPReplay.SetCurrentValue(1, PropertuModify_GUID);

	Evade.GetMaxProperty().SetCurrentValue(100);
	Evade.SetCurrentValue(20, PropertuModify_GUID);

	HitRate.GetMaxProperty().SetCurrentValue(100);
	HitRate.SetCurrentValue(50, PropertuModify_GUID);

	Toughness.GetMaxProperty().SetCurrentValue(100);
	Toughness.SetCurrentValue(0, PropertuModify_GUID);

	CriticalHitRate.GetMaxProperty().SetCurrentValue(100);
	CriticalHitRate.SetCurrentValue(0, PropertuModify_GUID);

	CriticalDamage.GetMaxProperty().SetCurrentValue(100);
	CriticalDamage.SetCurrentValue(50, PropertuModify_GUID);

	MoveSpeed.GetMaxProperty().SetCurrentValue(800);
	MoveSpeed.SetCurrentValue(250, PropertuModify_GUID);

	RunningSpeedOffset.GetMaxProperty().SetCurrentValue(100);
	RunningSpeedOffset.SetCurrentValue(100, PropertuModify_GUID);

	RunningConsume.GetMaxProperty().SetCurrentValue(100);
	RunningConsume.SetCurrentValue(3, PropertuModify_GUID);
}

FCharacterAttributes::~FCharacterAttributes()
{

}

void FCharacterAttributes::ProcessGAEVent(const FGameplayAbilityTargetData_GAReceivedEvent& GAEvent)
{
	// 处理数据
	const auto& Ref = GAEvent.Data;
	if (Ref.HitRate <= 0)
	{
	}
	else
	{
		float CurCriticalDamage = 1.f;
		if (Ref.CriticalHitRate >= 100)
		{
			CurCriticalDamage = (100 + Ref.CriticalDamage) / 100.f;
		}

		FScoped_BaseProperty_SaveUpdate Scoped_BaseProperty_SaveUpdate(HP.GetCurrentProperty());
		HP.AddCurrentValue(Ref.HP, PropertuModify_GUID);

		if (Ref.ElementSet.IsEmpty())
		{
			HP.AddCurrentValue(-Ref.BaseDamage * CurCriticalDamage, PropertuModify_GUID);
		}
		else
		{
			for (const auto& Iter : Ref.ElementSet)
			{
				HP.AddCurrentValue(-Iter.Get<2>() * CurCriticalDamage, PropertuModify_GUID);
			}
		}

		HP.AddCurrentValue(Ref.TrueDamage, PropertuModify_GUID);
	}

	// 显示对应的浮动UI
	auto UIPtr = CreateWidget<UFightingTips>(GetWorldImp(), UAssetRefMap::GetInstance()->FightingTipsClass);
	UIPtr->ProcessGAEVent(GAEvent);
	UIPtr->AddToViewport(EUIOrder::kFightingTips);
}

FScopeCharacterAttributes::FScopeCharacterAttributes(FCharacterAttributes& CharacterAttributes)
{

}
