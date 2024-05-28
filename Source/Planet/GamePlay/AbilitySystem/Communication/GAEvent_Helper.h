

#pragma once

#include "CoreMinimal.h"

#include "Abilities/GameplayAbilityTargetTypes.h"

#include "GenerateType.h"

class ACharacterBase;
class UEquipmentElementComponent;

struct FGameplayAbilityTargetData_GAEvent : public FGameplayAbilityTargetData
{
	struct FData
	{
		bool bIsWeaponAttack = false;

		int32 ADDamage = 0;

		int32 TreatmentVolume = 0;

		int32 GAPerformSpeedOffset = 0;
	};

	using FCallbackHandleContainer = TCallbackHandleContainer<void(ACharacterBase*, const FData&)> ;

	FGameplayAbilityTargetData_GAEvent* Clone()const;

	TArray<ACharacterBase*>TargetActorAry;

	ACharacterBase* TriggerCharacterPtr = nullptr;

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