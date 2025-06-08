#include "AS_Character.h"

#include <functional>

#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

#include "GameplayTagsLibrary.h"

void FMyGameplayAttributeData::SetBaseValue(
	float NewValue
	)
{
	Super::SetBaseValue(NewValue);
}

void UAS_Character::PreAttributeChange(
	const FGameplayAttribute& Attribute,
	float& NewValue
	)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void UAS_Character::PreAttributeBaseChange(
	const FGameplayAttribute& Attribute,
	float& NewValue
	) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
}

void UAS_Character::PostGameplayEffectExecute(
	const FGameplayEffectModCallbackData& Data
	)
{
	Super::PostGameplayEffectExecute(Data);

	// 通过使用属性获取器来查看这个调用是否会影响生命值。
	if (Data.EvaluatedData.Attribute == GetHPAttribute())
	{
		// 这个游戏玩法效果是改变生命值。应用它，但要先限制数值。
		// 在这种情况下，生命值的基础值不可是负值。
		SetHP(FMath::Clamp(GetHP(), 0.0f, GetMax_HP()));
	}
	else if (Data.EvaluatedData.Attribute == GetMoveSpeedAttribute())
	{
		SetMoveSpeed(GetMoveSpeed());
	}
}

void UAS_Character::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
	) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Max_HP, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, HP, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, HP_Replay, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Max_Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Stamina_Replay, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Max_Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Mana_Replay, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Level, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Experience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, LevelExperience, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION(ThisClass, MetalValue, COND_None);
	DOREPLIFETIME_CONDITION(ThisClass, MetalLevel, COND_None);
	DOREPLIFETIME_CONDITION(ThisClass, MetalPenetration, COND_None);
	DOREPLIFETIME_CONDITION(ThisClass, MetalPercentPenetration, COND_None);
	DOREPLIFETIME_CONDITION(ThisClass, MetalResistance, COND_None);

	DOREPLIFETIME_CONDITION(ThisClass, WoodValue, COND_None);
	DOREPLIFETIME_CONDITION(ThisClass, WoodLevel, COND_None);
	DOREPLIFETIME_CONDITION(ThisClass, WoodPenetration, COND_None);
	DOREPLIFETIME_CONDITION(ThisClass, WoodPercentPenetration, COND_None);
	DOREPLIFETIME_CONDITION(ThisClass, WoodResistance, COND_None);

	DOREPLIFETIME_CONDITION(ThisClass, WaterValue, COND_None);
	DOREPLIFETIME_CONDITION(ThisClass, WaterLevel, COND_None);
	DOREPLIFETIME_CONDITION(ThisClass, WaterPenetration, COND_None);
	DOREPLIFETIME_CONDITION(ThisClass, WaterPercentPenetration, COND_None);
	DOREPLIFETIME_CONDITION(ThisClass, WaterResistance, COND_None);

	DOREPLIFETIME_CONDITION(ThisClass, FireValue, COND_None);
	DOREPLIFETIME_CONDITION(ThisClass, FireLevel, COND_None);
	DOREPLIFETIME_CONDITION(ThisClass, FirePenetration, COND_None);
	DOREPLIFETIME_CONDITION(ThisClass, FirePercentPenetration, COND_None);
	DOREPLIFETIME_CONDITION(ThisClass, FireResistance, COND_None);

	DOREPLIFETIME_CONDITION(ThisClass, EarthValue, COND_None);
	DOREPLIFETIME_CONDITION(ThisClass, EarthLevel, COND_None);
	DOREPLIFETIME_CONDITION(ThisClass, EarthPenetration, COND_None);
	DOREPLIFETIME_CONDITION(ThisClass, EarthPercentPenetration, COND_None);
	DOREPLIFETIME_CONDITION(ThisClass, EarthResistance, COND_None);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MoveSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION(ThisClass, CriticalDamage, COND_None);
	DOREPLIFETIME_CONDITION(ThisClass, CriticalHitRate, COND_None);
	DOREPLIFETIME_CONDITION(ThisClass, HitRate, COND_None);
	DOREPLIFETIME_CONDITION(ThisClass, EvadeRate, COND_None);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, PerformSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION(ThisClass, Shield, COND_None);
}

void UAS_Character::ProcessForAttributeTag(
	const FGameplayTag& AttributeTag,
	const std::function<void(
		const FGameplayAttribute&,
		float 
		)>& Func
	)const
{
	if (AttributeTag.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_HP))
	{
		Func(GetHPAttribute(), GetHP());
	}
	else if (AttributeTag.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Stamina))
	{
		Func(GetStaminaAttribute(), GetStamina());
	}
	else if (AttributeTag.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Mana))
	{
		Func(GetManaAttribute(), GetMana());
	}

	else if (AttributeTag.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_MaxHP))
	{
		Func(GetMax_HPAttribute(), GetMax_HP());
	}
	else if (AttributeTag.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_MaxStamina))
	{
		Func(GetMax_StaminaAttribute(), GetMax_Stamina());
	}
	else if (AttributeTag.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_MaxMana))
	{
		Func(GetMax_ManaAttribute(), GetMax_Mana());
	}
	
	else if (AttributeTag.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_MoveSpeed))
	{
		Func(GetMoveSpeedAttribute(), GetMoveSpeed());
	}
	else if (AttributeTag.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_MoveSpeed))
	{
		Func(GetPerformSpeedAttribute(), GetPerformSpeed());
	}
	else if (AttributeTag.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_CriticalDamage))
	{
		Func(GetCriticalDamageAttribute(), GetCriticalDamage());
	}
	else if (AttributeTag.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_CriticalHitRate))
	{
		Func(GetCriticalHitRateAttribute(), GetCriticalHitRate());
	}
	else if (AttributeTag.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_HitRate))
	{
		Func(GetHitRateAttribute(), GetHitRate());
	}
	else if (AttributeTag.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_EvadeRate))
	{
		Func(GetEvadeRateAttribute(), GetEvadeRate());
	}
	
	else if (AttributeTag.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Shield))
	{
		Func(GetShieldAttribute(), GetShield());
	}

	else if (AttributeTag.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Metal_Value))
	{
		Func(GetMetalValueAttribute(), GetMetalValue());
	}
	else if (AttributeTag.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Metal_Level))
	{
		Func(GetMetalLevelAttribute(), GetMetalLevel());
	}
	else if (AttributeTag.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Metal_Penetration))
	{
		Func(GetMetalPenetrationAttribute(), GetMetalPenetration());
	}
	else if (AttributeTag.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Metal_PercentPenetration))
	{
		Func(GetMetalPercentPenetrationAttribute(), GetMetalPercentPenetration());
	}
	else if (AttributeTag.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Metal_Resistance))
	{
		Func(GetMetalResistanceAttribute(), GetMetalResistance());
	}

	else if (AttributeTag.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Wood_Value))
	{
		Func(GetWoodValueAttribute(), GetWoodValue());
	}
	else if (AttributeTag.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Wood_Level))
	{
		Func(GetWoodLevelAttribute(), GetWoodLevel());
	}
	else if (AttributeTag.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Wood_Penetration))
	{
		Func(GetWoodPenetrationAttribute(), GetWoodPenetration());
	}
	else if (AttributeTag.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Wood_PercentPenetration))
	{
		Func(GetWoodPercentPenetrationAttribute(), GetWoodPercentPenetration());
	}
	else if (AttributeTag.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Wood_Resistance))
	{
		Func(GetWoodResistanceAttribute(), GetWoodResistance());
	}

	else if (AttributeTag.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Water_Value))
	{
		Func(GetWaterValueAttribute(), GetWaterValue());
	}
	else if (AttributeTag.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Water_Level))
	{
		Func(GetWaterLevelAttribute(), GetWaterLevel());
	}
	else if (AttributeTag.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Water_Penetration))
	{
		Func(GetWaterPenetrationAttribute(), GetWaterPenetration());
	}
	else if (AttributeTag.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Water_PercentPenetration))
	{
		Func(GetWaterPercentPenetrationAttribute(), GetWaterPercentPenetration());
	}
	else if (AttributeTag.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Water_Resistance))
	{
		Func(GetWaterResistanceAttribute(), GetWaterResistance());
	}

	else if (AttributeTag.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Fire_Value))
	{
		Func(GetFireValueAttribute(), GetFireValue());
	}
	else if (AttributeTag.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Fire_Level))
	{
		Func(GetFireLevelAttribute(), GetFireLevel());
	}
	else if (AttributeTag.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Fire_Penetration))
	{
		Func(GetFirePenetrationAttribute(), GetFirePenetration());
	}
	else if (AttributeTag.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Fire_PercentPenetration))
	{
		Func(GetFirePercentPenetrationAttribute(), GetFirePercentPenetration());
	}
	else if (AttributeTag.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Fire_Resistance))
	{
		Func(GetFireResistanceAttribute(), GetFireResistance());
	}

	else if (AttributeTag.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Earth_Value))
	{
		Func(GetEarthValueAttribute(), GetEarthValue());
	}
	else if (AttributeTag.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Earth_Level))
	{
		Func(GetEarthLevelAttribute(), GetEarthLevel());
	}
	else if (AttributeTag.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Earth_Penetration))
	{
		Func(GetEarthPenetrationAttribute(), GetEarthPenetration());
	}
	else if (AttributeTag.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Earth_PercentPenetration))
	{
		Func(GetEarthPercentPenetrationAttribute(), GetEarthPercentPenetration());
	}
	else if (AttributeTag.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Earth_Resistance))
	{
		Func(GetEarthResistanceAttribute(), GetEarthResistance());
	}

}

void UAS_Character::OnRep_Max_HP(
	const FMyGameplayAttributeData& OldValue
	)
{
	// 使用默认的游戏玩法属性系统更新通知行为。
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Max_HP, OldValue);
}

void UAS_Character::OnRep_HP(
	const FMyGameplayAttributeData& OldValue
	)
{
	// 使用默认的游戏玩法属性系统更新通知行为。
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, HP, OldValue);
}

void UAS_Character::OnRep_HP_Replay(
	const FMyGameplayAttributeData& OldValue
	)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, HP_Replay, OldValue);
}

void UAS_Character::OnRep_Max_Stamina(
	const FMyGameplayAttributeData& OldValue
	)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Max_Stamina, OldValue);
}

void UAS_Character::OnRep_Stamina(
	const FMyGameplayAttributeData& OldValue
	)
{
	// 使用默认的游戏玩法属性系统更新通知行为。
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Stamina, OldValue);
}

void UAS_Character::OnRep_Stamina_Replay(
	const FMyGameplayAttributeData& OldValue
	)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Stamina_Replay, OldValue);
}

void UAS_Character::OnRep_Mana(
	const FMyGameplayAttributeData& OldValue
	)
{
	// 使用默认的游戏玩法属性系统更新通知行为。
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Mana, OldValue);
}

void UAS_Character::OnRep_Max_Mana(
	const FMyGameplayAttributeData& OldValue
	)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Max_Mana, OldValue);
}

void UAS_Character::OnRep_Mana_Replay(
	const FMyGameplayAttributeData& OldValue
	)
{
	// 使用默认的游戏玩法属性系统更新通知行为。
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Mana_Replay, OldValue);
}

void UAS_Character::OnRep_Experience(
	const FMyGameplayAttributeData& OldValue
	)
{
	// 使用默认的游戏玩法属性系统更新通知行为。
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Experience, OldValue);
}

void UAS_Character::OnRep_LevelExperience(
	const FMyGameplayAttributeData& OldValue
	)
{
	// 使用默认的游戏玩法属性系统更新通知行为。
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, LevelExperience, OldValue);
}

void UAS_Character::OnRep_MetalValue(
	const FMyGameplayAttributeData& OldValue
	)
{
	// 使用默认的游戏玩法属性系统更新通知行为。
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MetalValue, OldValue);
}

void UAS_Character::OnRep_MetalLevel(
	const FMyGameplayAttributeData& OldValue
	)
{
	// 使用默认的游戏玩法属性系统更新通知行为。
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MetalLevel, OldValue);
}

void UAS_Character::OnRep_MetalPenetration(
	const FMyGameplayAttributeData& OldValue
	)
{
	// 使用默认的游戏玩法属性系统更新通知行为。
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MetalPenetration, OldValue);
}

void UAS_Character::OnRep_MetalPercentPenetration(
	const FMyGameplayAttributeData& OldValue
	)
{
	// 使用默认的游戏玩法属性系统更新通知行为。
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MetalPercentPenetration, OldValue);
}

void UAS_Character::OnRep_MetalResistance(
	const FMyGameplayAttributeData& OldValue
	)
{
	// 使用默认的游戏玩法属性系统更新通知行为。
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MetalResistance, OldValue);
}

void UAS_Character::OnRep_WoodValue(
	const FMyGameplayAttributeData& OldValue
	)
{
	// 使用默认的游戏玩法属性系统更新通知行为。
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, WoodValue, OldValue);
}

void UAS_Character::OnRep_WoodLevel(
	const FMyGameplayAttributeData& OldValue
	)
{
	// 使用默认的游戏玩法属性系统更新通知行为。
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, WoodLevel, OldValue);
}

void UAS_Character::OnRep_WoodPenetration(
	const FMyGameplayAttributeData& OldValue
	)
{
	// 使用默认的游戏玩法属性系统更新通知行为。
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, WoodPenetration, OldValue);
}

void UAS_Character::OnRep_WoodPercentPenetration(
	const FMyGameplayAttributeData& OldValue
	)
{
	// 使用默认的游戏玩法属性系统更新通知行为。
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, WoodPercentPenetration, OldValue);
}

void UAS_Character::OnRep_WoodResistance(
	const FMyGameplayAttributeData& OldValue
	)
{
	// 使用默认的游戏玩法属性系统更新通知行为。
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, WoodResistance, OldValue);
}

void UAS_Character::OnRep_EarthValue(
	const FMyGameplayAttributeData& OldValue
	)
{
	// 使用默认的游戏玩法属性系统更新通知行为。
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, EarthValue, OldValue);
}

void UAS_Character::OnRep_EarthLevel(
	const FMyGameplayAttributeData& OldValue
	)
{
	// 使用默认的游戏玩法属性系统更新通知行为。
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, EarthLevel, OldValue);
}

void UAS_Character::OnRep_EarthPenetration(
	const FMyGameplayAttributeData& OldValue
	)
{
	// 使用默认的游戏玩法属性系统更新通知行为。
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, EarthPenetration, OldValue);
}

void UAS_Character::OnRep_EarthPercentPenetration(
	const FMyGameplayAttributeData& OldValue
	)
{
	// 使用默认的游戏玩法属性系统更新通知行为。
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, EarthPercentPenetration, OldValue);
}

void UAS_Character::OnRep_EarthResistance(
	const FMyGameplayAttributeData& OldValue
	)
{
	// 使用默认的游戏玩法属性系统更新通知行为。
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, EarthResistance, OldValue);
}

void UAS_Character::OnRep_WaterValue(
	const FMyGameplayAttributeData& OldValue
	)
{
	// 使用默认的游戏玩法属性系统更新通知行为。
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, WaterValue, OldValue);
}

void UAS_Character::OnRep_WaterLevel(
	const FMyGameplayAttributeData& OldValue
	)
{
	// 使用默认的游戏玩法属性系统更新通知行为。
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, WaterLevel, OldValue);
}

void UAS_Character::OnRep_WaterPenetration(
	const FMyGameplayAttributeData& OldValue
	)
{
	// 使用默认的游戏玩法属性系统更新通知行为。
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, WaterPenetration, OldValue);
}

void UAS_Character::OnRep_WaterPercentPenetration(
	const FMyGameplayAttributeData& OldValue
	)
{
	// 使用默认的游戏玩法属性系统更新通知行为。
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, WaterPercentPenetration, OldValue);
}

void UAS_Character::OnRep_WaterResistance(
	const FMyGameplayAttributeData& OldValue
	)
{
	// 使用默认的游戏玩法属性系统更新通知行为。
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, WaterResistance, OldValue);
}

void UAS_Character::OnRep_FireValue(
	const FMyGameplayAttributeData& OldValue
	)
{
	// 使用默认的游戏玩法属性系统更新通知行为。
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, FireValue, OldValue);
}

void UAS_Character::OnRep_FireLevel(
	const FMyGameplayAttributeData& OldValue
	)
{
	// 使用默认的游戏玩法属性系统更新通知行为。
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, FireLevel, OldValue);
}

void UAS_Character::OnRep_FirePenetration(
	const FMyGameplayAttributeData& OldValue
	)
{
	// 使用默认的游戏玩法属性系统更新通知行为。
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, FirePenetration, OldValue);
}

void UAS_Character::OnRep_FirePercentPenetration(
	const FMyGameplayAttributeData& OldValue
	)
{
	// 使用默认的游戏玩法属性系统更新通知行为。
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, FirePercentPenetration, OldValue);
}

void UAS_Character::OnRep_FireResistance(
	const FMyGameplayAttributeData& OldValue
	)
{
	// 使用默认的游戏玩法属性系统更新通知行为。
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, FireResistance, OldValue);
}

void UAS_Character::OnRep_MoveSpeed(
	const FMyGameplayAttributeData& OldValue
	)
{
	// 使用默认的游戏玩法属性系统更新通知行为。
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MoveSpeed, OldValue);
}

void UAS_Character::OnRep_CriticalDamage(
	const FMyGameplayAttributeData& OldValue
	)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, CriticalDamage, OldValue);
}

void UAS_Character::OnRep_CriticalHitRate(
	const FMyGameplayAttributeData& OldValue
	)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, CriticalHitRate, OldValue);
}

void UAS_Character::OnRep_HitRate(
	const FMyGameplayAttributeData& OldValue
	)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, HitRate, OldValue);
}

void UAS_Character::OnRep_EvadeRate(
	const FMyGameplayAttributeData& OldValue
	)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, EvadeRate, OldValue);
}

void UAS_Character::OnRep_PerformSpeed(
	const FMyGameplayAttributeData& OldValue
	)
{
	// 使用默认的游戏玩法属性系统更新通知行为。
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, PerformSpeed, OldValue);
}

inline void UAS_Character::OnRep_Shield(
	const FMyGameplayAttributeData& OldValue
	)
{
	// 使用默认的游戏玩法属性系统更新通知行为。
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Shield, OldValue);
}

void UAS_Character::OnRep_Haste(
	const FMyGameplayAttributeData& OldValue
	)
{
	// 使用默认的游戏玩法属性系统更新通知行为。
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Haste, OldValue);
}

void UAS_Character::OnRep_Level(
	const FMyGameplayAttributeData& OldValue
	)
{
	// 使用默认的游戏玩法属性系统更新通知行为。
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Level, OldValue);
}
