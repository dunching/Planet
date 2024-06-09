

#pragma once

#include "CoreMinimal.h"

#include "Abilities/GameplayAbilityTargetTypes.h"

#include "GenerateType.h"
#include "SceneElement.h"

class ACharacterBase;
class UEquipmentElementComponent;

struct FCharacterAttributes;

struct FGameplayAbilityTargetData_GAEvent : public FGameplayAbilityTargetData
{
	struct FData
	{
		// 
		bool bIsWeaponAttack = false;

		// 本次攻击的 穿透
		int32 Penetration = 0;

		// 本次攻击的 百分比穿透
		int32 PercentPenetration = 0;

		// 本次攻击的 命中率(0则为此次被闪避)
		int32 HitRate = 0;

		// 本次攻击的 会心率(100则为此次被会心)
		int32 CriticalHitRate = 0;

		// 本次攻击的 会心伤害
		int32 CriticalDamage = 0;

		// 造成的真实伤害
		int32 TrueDamage = 0;

		// 攻击速度
		int32 GAPerformSpeedOffset = 0;

		void SetBaseDamage(int32 Value);

		void SetWuXingDamage(EWuXingType WuXingType, int32 Value);

		// 治疗量
		int32 TreatmentVolume = 0;

		// 造成的基础伤害
		int32 BaseDamage = 0;

		// 伤害分布：类型、等级、伤害量
		TSet<TTuple<EWuXingType, int32, int32>>ElementSet;

		TWeakObjectPtr<ACharacterBase> TriggerCharacterPtr = nullptr;
	};

	using FCallbackHandleContainer = TCallbackHandleContainer<void(ACharacterBase*, const FData&)>;

	FGameplayAbilityTargetData_GAEvent* Clone()const;

	FGameplayAbilityTargetData_GAEvent(
		ACharacterBase* TriggerCharacterPtr
	);

	TArray<ACharacterBase*>TargetActorAry;

	TWeakObjectPtr<ACharacterBase> TriggerCharacterPtr = nullptr;

	FData Data;

	FCallbackHandleContainer TrueDataDelagate;

};

struct FGAEventModify_key_compare;

class PLANET_API IGAEventModifyInterface
{
public:

	friend UEquipmentElementComponent;
	friend FGAEventModify_key_compare;

	IGAEventModifyInterface(int32 InPriority = 1);

	virtual void Modify(FGameplayAbilityTargetData_GAEvent& OutGameplayAbilityTargetData_GAEvent);

	bool operator<(const IGAEventModifyInterface& RightValue)const;

private:

	// 越小的越先算
	int32 Priority = -1;

	int32 ID = -1;

};

struct FGAEventModify_key_compare
{
	bool operator()(const TSharedPtr<IGAEventModifyInterface>& lhs, const TSharedPtr<IGAEventModifyInterface>& rhs) const
	{
		return lhs->Priority < rhs->Priority;
	}
};