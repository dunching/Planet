

#pragma once

#include "CoreMinimal.h"

#include "Abilities/GameplayAbilityTargetTypes.h"
#include "UObject/Interface.h"

class ACharacterBase;
class UEquipmentElementComponent;

struct FGameplayAbilityTargetData_GAEvent : public FGameplayAbilityTargetData
{
	FGameplayAbilityTargetData_GAEvent* Clone()const;

	TArray<ACharacterBase*>TargetActorAry;

	ACharacterBase* TriggerCharacterPtr = nullptr;

	int32 ADDamage = 10;

	int32 GAPerformSpeedOffset = 0;

};

struct FGAEventModify_key_compare;

class PLANET_API IGAEventModifyInterface
{
public:

	friend UEquipmentElementComponent;
	friend FGAEventModify_key_compare;

	IGAEventModifyInterface(int32 InPriority = 1);

	virtual void Modify(FGameplayAbilityTargetData_GAEvent& GameplayAbilityTargetData_GAEvent);

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