
#include "InteractiveBaseGAComponent.h"

#include <queue>
#include <map>

#include "GameplayAbilitySpec.h"

#include "GAEvent_Helper.h"
#include "CharacterBase.h"
#include "CharacterAttributesComponent.h"
#include "GenerateType.h"
#include "PlanetPlayerState.h"
#include "BasicFuturesBase.h"
#include "GAEvent_Send.h"
#include "GAEvent_Received.h"
#include "SceneElement.h"
#include "Weapon_Base.h"
#include "Skill_Base.h"
#include "Skill_Active_Base.h"
#include "Skill_WeaponActive_PickAxe.h"
#include "Skill_WeaponActive_HandProtection.h"
#include "Skill_WeaponActive_RangeTest.h"
#include "Weapon_HandProtection.h"
#include "Weapon_PickAxe.h"
#include "Weapon_RangeTest.h"
#include "AssetRefMap.h"
#include "Skill_Talent_NuQi.h"
#include "Skill_Talent_YinYang.h"
#include "Skill_Element_Gold.h"
#include "InputComponent/InputProcessorSubSystem.h"
#include "Tool_PickAxe.h"
#include "HumanRegularProcessor.h"
#include "HumanCharacter.h"
#include "GA_Tool_Periodic.h"

FName UInteractiveBaseGAComponent::ComponentName = TEXT("InteractiveBaseGAComponent");

void UInteractiveBaseGAComponent::AddSendEventModify(const TSharedPtr<IGAEventModifySendInterface>& GAEventModifySPtr)
{
	for (bool bIsContinue = true; bIsContinue;)
	{
		bIsContinue = false;
		GAEventModifySPtr->ID = FMath::RandRange(1, std::numeric_limits<int32>::max());
		for (const auto& Iter : SendEventModifysMap)
		{
			if (Iter->ID == GAEventModifySPtr->ID)
			{
				bIsContinue = true;
				break;
			}
		}
	}
	SendEventModifysMap.emplace(GAEventModifySPtr);
}

void UInteractiveBaseGAComponent::RemoveSendEventModify(const TSharedPtr<IGAEventModifySendInterface>& GAEventModifySPtr)
{
	for (auto Iter = SendEventModifysMap.begin(); Iter != SendEventModifysMap.end(); Iter++)
	{
		if ((*Iter)->ID == GAEventModifySPtr->ID)
		{
			SendEventModifysMap.erase(Iter);
			break;
		}
	}
}

void UInteractiveBaseGAComponent::AddReceviedEventModify(const TSharedPtr<IGAEventModifyReceivedInterface>& GAEventModifySPtr)
{
	for (bool bIsContinue = true; bIsContinue;)
	{
		bIsContinue = false;
		GAEventModifySPtr->ID = FMath::RandRange(1, std::numeric_limits<int32>::max());
		for (const auto& Iter : ReceivedEventModifysMap)
		{
			if (Iter->ID == GAEventModifySPtr->ID)
			{
				bIsContinue = true;
				break;
			}
		}
	}
	ReceivedEventModifysMap.emplace(GAEventModifySPtr);
}

void UInteractiveBaseGAComponent::RemoveReceviedEventModify(const TSharedPtr<IGAEventModifyReceivedInterface>& GAEventModifySPtr)
{
	for (auto Iter = ReceivedEventModifysMap.begin(); Iter != ReceivedEventModifysMap.end(); Iter++)
	{
		if ((*Iter)->ID == GAEventModifySPtr->ID)
		{
			ReceivedEventModifysMap.erase(Iter);
			break;
		}
	}
}

UGA_Tool_Periodic* UInteractiveBaseGAComponent::ExcuteEffects(FGameplayAbilityTargetData_Tool_Periodic* GameplayAbilityTargetDataPtr)
{
	auto OnwerActorPtr = GetOwner<ACharacterBase>();
	if (!OnwerActorPtr)
	{
		return nullptr;
	}

	auto ASCPtr = OnwerActorPtr->GetAbilitySystemComponent();

	FGameplayEventData Payload;
	Payload.TargetData.Add(GameplayAbilityTargetDataPtr);
	if (ASCPtr->TriggerAbilityFromGameplayEvent(
		GAToolPeriodicHandle,
		ASCPtr->AbilityActorInfo.Get(),
		FGameplayTag::EmptyTag,
		&Payload,
		*ASCPtr
	))
	{
		auto GameplayAbilitySpecPtr = ASCPtr->FindAbilitySpecFromHandle(GAToolPeriodicHandle);
		if (!GameplayAbilitySpecPtr)
		{
			return Cast<UGA_Tool_Periodic>(GameplayAbilitySpecPtr->GetPrimaryInstance());
		}
	}

	return nullptr;
}

void UInteractiveBaseGAComponent::InitialBaseGAs()
{
	auto OnwerActorPtr = GetOwner<ACharacterBase>();
	if (OnwerActorPtr)
	{
		auto GASPtr = OnwerActorPtr->GetAbilitySystemComponent();

		SendEventHandle = GASPtr->GiveAbility(
			FGameplayAbilitySpec(UGAEvent_Send::StaticClass(), 1)
		);

		ReceivedEventHandle = GASPtr->GiveAbility(
			FGameplayAbilitySpec(UGAEvent_Received::StaticClass(), 1)
		);
		
		GAToolPeriodicHandle = GASPtr->GiveAbility(
			FGameplayAbilitySpec(UGA_Tool_Periodic::StaticClass(), 1)
		);

		for (auto Iter : CharacterAbilities)
		{
			GASPtr->GiveAbility(
				FGameplayAbilitySpec(Iter, 1)
			);
		}
	}

#pragma region 结算效果修正
	// 输出

	// 群体伤害或治疗减益
	AddSendGroupEffectModify();
	// 伤害类型
	AddSendWuXingModify();

	// 接收

	// 五行之间的减免
	AddReceivedWuXingModify();

	// 基础属性
	AddReceivedModify();
#pragma endregion 结算效果修正
}

void UInteractiveBaseGAComponent::AddSendGroupEffectModify()
{
	struct GAEventModify_MultyTarget : public IGAEventModifySendInterface
	{
		GAEventModify_MultyTarget(int32 InPriority) :
			IGAEventModifySendInterface(InPriority)
		{
		}

		virtual void Modify(FGameplayAbilityTargetData_GASendEvent& GameplayAbilityTargetData_GAEvent)override
		{
			if (GameplayAbilityTargetData_GAEvent.DataAry.Num() > 1)
			{
				for (auto& Iter : GameplayAbilityTargetData_GAEvent.DataAry)
				{
					Iter.TrueDamage =
						Iter.TrueDamage / GameplayAbilityTargetData_GAEvent.DataAry.Num();

					Iter.BaseDamage =
						Iter.BaseDamage / GameplayAbilityTargetData_GAEvent.DataAry.Num();

					for (auto& ElementIter : Iter.ElementSet)
					{
						ElementIter.Get<2>() =
							ElementIter.Get<2>() / GameplayAbilityTargetData_GAEvent.DataAry.Num();
					}

					Iter.HP =
						Iter.HP / GameplayAbilityTargetData_GAEvent.DataAry.Num();
				}
			}
		}
	};
	AddSendEventModify(MakeShared<GAEventModify_MultyTarget>(9999));
}

void UInteractiveBaseGAComponent::AddSendWuXingModify()
{
	struct GAEventModify_MultyTarget : public IGAEventModifySendInterface
	{
		GAEventModify_MultyTarget(int32 InPriority) :
			IGAEventModifySendInterface(InPriority)
		{
		}

		virtual void Modify(FGameplayAbilityTargetData_GASendEvent& GameplayAbilityTargetData_GAEvent)override
		{
			for (auto& Iter : GameplayAbilityTargetData_GAEvent.DataAry)
			{
				if (Iter.ElementSet.IsEmpty())
				{
					const auto& CharacterAttributes =
						GameplayAbilityTargetData_GAEvent.TriggerCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();

					std::map<int32, EWuXingType, std::greater<int>> ElementMap;
					ElementMap.emplace(CharacterAttributes.Element.GoldElement.GetCurrentValue(), EWuXingType::kGold);
					ElementMap.emplace(CharacterAttributes.Element.WoodElement.GetCurrentValue(), EWuXingType::kWood);
					ElementMap.emplace(CharacterAttributes.Element.WaterElement.GetCurrentValue(), EWuXingType::kWater);
					ElementMap.emplace(CharacterAttributes.Element.FireElement.GetCurrentValue(), EWuXingType::kFire);
					ElementMap.emplace(CharacterAttributes.Element.SoilElement.GetCurrentValue(), EWuXingType::kSoil);

					if (ElementMap.begin()->first > 0)
					{
						const auto Tuple = MakeTuple(
							ElementMap.begin()->second,
							ElementMap.begin()->first,
							Iter.BaseDamage
						);
						Iter.ElementSet.Add(Tuple);
					}
				}
			}
		}
	};
	AddSendEventModify(MakeShared<GAEventModify_MultyTarget>(9998));
}

void UInteractiveBaseGAComponent::AddReceivedWuXingModify()
{
	struct GAEventModify_MultyTarget : public IGAEventModifyReceivedInterface
	{
		GAEventModify_MultyTarget(int32 InPriority) :
			IGAEventModifyReceivedInterface(InPriority)
		{
		}

		virtual void Modify(FGameplayAbilityTargetData_GAReceivedEvent& GameplayAbilityTargetData_GAEvent)override
		{
			const auto Caculation_Effective_Rate = [](int32 SelfLevel, int32 TargetLevel) {

				// 
				const auto Offset = (SelfLevel - TargetLevel) / 3;
				const auto Effective_Rate = 1.f + (Offset * 0.25f);
				return Effective_Rate;
				};

			auto& DataRef = GameplayAbilityTargetData_GAEvent.Data;
			if (DataRef.ElementSet.IsEmpty() && DataRef.TargetCharacterPtr.IsValid())
			{
				const auto& CharacterAttributes =
					DataRef.TargetCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();

				std::map<int32, EWuXingType, std::greater<int>> ElementMap;
				ElementMap.emplace(CharacterAttributes.Element.GoldElement.GetCurrentValue(), EWuXingType::kGold);
				ElementMap.emplace(CharacterAttributes.Element.WoodElement.GetCurrentValue(), EWuXingType::kWood);
				ElementMap.emplace(CharacterAttributes.Element.WaterElement.GetCurrentValue(), EWuXingType::kWater);
				ElementMap.emplace(CharacterAttributes.Element.FireElement.GetCurrentValue(), EWuXingType::kFire);
				ElementMap.emplace(CharacterAttributes.Element.SoilElement.GetCurrentValue(), EWuXingType::kSoil);

				const auto Effective_Rate = Caculation_Effective_Rate(ElementMap.begin()->first, 0);

				DataRef.BaseDamage = DataRef.BaseDamage * Effective_Rate;
			}
			else
			{
				const auto& CharacterAttributes =
					GameplayAbilityTargetData_GAEvent.TriggerCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();

				for (auto& ElementIter : DataRef.ElementSet)
				{
					// ľ          ˮ  ˮ ˻𣬻 ˽𣬽  ľ
					switch (ElementIter.Get<0>())
					{
					case EWuXingType::kGold:
					{
						const auto Effective_Rate = Caculation_Effective_Rate(CharacterAttributes.Element.FireElement.GetCurrentValue(), ElementIter.Get<1>());
						ElementIter.Get<2>() = ElementIter.Get<2>() * Effective_Rate;
					}
					break;
					case EWuXingType::kWood:
					{
						const auto Effective_Rate = Caculation_Effective_Rate(CharacterAttributes.Element.GoldElement.GetCurrentValue(), ElementIter.Get<1>());
						ElementIter.Get<2>() = ElementIter.Get<2>() * Effective_Rate;
					}
					break;
					case EWuXingType::kWater:
					{
						const auto Effective_Rate = Caculation_Effective_Rate(CharacterAttributes.Element.SoilElement.GetCurrentValue(), ElementIter.Get<1>());
						ElementIter.Get<2>() = ElementIter.Get<2>() * Effective_Rate;
					}
					break;
					case EWuXingType::kFire:
					{
						const auto Effective_Rate = Caculation_Effective_Rate(CharacterAttributes.Element.WaterElement.GetCurrentValue(), ElementIter.Get<1>());
						ElementIter.Get<2>() = ElementIter.Get<2>() * Effective_Rate;
					}
					break;
					case EWuXingType::kSoil:
					{
						const auto Effective_Rate = Caculation_Effective_Rate(CharacterAttributes.Element.WoodElement.GetCurrentValue(), ElementIter.Get<1>());
						ElementIter.Get<2>() = ElementIter.Get<2>() * Effective_Rate;
					}
					break;
					default:
						break;
					}
				}
			}
		}
	};
	AddReceviedEventModify(MakeShared<GAEventModify_MultyTarget>(9999));
}

void UInteractiveBaseGAComponent::AddReceivedModify()
{
	struct GAEventModify_MultyTarget : public IGAEventModifyReceivedInterface
	{
		GAEventModify_MultyTarget(int32 InPriority) :
			IGAEventModifyReceivedInterface(InPriority)
		{
		}

		virtual void Modify(FGameplayAbilityTargetData_GAReceivedEvent& GameplayAbilityTargetData_GAEvent)override
		{
			if (GameplayAbilityTargetData_GAEvent.Data.TargetCharacterPtr.IsValid())
			{
				const auto& SelfCharacterAttributes =
					GameplayAbilityTargetData_GAEvent.Data.TargetCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();

				const auto& TargetCharacterAttributes =
					GameplayAbilityTargetData_GAEvent.TriggerCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();

				{
					const auto Rate = (TargetCharacterAttributes.HitRate.GetCurrentValue() - SelfCharacterAttributes.Evade.GetCurrentValue()) /
						static_cast<float>(TargetCharacterAttributes.HitRate.GetMaxValue());

					GameplayAbilityTargetData_GAEvent.Data.HitRate = FMath::FRand() <= Rate ? 100 : 0;
				}
			}
		}
	};
	AddReceviedEventModify(MakeShared<GAEventModify_MultyTarget>(9998));
}

void UInteractiveBaseGAComponent::OnSendEventModifyData(FGameplayAbilityTargetData_GASendEvent& OutGAEventData)
{
	for (auto Iter : SendEventModifysMap)
	{
		Iter->Modify(OutGAEventData);
	}
}

void UInteractiveBaseGAComponent::OnReceivedEventModifyData(FGameplayAbilityTargetData_GAReceivedEvent& OutGAEventData)
{
	for (auto Iter : ReceivedEventModifysMap)
	{
		Iter->Modify(OutGAEventData);
	}
}
