
#include "CharacterAttibutes.h"

#include "AssetRefMap.h"
#include "FightingTips.h"
#include "Planet.h"

FBaseProperty::FBaseProperty()
{
}

int32 FBaseProperty::GetCurrentValue() const
{
	return CurrentValue;
}

void FBaseProperty::SetCurrentValue(int32 val)
{
	if (CurrentValue != val)
	{
		if (bIsSaveUpdate)
		{
		}
		else
		{
			CallbackContainerHelper.ValueChanged(CurrentValue, val);
		}

		CurrentValue = val;
	}
}

void FBaseProperty::AddCurrentValue(int32 val)
{
	SetCurrentValue(GetCurrentValue() + val);
}

void FBasePropertySet::AddCurrentValue(int32 val)
{
	const auto NewVal = CurrentValue.GetCurrentValue() + val;

	CurrentValue.SetCurrentValue(FMath::Clamp(NewVal, MinValue.GetCurrentValue(), MaxValue.GetCurrentValue()));
}

int32 FBasePropertySet::GetCurrentValue() const
{
	return CurrentValue.GetCurrentValue();
}

FBaseProperty& FBasePropertySet::GetCurrentProperty()
{
	return CurrentValue;
}

int32 FBasePropertySet::GetMaxValue() const
{
	return MaxValue.GetCurrentValue();
}

FBaseProperty& FBasePropertySet::GetMaxProperty()
{
	return MaxValue;
}

FBaseProperty& FBasePropertySet::GetMinProperty()
{
	return MinValue;
}

FCharacterAttributes::FCharacterAttributes()
{
	LiDao.GetCurrentProperty().SetCurrentValue(0);
	LiDao.GetMaxProperty().SetCurrentValue(6);
	
	GenGu.GetCurrentProperty().SetCurrentValue(0);
	GenGu.GetMaxProperty().SetCurrentValue(6);
	
	ShenFa.GetCurrentProperty().SetCurrentValue(0);
	ShenFa.GetMaxProperty().SetCurrentValue(6);
	
	DongCha.GetCurrentProperty().SetCurrentValue(0);
	DongCha.GetMaxProperty().SetCurrentValue(6);

	TianZi.GetCurrentProperty().SetCurrentValue(0);
	TianZi.GetMaxProperty().SetCurrentValue(6);
	
	Element.GoldElement.GetCurrentProperty().SetCurrentValue(0);
	Element.GoldElement.GetMaxProperty().SetCurrentValue(9);
	Element.GoldElement.WuXingType = EWuXingType::kGold;
	
	Element.WoodElement.GetCurrentProperty().SetCurrentValue(0);
	Element.WoodElement.GetMaxProperty().SetCurrentValue(9);
	Element.WoodElement.WuXingType = EWuXingType::kWood;
	
	Element.WaterElement.GetCurrentProperty().SetCurrentValue(0);
	Element.WaterElement.GetMaxProperty().SetCurrentValue(9);
	Element.WaterElement.WuXingType = EWuXingType::kWater;
	
	Element.FireElement.GetCurrentProperty().SetCurrentValue(0);
	Element.FireElement.GetMaxProperty().SetCurrentValue(9);
	Element.FireElement.WuXingType = EWuXingType::kFire;
	
	Element.SoilElement.GetCurrentProperty().SetCurrentValue(0);
	Element.SoilElement.GetMaxProperty().SetCurrentValue(9);
	Element.SoilElement.WuXingType = EWuXingType::kSoil;

	BaseAttackPower.GetCurrentProperty().SetCurrentValue(100);
	BaseAttackPower.GetMaxProperty().SetCurrentValue(3000);

	Penetration.GetCurrentProperty().SetCurrentValue(0);
	Penetration.GetMaxProperty().SetCurrentValue(1000);

	PercentPenetration.GetCurrentProperty().SetCurrentValue(0);
	PercentPenetration.GetMaxProperty().SetCurrentValue(100);
	
	Resistance.GetCurrentProperty().SetCurrentValue(20);
	Resistance.GetMaxProperty().SetCurrentValue(1000);
	
	GAPerformSpeed.GetCurrentProperty().SetCurrentValue(100);
	GAPerformSpeed.GetMaxProperty().SetCurrentValue(500);
	
	HP.GetCurrentProperty().SetCurrentValue(100);
	HP.GetMaxProperty().SetCurrentValue(5000);
	
	HPReplay.GetCurrentProperty().SetCurrentValue(1);
	HPReplay.GetMaxProperty().SetCurrentValue(1000);
	
	PP.GetCurrentProperty().SetCurrentValue(100);
	PP.GetMaxProperty().SetCurrentValue(1000);
	
	PPReplay.GetCurrentProperty().SetCurrentValue(1);
	PPReplay.GetMaxProperty().SetCurrentValue(1000);
	
	Evade.GetCurrentProperty().SetCurrentValue(20);
	Evade.GetMaxProperty().SetCurrentValue(100);
	
	HitRate.GetCurrentProperty().SetCurrentValue(50);
	HitRate.GetMaxProperty().SetCurrentValue(100);
	
	Toughness.GetCurrentProperty().SetCurrentValue(0);
	Toughness.GetMaxProperty().SetCurrentValue(100);
	
	CriticalHitRate.GetCurrentProperty().SetCurrentValue(0);
	CriticalHitRate.GetMaxProperty().SetCurrentValue(100);
	
	CriticalDamage.GetCurrentProperty().SetCurrentValue(50);
	CriticalDamage.GetMaxProperty().SetCurrentValue(100);
	
	WalkingSpeed.GetCurrentProperty().SetCurrentValue(250);
	WalkingSpeed.GetMaxProperty().SetCurrentValue(800);
	
	RunningSpeedOffset.GetCurrentProperty().SetCurrentValue(100);
	RunningSpeedOffset.GetMaxProperty().SetCurrentValue(100);
	
	RunningConsume.GetCurrentProperty().SetCurrentValue(3);
	RunningConsume.GetMaxProperty().SetCurrentValue(100);
}

FCharacterAttributes::~FCharacterAttributes()
{

}

void FCharacterAttributes::ProcessGAEVent(const FGameplayAbilityTargetData_GAEvent& GAEvent)
{
	// 处理数据
	if (GAEvent.Data.HitRate <= 0)
	{
	}
	else
	{
		float CurCriticalDamage = 1.f;
		if (GAEvent.Data.CriticalHitRate >= 100)
		{
			CurCriticalDamage = (100 + GAEvent.Data.CriticalDamage) / 100.f;
		}

		FScoped_BaseProperty_SaveUpdate Scoped_BaseProperty_SaveUpdate(HP.GetCurrentProperty());
		HP.AddCurrentValue(GAEvent.Data.TreatmentVolume);

		if (GAEvent.Data.ElementSet.IsEmpty())
		{
			HP.AddCurrentValue(-GAEvent.Data.BaseDamage * CurCriticalDamage);
		}
		else
		{
			for (const auto& Iter : GAEvent.Data.ElementSet)
			{
				HP.AddCurrentValue(-Iter.Get<2>() * CurCriticalDamage);
			}
		}

		HP.AddCurrentValue(GAEvent.Data.TrueDamage);
	}

	// 显示对应的浮动UI
	if (GAEvent.TargetActorAry.IsValidIndex(0))
	{
		auto UIPtr = CreateWidget<UFightingTips>(GetWorldImp(), UAssetRefMap::GetInstance()->FightingTipsClass);
		UIPtr->ProcessGAEVent(GAEvent);
		UIPtr->AddToViewport();
	}
}

FScopeCharacterAttributes::FScopeCharacterAttributes(FCharacterAttributes& CharacterAttributes)
{

}
