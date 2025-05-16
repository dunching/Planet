#include "AS_Character.h"

#include <functional>

#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

#include "GameplayTagsLibrary.h"

void FMyGameplayAttributeData::SetBaseValue(float NewValue)
{
	FGameplayAttributeData::SetBaseValue(NewValue);
}

void UAS_Character::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void UAS_Character::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
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

void UAS_Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
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
	
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MoveSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION(ThisClass, CriticalDamage, COND_None);
	DOREPLIFETIME_CONDITION(ThisClass, HitRate, COND_None);
	DOREPLIFETIME_CONDITION(ThisClass, Evade, COND_None);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, PerformSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION(ThisClass, Shield, COND_None);
}

void UAS_Character::OnRep_Max_HP(const FMyGameplayAttributeData& OldHealth)
{
	// 使用默认的游戏玩法属性系统更新通知行为。
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Max_HP, OldHealth);
}

void UAS_Character::OnRep_HP(const FMyGameplayAttributeData& OldHealth)
{
	// 使用默认的游戏玩法属性系统更新通知行为。
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, HP, OldHealth);
}

void UAS_Character::OnRep_Stamina(const FMyGameplayAttributeData& OldHealth)
{
	// 使用默认的游戏玩法属性系统更新通知行为。
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Stamina, OldHealth);
}

void UAS_Character::OnRep_Mana(const FMyGameplayAttributeData& OldHealth)
{
	// 使用默认的游戏玩法属性系统更新通知行为。
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Mana, OldHealth);
}

void UAS_Character::OnRep_MoveSpeed(const FMyGameplayAttributeData& OldHealth)
{
	// 使用默认的游戏玩法属性系统更新通知行为。
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MoveSpeed, OldHealth);
}

void UAS_Character::OnRep_PerformSpeed(const FMyGameplayAttributeData& OldHealth)
{
	// 使用默认的游戏玩法属性系统更新通知行为。
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, PerformSpeed, OldHealth);
}

inline void UAS_Character::OnRep_Shield(
	const FMyGameplayAttributeData& OldHealth
	)
{
	// 使用默认的游戏玩法属性系统更新通知行为。
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Shield, OldHealth);
}
