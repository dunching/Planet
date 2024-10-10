
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

const FGameplayTag DataSource_Character = FGameplayTag::RequestGameplayTag(FName(TEXT("DataSource.Character")));

FBasePropertySet::FBasePropertySet()
{
	PropertySettlementModifySet.emplace(MakeShared<FPropertySettlementModify>());
}

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
	check(PropertySettlementModifySet.size() > 0);

	// 人物身上自带的属性
	auto Character_Value_Iter = ValueMap.Find(DataSource_Character);
	if (Character_Value_Iter)
	{
		*Character_Value_Iter = FMath::Clamp(*Character_Value_Iter, MinValue.GetCurrentValue(), MaxValue.GetCurrentValue());
	}

	for (const auto& Iter : PropertySettlementModifySet)
	{
		const auto& Value = Iter->SettlementModify(ValueMap);

		CurrentValue.SetCurrentValue(FMath::Clamp(Value, MinValue.GetCurrentValue(), MaxValue.GetCurrentValue()));
		return;
	}
}

const FBaseProperty& FBasePropertySet::GetMinProperty()const
{
	return MinValue;
}

int32 FBasePropertySet::AddSettlementModify(const TSharedPtr<FPropertySettlementModify>& PropertySettlementModify)
{
	if (PropertySettlementModify)
	{
		PropertySettlementModifySet.emplace(PropertySettlementModify);

		Update();

		return PropertySettlementModify->ID;
	}

	return -1;
}

int32 FBasePropertySet::UpdateSettlementModify(const TSharedPtr<FPropertySettlementModify>& PropertySettlementModify)
{
	if (PropertySettlementModify)
	{
		Update();

		return PropertySettlementModify->ID;
	}

	return -1;
}

void FBasePropertySet::RemoveSettlementModify(const TSharedPtr<FPropertySettlementModify>& PropertySettlementModify)
{
	if (PropertySettlementModify)
	{
		PropertySettlementModifySet.erase(PropertySettlementModify);

		Update();
	}
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

bool FCharacterAttributes::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	if (Ar.IsSaving())
	{
		Ar << Name;
		Ar << Level;

		Ar << LiDao.CurrentValue.CurrentValue;
		Ar << GenGu.CurrentValue.CurrentValue;
		Ar << ShenFa.CurrentValue.CurrentValue;
		Ar << DongCha.CurrentValue.CurrentValue;
		Ar << TianZi.CurrentValue.CurrentValue;

		Ar << HP.CurrentValue.CurrentValue;
		Ar << PP.CurrentValue.CurrentValue;

		Ar << AD.CurrentValue.CurrentValue;
		Ar << AD_Penetration.CurrentValue.CurrentValue;
		Ar << GAPerformSpeed.CurrentValue.CurrentValue;
		Ar << MoveSpeed.CurrentValue.CurrentValue;
	}
	else if (Ar.IsLoading())
	{
		auto Lambda = [&](FBaseProperty& Value)
			{
				int32 TempValue;
				Ar << TempValue;
				if (TempValue != Value.CurrentValue)
				{
					Value.SetCurrentValue(TempValue);
				}
			};

		Ar << Name;
		Ar << Level;

		Lambda(LiDao.CurrentValue);
		Lambda(GenGu.CurrentValue);
		Lambda(ShenFa.CurrentValue);
		Lambda(DongCha.CurrentValue);
		Lambda(TianZi.CurrentValue);

		Lambda(HP.CurrentValue);
		Lambda(PP.CurrentValue);

		Lambda(AD.CurrentValue);
		Lambda(AD_Penetration.CurrentValue);
		Lambda(GAPerformSpeed.CurrentValue);
		Lambda(MoveSpeed.CurrentValue);

		bIsNotChanged = true;
	}

	return true;
}

void FCharacterAttributes::InitialData()
{
	Name = TEXT("Player");

	LiDao.GetMaxProperty().SetCurrentValue(6);

	GenGu.GetMaxProperty().SetCurrentValue(6);

	ShenFa.GetMaxProperty().SetCurrentValue(6);

	DongCha.GetMaxProperty().SetCurrentValue(6);

	TianZi.GetMaxProperty().SetCurrentValue(6);

	GoldElement.GetMaxProperty().SetCurrentValue(9);

	WoodElement.GetMaxProperty().SetCurrentValue(9);

	WaterElement.GetMaxProperty().SetCurrentValue(9);

	FireElement.GetMaxProperty().SetCurrentValue(9);

	SoilElement.GetMaxProperty().SetCurrentValue(9);

	AD.GetMaxProperty().SetCurrentValue(3000);
	AD.SetCurrentValue(100, DataSource_Character);

	AD_Penetration.GetMaxProperty().SetCurrentValue(1000);
	AD_Penetration.SetCurrentValue(0, DataSource_Character);

	AD_PercentPenetration.GetMaxProperty().SetCurrentValue(100);
	AD_PercentPenetration.SetCurrentValue(0, DataSource_Character);

	AD_Resistance.GetMaxProperty().SetCurrentValue(1000);
	AD_Resistance.SetCurrentValue(20, DataSource_Character);

	GAPerformSpeed.GetMaxProperty().SetCurrentValue(500);
	GAPerformSpeed.SetCurrentValue(100, DataSource_Character);

	Shield.GetMaxProperty().SetCurrentValue(200);
	Shield.SetCurrentValue(0, DataSource_Character);

	HP.GetMaxProperty().SetCurrentValue(200);
	HP.SetCurrentValue(200, DataSource_Character);

	HP_Replay.GetMaxProperty().SetCurrentValue(1000);
	HP_Replay.SetCurrentValue(1, DataSource_Character);

	PP.GetMaxProperty().SetCurrentValue(100);
	PP.SetCurrentValue(100, DataSource_Character);

	PP_Replay.GetMaxProperty().SetCurrentValue(1000);
	PP_Replay.SetCurrentValue(1, DataSource_Character);

	Mana.GetMaxProperty().SetCurrentValue(500);
	Mana.SetCurrentValue(500, DataSource_Character);

	Mana_Replay.GetMaxProperty().SetCurrentValue(1000);
	Mana_Replay.SetCurrentValue(1, DataSource_Character);
	
	Evade.GetMaxProperty().SetCurrentValue(100);
	Evade.SetCurrentValue(20, DataSource_Character);

	HitRate.GetMaxProperty().SetCurrentValue(100);
	HitRate.SetCurrentValue(50, DataSource_Character);

	Toughness.GetMaxProperty().SetCurrentValue(100);
	Toughness.SetCurrentValue(0, DataSource_Character);

	CriticalHitRate.GetMaxProperty().SetCurrentValue(100);
	CriticalHitRate.SetCurrentValue(0, DataSource_Character);

	CriticalDamage.GetMaxProperty().SetCurrentValue(100);
	CriticalDamage.SetCurrentValue(50, DataSource_Character);

	MoveSpeed.GetMaxProperty().SetCurrentValue(800);
	MoveSpeed.SetCurrentValue(250, DataSource_Character);
}

void FCharacterAttributes::ProcessGAEVent(const FGameplayAbilityTargetData_GAReceivedEvent& GAEvent)
{
	ON_SCOPE_EXIT
	{
	bIsNotChanged = false;
	};

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
		FScoped_BaseProperty_SaveUpdate HP_Scope(HP.GetCurrentProperty());
		FScoped_BaseProperty_SaveUpdate PP_Scope(PP.GetCurrentProperty());

		if (Ref.HitRate > 0)
		{
			if (Ref.ElementSet.IsEmpty())
			{
				// 基础伤害
				HP.AddCurrentValue(-Ref.BaseDamage * CurCriticalDamage, Ref.DataSource);
			}
			else
			{
				// 元素伤害
				for (const auto& Iter : Ref.ElementSet)
				{
					HP.AddCurrentValue(-Iter.Get<2>() * CurCriticalDamage, Ref.DataSource);
				}
			}

			// 真实伤害
			HP.AddCurrentValue(Ref.TrueDamage, Ref.DataSource);
		}

		{
			auto Lambda = [&Ref](
				FBasePropertySet& PropertySetRef,
				const FBaseProperty& Property
				)
				{
					if (Ref.bIsClearData)
					{
						PropertySetRef.RemoveCurrentValue(Ref.DataSource);
					}
					else if (Ref.bIsOverlapData)
					{
						PropertySetRef.SetCurrentValue(Property.GetCurrentValue(), Ref.DataSource);
					}
					else
					{
						PropertySetRef.AddCurrentValue(Property.GetCurrentValue(), Ref.DataSource);
					}
				};

			for (const auto Iter : Ref.DataModify)
			{
				switch (Iter.Key)
				{

#pragma region 
				case ECharacterPropertyType::GoldElement:
				{
					Lambda(GoldElement, Iter.Value);
				}
				break;
				case ECharacterPropertyType::WoodElement:
				{
					Lambda(WoodElement, Iter.Value);
				}
				break;
				case ECharacterPropertyType::WaterElement:
				{
					Lambda(WaterElement, Iter.Value);
				}
				break;
				case ECharacterPropertyType::FireElement:
				{
					Lambda(FireElement, Iter.Value);
				}
				break;
				case ECharacterPropertyType::SoilElement:
				{
					Lambda(SoilElement, Iter.Value);
				}
				break;
#pragma endregion

#pragma region 
				case ECharacterPropertyType::HP:
				{
					Lambda(HP, Iter.Value);
				}
				break;

				case ECharacterPropertyType::PP:
				{
					Lambda(PP, Iter.Value);
				}
				break;
				case ECharacterPropertyType::Mana:
				{
					Lambda(Mana, Iter.Value);
				}
				break;
#pragma endregion


#pragma region 
				case ECharacterPropertyType::AD:
				{
					Lambda(AD, Iter.Value);
				}
				break;

				case ECharacterPropertyType::AD_Penetration:
				{
					Lambda(AD_Penetration, Iter.Value);
				}
				break;
				case ECharacterPropertyType::AD_PercentPenetration:
				{
					Lambda(AD_PercentPenetration, Iter.Value);
				}
				break;
#pragma endregion

				case ECharacterPropertyType::GAPerformSpeed:
				{
					Lambda(GAPerformSpeed, Iter.Value);
				}
				break;
				case ECharacterPropertyType::MoveSpeed:
				{
					Lambda(MoveSpeed, Iter.Value);
				}
				break;
				}
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

bool FCharacterAttributes::Identical(const FCharacterAttributes* Other, uint32 PortFlags) const
{
	return Other->bIsNotChanged;
}

bool FCharacterAttributes::operator==(const FCharacterAttributes& RightValue) const
{
	return true;
}

const FBasePropertySet& FCharacterAttributes::GetHPReply() const
{
	return HP_Replay;
}

const FBasePropertySet& FCharacterAttributes::GetPPReply() const
{
	return PP_Replay;
}

FScopeCharacterAttributes::FScopeCharacterAttributes(FCharacterAttributes& CharacterAttributes)
{

}

FPropertySettlementModify::FPropertySettlementModify(int32 InPriority /*= 1*/) :
	Priority(InPriority)
{
	ID = FMath::Rand32();
}

int32 FPropertySettlementModify::SettlementModify(const TMap<FGameplayTag, int32>& ValueMap) const
{
	int32 TatolValue = 0;
	for (const auto& Iter : ValueMap)
	{
		TatolValue += Iter.Value;
	}
	return TatolValue;
}
