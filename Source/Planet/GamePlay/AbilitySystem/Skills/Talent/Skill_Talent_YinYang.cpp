
#include "Skill_Talent_YinYang.h"

#include "AbilitySystemComponent.h"
#include <Engine/OverlapResult.h>

#include "CharacterBase.h"
#include "EquipmentElementComponent.h"
#include "CharacterAttributesComponent.h"
#include "GenerateType.h"
#include "GAEvent_Send.h"
#include "EffectsList.h"
#include "UIManagerSubSystem.h"
#include "EffectItem.h"
#include "AbilityTask_TimerHelper.h"
#include "CollisionDataStruct.h"
#include "HumanCharacter.h"
#include "GroupMnaggerComponent.h"

int32 FTalent_YinYang::GetCurrentValue() const
{
	return CurrentValue;
}

int32 FTalent_YinYang::GetMaxValue() const
{
	return MaxValue;
}

void FTalent_YinYang::SetCurrentValue(int32 NewVal)
{
	if (CurrentValue != NewVal)
	{
		NewVal = FMath::Clamp(NewVal, 0, MaxValue);

		CallbackContainerHelper.ValueChanged(CurrentValue, NewVal);

		CurrentValue = NewVal;
	}
}

void FTalent_YinYang::AddCurrentValue(int32 Value)
{
	SetCurrentValue(GetCurrentValue() + Value);
}

USkill_Talent_YinYang::USkill_Talent_YinYang() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	bRetriggerInstancedAbility = true;
}

void USkill_Talent_YinYang::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
	if (CharacterPtr)
	{
		AbilityActivatedCallbacksHandle = CharacterPtr->GetAbilitySystemComponent()->AbilityActivatedCallbacks.AddUObject(this, &ThisClass::OnSendDamage);

		auto CharacterAttributes = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
		OnValueChanged = CharacterAttributes.HP.GetCurrentProperty().CallbackContainerHelper.AddOnValueChanged(
			std::bind(&ThisClass::OnHPValueChanged, this, std::placeholders::_1, std::placeholders::_2)
		);

		TalentSPtr = TSharedPtr<FCurrentTalentType>(
			CharacterAttributes.TalentSPtr, dynamic_cast<FCurrentTalentType*>(CharacterAttributes.TalentSPtr.Get())
		);
	}
}

void USkill_Talent_YinYang::OnRemoveAbility(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
)
{
	if (OnValueChanged)
	{
		OnValueChanged->UnBindCallback();
	}

	if (CharacterPtr)
	{
		CharacterPtr->GetAbilitySystemComponent()->AbilityActivatedCallbacks.Remove(AbilityActivatedCallbacksHandle);
	}

	Super::OnRemoveAbility(ActorInfo, Spec);
}

void USkill_Talent_YinYang::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USkill_Talent_YinYang::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	{
		Tick_StateTrans_Accumulate += DeltaTime;
		if (Tick_StateTrans_Accumulate >= Tick_StateTrans_Interval)
		{
			Tick_StateTrans_Accumulate = 0.f;

			AddValue(Increment);
		}
	}
	{
		Tick_Buff_Accumulate += DeltaTime;
		if (Tick_Buff_Accumulate >= Tick_Buff_Interval)
		{
			Tick_Buff_Accumulate = 0.f;

			PerformAction();
		}
	}
	if (EffectItemPtr)
	{
	}
}

void USkill_Talent_YinYang::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void USkill_Talent_YinYang::AddValue(int32 Value)
{
	switch (TalentSPtr->CurentType)
	{
	case ETalent_State_Type::kYin:
	{
		TalentSPtr->AddCurrentValue(Value);
		if (TalentSPtr->GetCurrentValue() >= TalentSPtr->GetMaxValue())
		{
			TalentSPtr->SetCurrentValue(0);
			TalentSPtr->CurentType = ETalent_State_Type::kYang;
		}
	}
	break;
	case ETalent_State_Type::kYang:
	{
		TalentSPtr->AddCurrentValue(Value);
		if (TalentSPtr->GetCurrentValue() >= TalentSPtr->GetMaxValue())
		{
			TalentSPtr->SetCurrentValue(0);
			TalentSPtr->CurentType = ETalent_State_Type::kYin;
		}
	}
	break;
	}
}

void USkill_Talent_YinYang::PerformAction()
{
	switch (TalentSPtr->CurentType)
	{
	case ETalent_State_Type::kYin:
	{
		PerformAction_Yin();
	}
	break;
	case ETalent_State_Type::kYang:
	{
		PerformAction_Yang();
	}
	break;
	}
}

void USkill_Talent_YinYang::PerformAction_Yang()
{
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(PawnECC);

	FCollisionShape  CollisionShape = FCollisionShape::MakeSphere(Radius);

	FCollisionQueryParams CapsuleParams;

	// Test:����״���ң�����ֱ�ӻ�ȡ����Ա��������뼴��
	TArray<struct FOverlapResult> OutOverlaps;
	if (GetWorldImp()->OverlapMultiByObjectType(
		OutOverlaps,
		CharacterPtr->GetActorLocation(),
		FQuat::Identity,
		ObjectQueryParams,
		CollisionShape,
		CapsuleParams
	))
	{
		FGameplayAbilityTargetData_GASendEvent* GAEventDataPtr = new FGameplayAbilityTargetData_GASendEvent(CharacterPtr);

		FGameplayEventData Payload;
		Payload.TargetData.Add(GAEventDataPtr);

		GAEventDataPtr->TriggerCharacterPtr = CharacterPtr;

		for (auto Iter : OutOverlaps)
		{
			auto TargetCharacterPtr = Cast<ACharacterBase>(Iter.GetActor());
			if (TargetCharacterPtr)
			{
				if (CharacterPtr->IsTeammate(TargetCharacterPtr))
				{
					FGAEventData GAEventData(TargetCharacterPtr, CharacterPtr);

					auto & CharacterAttributesRef = TargetCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
					GAEventData.HP = CharacterAttributesRef.HP.GetMaxValue() * (TreatmentVolumePercent / 100.f);

					GAEventDataPtr->DataAry.Add(GAEventData);
				}
			}
		}

		SendEvent(Payload);
	}
}

void USkill_Talent_YinYang::PerformAction_Yin()
{

}

void USkill_Talent_YinYang::OnHPValueChanged(int32 OldValue, int32 NewValue)
{
	if (NewValue < OldValue)
	{
	}
}

void USkill_Talent_YinYang::OnSendDamage(UGameplayAbility* GAPtr)
{
	if (CharacterPtr)
	{
		if (
			GAPtr &&
			(GAPtr->GetCurrentAbilitySpecHandle() == CharacterPtr->GetEquipmentItemsComponent()->SendEventHandle)
			)
		{
			auto SendGaPtr = Cast<UGAEvent_Send>(GAPtr);
			if (!SendGaPtr)
			{
				return;
			}
			const auto& EventDataRef = SendGaPtr->GetCurrentEventData();
			if (!EventDataRef.TargetData.Data.IsValidIndex(0))
			{
				return;
			}
			auto GAEventDataPtr = dynamic_cast<const FGameplayAbilityTargetData_GASendEvent*>(EventDataRef.TargetData.Get(0));
			if (!GAEventDataPtr)
			{
				return;
			}

			for (const auto& DataIter : GAEventDataPtr->DataAry)
			{
				switch (TalentSPtr->CurentType)
				{
				case ETalent_State_Type::kYin:
				{
					for (const auto& Iter : DataIter.ElementSet)
					{
						const auto Type = Iter.Get<0>();
						if (
							(Type == EWuXingType::kWood) &&
							(Type == EWuXingType::kWater)
							)
						{
							AddValue(AttackIncrement);
							break;
						}
					}
				}
				break;
				case ETalent_State_Type::kYang:
				{
					for (const auto& Iter : DataIter.ElementSet)
					{
						const auto Type = Iter.Get<0>();
						if (
							(Type == EWuXingType::kGold) &&
							(Type == EWuXingType::kFire)
							)
						{
							AddValue(AttackIncrement);
							break;
						}
					}
				}
				break;
				}
			}
		}
	}
}