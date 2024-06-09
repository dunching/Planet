

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

		// ���ι����� ��͸
		int32 Penetration = 0;

		// ���ι����� �ٷֱȴ�͸
		int32 PercentPenetration = 0;

		// ���ι����� ������(0��Ϊ�˴α�����)
		int32 HitRate = 0;

		// ���ι����� ������(100��Ϊ�˴α�����)
		int32 CriticalHitRate = 0;

		// ���ι����� �����˺�
		int32 CriticalDamage = 0;

		// ��ɵ���ʵ�˺�
		int32 TrueDamage = 0;

		// �����ٶ�
		int32 GAPerformSpeedOffset = 0;

		void SetBaseDamage(int32 Value);

		void SetWuXingDamage(EWuXingType WuXingType, int32 Value);

		// ������
		int32 TreatmentVolume = 0;

		// ��ɵĻ����˺�
		int32 BaseDamage = 0;

		// �˺��ֲ������͡��ȼ����˺���
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

	// ԽС��Խ����
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