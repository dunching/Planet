

#pragma once

#include "CoreMinimal.h"

#include "Abilities/GameplayAbilityTargetTypes.h"

#include "GenerateType.h"
#include "SceneElement.h"

class ACharacterBase;
class UInteractiveComponent;
class UInteractiveBaseGAComponent;

struct FCharacterAttributes;

struct FGAEventData
{
	FGAEventData(
		TWeakObjectPtr<ACharacterBase>  TargetCharacterPtr,
		TWeakObjectPtr<ACharacterBase>  TriggerCharacterPtr
	);

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

	void AddWuXingDamage(EWuXingType WuXingType, int32 Value);

	// �ظ���HP
	int32 HP = 0;

	// �ظ���PP
	int32 PP = 0;

	// ��ɵĻ����˺�
	int32 BaseDamage = 0;

	// �˺��ֲ������͡��ȼ����˺���
	TSet<TTuple<EWuXingType, int32, int32>>ElementSet;

	TWeakObjectPtr<ACharacterBase> TriggerCharacterPtr = nullptr;

	TWeakObjectPtr<ACharacterBase> TargetCharacterPtr = nullptr;
};

struct FGameplayAbilityTargetData_GASendEvent : public FGameplayAbilityTargetData
{
	using FCallbackHandleContainer = TCallbackHandleContainer<void(ACharacterBase*, const FGAEventData&)>;

	FGameplayAbilityTargetData_GASendEvent(
		TWeakObjectPtr<ACharacterBase>  TriggerCharacterPtr
	);

	FGameplayAbilityTargetData_GASendEvent* Clone()const;

	TWeakObjectPtr<ACharacterBase> TriggerCharacterPtr = nullptr;

	TArray<FGAEventData> DataAry;

	FCallbackHandleContainer TrueDataDelagate;

};

struct FGameplayAbilityTargetData_GAReceivedEvent : public FGameplayAbilityTargetData
{
	using FCallbackHandleContainer = TCallbackHandleContainer<void(ACharacterBase*, const FGAEventData&)>;

	FGameplayAbilityTargetData_GAReceivedEvent(
		TWeakObjectPtr<ACharacterBase>  TargetCharacterPtr,
		TWeakObjectPtr<ACharacterBase>  TriggerCharacterPtr
	);

	FGameplayAbilityTargetData_GAReceivedEvent* Clone()const;

	FGAEventData Data;

	TWeakObjectPtr<ACharacterBase> TriggerCharacterPtr = nullptr;

	FCallbackHandleContainer TrueDataDelagate;
};

struct FGAEventModify_key_compare;

class PLANET_API IGAEventModifyInterface
{
public:

	friend UInteractiveBaseGAComponent;
	friend FGAEventModify_key_compare;

	IGAEventModifyInterface(int32 InPriority = 1);

	bool operator<(const IGAEventModifyInterface& RightValue)const;

private:

	// ԽС��Խ����
	int32 Priority = -1;

	int32 ID = -1;

};

class PLANET_API IGAEventModifySendInterface : public IGAEventModifyInterface
{
public:

	IGAEventModifySendInterface(int32 InPriority = 1);

	virtual void Modify(FGameplayAbilityTargetData_GASendEvent& OutGameplayAbilityTargetData_GAEvent);

};

class PLANET_API IGAEventModifyReceivedInterface : public IGAEventModifyInterface
{
public:

	IGAEventModifyReceivedInterface(int32 InPriority = 1);

	virtual void Modify(FGameplayAbilityTargetData_GAReceivedEvent& OutGameplayAbilityTargetData_GAEvent);

};

struct FGAEventModify_key_compare
{
	bool operator()(const TSharedPtr<IGAEventModifyInterface>& lhs, const TSharedPtr<IGAEventModifyInterface>& rhs) const
	{
		return lhs->Priority < rhs->Priority;
	}
};