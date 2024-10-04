
#include "BaseFeatureComponent.h"

#include <queue>
#include <map>

#include "GameplayAbilitySpec.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

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
#include "Skill_Talent_NuQi.h"
#include "Skill_Talent_YinYang.h"
#include "Skill_Element_Gold.h"
#include "CS_RootMotion.h"
#include "CS_PeriodicPropertyModify.h"
#include "Weapon_HandProtection.h"
#include "Weapon_PickAxe.h"
#include "Weapon_RangeTest.h"
#include "AssetRefMap.h"
#include "InputProcessorSubSystem.h"
#include "Tool_PickAxe.h"
#include "HumanRegularProcessor.h"
#include "HumanCharacter.h"
#include "GameplayTagsSubSystem.h"
#include "BasicFutures_Dash.h"
#include "BasicFutures_MoveToAttaclArea.h"
#include "BasicFutures_Affected.h"
#include "SceneUnitExtendInfo.h"
#include "StateTagExtendInfo.h"
#include "StateProcessorComponent.h"
#include "CS_PeriodicStateModify.h"
#include "CS_RootMotion_FlyAway.h"
#include "CS_RootMotion_TornadoTraction.h"
#include "CS_RootMotion_MoveAlongSpline.h"
#include "CS_RootMotion_KnockDown.h"
#include "CS_PeriodicStateModify_Stun.h"
#include "CS_PeriodicStateModify_Charm.h"
#include "CS_PeriodicStateModify_Ice.h"
#include "CS_PeriodicPropertyTag.h"

FName UBaseFeatureComponent::ComponentName = TEXT("InteractiveBaseGAComponent");

bool UBaseFeatureComponent::IsInDeath() const
{
	auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
	return OnwerActorPtr->GetAbilitySystemComponent()->HasMatchingGameplayTag(
		UGameplayTagsSubSystem::GetInstance()->DeathingTag
	);
}

bool UBaseFeatureComponent::IsUnSelected() const
{
	return false;
}

bool UBaseFeatureComponent::IsRunning() const
{
	auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
	return OnwerActorPtr->GetAbilitySystemComponent()->K2_HasMatchingGameplayTag(
		UGameplayTagsSubSystem::GetInstance()->State_Locomotion_Run
	);
}

bool UBaseFeatureComponent::IsRootMotion() const
{
	auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
	return OnwerActorPtr->GetCharacterMovement()->HasRootMotionSources();
}

bool UBaseFeatureComponent::IsInFighting() const
{
	auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
	return OnwerActorPtr->GetAbilitySystemComponent()->HasMatchingGameplayTag(
		UGameplayTagsSubSystem::GetInstance()->InFightingTag
	);
}

void UBaseFeatureComponent::AddSendEventModify(const TSharedPtr<IGAEventModifySendInterface>& GAEventModifySPtr)
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

void UBaseFeatureComponent::RemoveSendEventModify(const TSharedPtr<IGAEventModifySendInterface>& GAEventModifySPtr)
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

void UBaseFeatureComponent::AddReceviedEventModify(const TSharedPtr<IGAEventModifyReceivedInterface>& GAEventModifySPtr)
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

void UBaseFeatureComponent::RemoveReceviedEventModify(const TSharedPtr<IGAEventModifyReceivedInterface>& GAEventModifySPtr)
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

void UBaseFeatureComponent::SendEventImp(
	FGameplayAbilityTargetData_TagModify* GameplayAbilityTargetDataSPtr
)
{
#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		FGameplayAbilitySpec Spec(UCS_PeriodicPropertyTag::StaticClass(), 1);

		FGameplayEventData* Payload = new FGameplayEventData;
		Payload->TargetData.Add(GameplayAbilityTargetDataSPtr);

		auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
		auto ASCPtr = OnwerActorPtr->GetAbilitySystemComponent();
		ASCPtr->GiveAbilityAndActivateOnce(
			Spec,
			Payload
		);
	}
#endif
}

void UBaseFeatureComponent::SendEventImp(
	FGameplayAbilityTargetData_GASendEvent* GAEventDataPtr
)
{
#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
		if (OnwerActorPtr)
		{
			FGameplayEventData Payload;

			Payload.TargetData.Add(new FGameplayAbilityTargetData_GAEventType(EEventType::kNormal));
			Payload.TargetData.Add(GAEventDataPtr);

			auto ASCPtr = OnwerActorPtr->GetAbilitySystemComponent();
			ASCPtr->TriggerAbilityFromGameplayEvent(
				SendEventHandle,
				ASCPtr->AbilityActorInfo.Get(),
				UGameplayTagsSubSystem::GetInstance()->BaseFeature_Send,
				&Payload,
				*ASCPtr
			);
		}
	}
#endif
}

void UBaseFeatureComponent::SendEventImp(
	FGameplayAbilityTargetData_RootMotion* GameplayAbilityTargetDataPtr
)
{
#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
		if (OnwerActorPtr)
		{
			FGameplayEventData Payload;

			Payload.TargetData.Add(new FGameplayAbilityTargetData_GAEventType(EEventType::kRootMotion));
			Payload.TargetData.Add(GameplayAbilityTargetDataPtr);

			auto ASCPtr = OnwerActorPtr->GetAbilitySystemComponent();
			ASCPtr->TriggerAbilityFromGameplayEvent(
				SendEventHandle,
				ASCPtr->AbilityActorInfo.Get(),
				FGameplayTag(),
				&Payload,
				*ASCPtr
			);
		}
	}
#endif
}

void UBaseFeatureComponent::SendEventImp(
	FGameplayAbilityTargetData_PropertyModify* GameplayAbilityTargetDataPtr
)
{
#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
		if (OnwerActorPtr)
		{
			FGameplayEventData Payload;

			Payload.TargetData.Add(new FGameplayAbilityTargetData_GAEventType(EEventType::kPeriodic_PropertyModify));
			Payload.TargetData.Add(GameplayAbilityTargetDataPtr);

			auto ASCPtr = OnwerActorPtr->GetAbilitySystemComponent();
			ASCPtr->TriggerAbilityFromGameplayEvent(
				SendEventHandle,
				ASCPtr->AbilityActorInfo.Get(),
				FGameplayTag(),
				&Payload,
				*ASCPtr
			);
		}
	}
#endif
}

void UBaseFeatureComponent::SendEventImp(
	FGameplayAbilityTargetData_StateModify* GameplayAbilityTargetDataPtr
)
{
#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
		if (OnwerActorPtr)
		{
			FGameplayEventData Payload;

			Payload.TargetData.Add(new FGameplayAbilityTargetData_GAEventType(EEventType::kPeriodic_StateTagModify));
			Payload.TargetData.Add(GameplayAbilityTargetDataPtr);

			auto ASCPtr = OnwerActorPtr->GetAbilitySystemComponent();
			ASCPtr->TriggerAbilityFromGameplayEvent(
				SendEventHandle,
				ASCPtr->AbilityActorInfo.Get(),
				FGameplayTag(),
				&Payload,
				*ASCPtr
			);
		}
	}
#endif
}

void UBaseFeatureComponent::ClearData2Other(
	const TMap<FOwnerPawnType*, TMap<ECharacterPropertyType, FBaseProperty>>& ModifyPropertyMap, const FGameplayTag& DataSource
)
{
	auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
	if (!OnwerActorPtr)
	{
		return;
	}

	FGameplayAbilityTargetData_GASendEvent* GAEventDataPtr = new FGameplayAbilityTargetData_GASendEvent(OnwerActorPtr);

	GAEventDataPtr->TriggerCharacterPtr = OnwerActorPtr;

	for (const auto Iter : ModifyPropertyMap)
	{
		FGAEventData GAEventData(Iter.Key, OnwerActorPtr);

		GAEventData.DataSource = DataSource;
		GAEventData.DataModify = Iter.Value;
		GAEventData.bIsClearData = true;

		GAEventDataPtr->DataAry.Add(GAEventData);
	}
	SendEventImp(GAEventDataPtr);
}

void UBaseFeatureComponent::ClearData2Self(
	const TMap<ECharacterPropertyType, FBaseProperty>& InModifyPropertyMap, const FGameplayTag& DataSource
)
{
	auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
	if (!OnwerActorPtr)
	{
		return;
	}

	TPair<FOwnerPawnType*, TMap<ECharacterPropertyType, FBaseProperty>>ModifyPropertyMap{ OnwerActorPtr, InModifyPropertyMap };

	ClearData2Other({ ModifyPropertyMap }, DataSource);
}

void UBaseFeatureComponent::ExcuteAttackedEffect(EAffectedDirection AffectedDirection)
{
	FGameplayEventData Payload;
	auto GameplayAbilityTargetDataPtr = new FGameplayAbilityTargetData_Affected;
	GameplayAbilityTargetDataPtr->AffectedDirection = AffectedDirection;

	Payload.TargetData.Add(GameplayAbilityTargetDataPtr);

	auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
	if (OnwerActorPtr)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OnwerActorPtr, UGameplayTagsSubSystem::GetInstance()->Affected, Payload);
	}
}

void UBaseFeatureComponent::SendEvent2Other(
	const TMap<FOwnerPawnType*, TMap<ECharacterPropertyType, FBaseProperty>>& ModifyPropertyMap,
	const FGameplayTag& DataSource
)
{
	auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
	if (!OnwerActorPtr)
	{
		return;
	}

	FGameplayAbilityTargetData_GASendEvent* GAEventDataPtr = new FGameplayAbilityTargetData_GASendEvent(OnwerActorPtr);

	GAEventDataPtr->TriggerCharacterPtr = OnwerActorPtr;

	for (const auto Iter : ModifyPropertyMap)
	{
		FGAEventData GAEventData(Iter.Key, OnwerActorPtr);

		GAEventData.DataSource = DataSource;
		GAEventData.DataModify = Iter.Value;

		GAEventDataPtr->DataAry.Add(GAEventData);
	}
	SendEventImp(GAEventDataPtr);
}

void UBaseFeatureComponent::SendEvent2Self(
	const TMap<ECharacterPropertyType, FBaseProperty>& InModifyPropertyMap,
	const FGameplayTag& DataSource
)
{
	auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
	if (!OnwerActorPtr)
	{
		return;
	}

	TPair<FOwnerPawnType*, TMap<ECharacterPropertyType, FBaseProperty>>ModifyPropertyMap{ OnwerActorPtr, InModifyPropertyMap };

	SendEvent2Other({ ModifyPropertyMap }, DataSource);
}

void UBaseFeatureComponent::InitialBaseGAs()
{
#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
		if (OnwerActorPtr)
		{
			auto GASPtr = OnwerActorPtr->GetAbilitySystemComponent();

			SendEventHandle = GASPtr->GiveAbility(
				FGameplayAbilitySpec(UGAEvent_Send::StaticClass(), 1)
			);

			ReceivedEventHandle = GASPtr->GiveAbility(
				FGameplayAbilitySpec(UGAEvent_Received::StaticClass(), 1)
			);

			// 五行技能
			GASPtr->GiveAbility(
				FGameplayAbilitySpec(
					Skill_Element_GoldClass
				)
			);

			GASPtr->GiveAbility(
				FGameplayAbilitySpec(UBasicFutures_MoveToAttaclArea::StaticClass(), 1)
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
		// 伤害类型
		AddSendBaseModify();

		// 接收

		// 基础属性
		AddReceivedBaseModify();
#pragma endregion 结算效果修正
	}
#endif
}

void UBaseFeatureComponent::SwitchWalkState_Implementation(bool bIsRun)
{
	if (bIsRun)
	{
		auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
		if (OnwerActorPtr)
		{
			if (OnwerActorPtr->GetAbilitySystemComponent()->K2_HasMatchingGameplayTag(UGameplayTagsSubSystem::GetInstance()->State_Locomotion_Run))
			{
				return;
			}

			OnwerActorPtr->GetAbilitySystemComponent()->TryActivateAbilitiesByTag(
				FGameplayTagContainer{ UGameplayTagsSubSystem::GetInstance()->State_Locomotion_Run }
			);
		}
	}
	else
	{
		auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
		if (OnwerActorPtr)
		{
			if (OnwerActorPtr->GetAbilitySystemComponent()->K2_HasMatchingGameplayTag(UGameplayTagsSubSystem::GetInstance()->State_Locomotion_Run))
			{
				FGameplayTagContainer GameplayTagContainer{ UGameplayTagsSubSystem::GetInstance()->State_Locomotion_Run };
				OnwerActorPtr->GetAbilitySystemComponent()->CancelAbilities(&GameplayTagContainer);
			}

			OnwerActorPtr->GetAbilitySystemComponent()->K2_HasMatchingGameplayTag(UGameplayTagsSubSystem::GetInstance()->State_Locomotion_Run);
		}
	}
}

void UBaseFeatureComponent::Dash_Implementation(EDashDirection DashDirection)
{
	FGameplayEventData Payload;
	auto GameplayAbilityTargetData_DashPtr = new FGameplayAbilityTargetData_Dash;
	GameplayAbilityTargetData_DashPtr->DashDirection = DashDirection;

	Payload.TargetData.Add(GameplayAbilityTargetData_DashPtr);

	auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
	if (OnwerActorPtr)
	{
		// 		auto ASCPtr = OnwerActorPtr->GetAbilitySystemComponent();
		// 
		// 		ASCPtr->TriggerAbilityFromGameplayEvent(
		// 			FGameplayAbilitySpecHandle(),
		// 			ASCPtr->AbilityActorInfo.Get(),
		// 			UGameplayTagsSubSystem::GetInstance()->Dash,
		// 			&Payload,
		// 			*ASCPtr
		// 		);	

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			OnwerActorPtr,
			UGameplayTagsSubSystem::GetInstance()->Dash,
			Payload
		);
	}
}

void UBaseFeatureComponent::Jump_Implementation()
{
	auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
	if (OnwerActorPtr)
	{
		OnwerActorPtr->GetAbilitySystemComponent()->TryActivateAbilitiesByTag(
			FGameplayTagContainer{ UGameplayTagsSubSystem::GetInstance()->Jump }
		);
	}
}

void UBaseFeatureComponent::Respawn()
{
	auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
	if (OnwerActorPtr)
	{
		FGameplayEventData Payload;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			OnwerActorPtr,
			UGameplayTagsSubSystem::GetInstance()->BaseFeature_Respawn,
			Payload
		);
	}
}

void UBaseFeatureComponent::MoveToAttackDistance(
	FGameplayAbilityTargetData_MoveToAttaclArea* MoveToAttaclAreaPtr
)
{
	FGameplayEventData Payload;
	Payload.TargetData.Add(MoveToAttaclAreaPtr);

	auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
	if (OnwerActorPtr)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			OnwerActorPtr,
			UGameplayTagsSubSystem::GetInstance()->State_MoveToAttaclArea,
			Payload
		);
	}
}

void UBaseFeatureComponent::BreakMoveToAttackDistance()
{
	auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
	if (OnwerActorPtr)
	{
		auto GASPtr = OnwerActorPtr->GetAbilitySystemComponent();

		FGameplayTagContainer GameplayTagContainer{ UGameplayTagsSubSystem::GetInstance()->State_MoveToAttaclArea };
		GASPtr->CancelAbilities(&GameplayTagContainer);
	}
}

void UBaseFeatureComponent::AddSendBaseModify()
{ 
	{
		struct FMyStruct : public IGAEventModifySendInterface
		{
			FMyStruct(int32 InPriority) :
				IGAEventModifySendInterface(InPriority)
			{
			}

			virtual bool Modify(FGameplayAbilityTargetData_GASendEvent& GameplayAbilityTargetData_GAEvent)override
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

						if (Iter.DataModify.Contains(ECharacterPropertyType::HP))
						{
							Iter.DataModify[ECharacterPropertyType::HP].CurrentValue =
								Iter.DataModify[ECharacterPropertyType::HP].CurrentValue / GameplayAbilityTargetData_GAEvent.DataAry.Num();
						}
					}
				}
				return true;
			}
		};
		AddSendEventModify(MakeShared<FMyStruct>(100));
	}
	{
		struct FMyStruct : public IGAEventModifySendInterface
		{
			FMyStruct(int32 InPriority) :
				IGAEventModifySendInterface(InPriority)
			{
			}

			virtual bool Modify(FGameplayAbilityTargetData_GASendEvent& GameplayAbilityTargetData_GAEvent)override
			{
				for (auto& Iter : GameplayAbilityTargetData_GAEvent.DataAry)
				{
					if (Iter.ElementSet.IsEmpty())
					{
						auto CharacterAttributes =
							GameplayAbilityTargetData_GAEvent.TriggerCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();

						std::map<int32, EWuXingType, std::greater<int>> ElementMap;
						ElementMap.emplace(CharacterAttributes.GoldElement.GetCurrentValue(), EWuXingType::kGold);
						ElementMap.emplace(CharacterAttributes.WoodElement.GetCurrentValue(), EWuXingType::kWood);
						ElementMap.emplace(CharacterAttributes.WaterElement.GetCurrentValue(), EWuXingType::kWater);
						ElementMap.emplace(CharacterAttributes.FireElement.GetCurrentValue(), EWuXingType::kFire);
						ElementMap.emplace(CharacterAttributes.SoilElement.GetCurrentValue(), EWuXingType::kSoil);

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
				return true;
			}
		};
		AddSendEventModify(MakeShared<FMyStruct>(101));
	}
	{
		struct FMyStruct : public IGAEventModifySendInterface
		{
			FMyStruct(int32 InPriority) :
				IGAEventModifySendInterface(InPriority)
			{
			}

			virtual bool Modify(FGameplayAbilityTargetData_GASendEvent& GameplayAbilityTargetData_GAEvent)override
			{
				const auto & CharacterAttributes =
					GameplayAbilityTargetData_GAEvent.TriggerCharacterPtr.Get()->GetCharacterAttributesComponent()->GetCharacterAttributes();
				for (auto& Iter : GameplayAbilityTargetData_GAEvent.DataAry)
				{
					Iter.AD_Penetration += CharacterAttributes.AD_Penetration.GetCurrentValue();
					Iter.AD_PercentPenetration += CharacterAttributes.AD_PercentPenetration.GetCurrentValue();
					Iter.HitRate += CharacterAttributes.HitRate.GetCurrentValue();
					Iter.CriticalHitRate += CharacterAttributes.CriticalHitRate.GetCurrentValue();
					Iter.CriticalDamage += CharacterAttributes.CriticalDamage.GetCurrentValue();
				}
				return true;
			}
		};
		AddSendEventModify(MakeShared<FMyStruct>(102));
	}
}

void UBaseFeatureComponent::AddReceivedBaseModify()
{
	// 元素克制衰减
	{
		struct FMyStruct : public IGAEventModifyReceivedInterface
		{
			FMyStruct(int32 InPriority) :
				IGAEventModifyReceivedInterface(InPriority)
			{
			}

			virtual bool Modify(FGameplayAbilityTargetData_GAReceivedEvent& GameplayAbilityTargetData_GAEvent)override
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
					auto CharacterAttributes =
						DataRef.TargetCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();

					std::map<int32, EWuXingType, std::greater<int>> ElementMap;
					ElementMap.emplace(CharacterAttributes.GoldElement.GetCurrentValue(), EWuXingType::kGold);
					ElementMap.emplace(CharacterAttributes.WoodElement.GetCurrentValue(), EWuXingType::kWood);
					ElementMap.emplace(CharacterAttributes.WaterElement.GetCurrentValue(), EWuXingType::kWater);
					ElementMap.emplace(CharacterAttributes.FireElement.GetCurrentValue(), EWuXingType::kFire);
					ElementMap.emplace(CharacterAttributes.SoilElement.GetCurrentValue(), EWuXingType::kSoil);

					const auto Effective_Rate = Caculation_Effective_Rate(ElementMap.begin()->first, 0);

					DataRef.BaseDamage = DataRef.BaseDamage * Effective_Rate;
				}
				else
				{
					auto CharacterAttributes =
						GameplayAbilityTargetData_GAEvent.TriggerCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();

					for (auto& ElementIter : DataRef.ElementSet)
					{
						switch (ElementIter.Get<0>())
						{
						case EWuXingType::kGold:
						{
							const auto Effective_Rate = Caculation_Effective_Rate(CharacterAttributes.FireElement.GetCurrentValue(), ElementIter.Get<1>());
							ElementIter.Get<2>() = ElementIter.Get<2>() * Effective_Rate;
						}
						break;
						case EWuXingType::kWood:
						{
							const auto Effective_Rate = Caculation_Effective_Rate(CharacterAttributes.GoldElement.GetCurrentValue(), ElementIter.Get<1>());
							ElementIter.Get<2>() = ElementIter.Get<2>() * Effective_Rate;
						}
						break;
						case EWuXingType::kWater:
						{
							const auto Effective_Rate = Caculation_Effective_Rate(CharacterAttributes.SoilElement.GetCurrentValue(), ElementIter.Get<1>());
							ElementIter.Get<2>() = ElementIter.Get<2>() * Effective_Rate;
						}
						break;
						case EWuXingType::kFire:
						{
							const auto Effective_Rate = Caculation_Effective_Rate(CharacterAttributes.WaterElement.GetCurrentValue(), ElementIter.Get<1>());
							ElementIter.Get<2>() = ElementIter.Get<2>() * Effective_Rate;
						}
						break;
						case EWuXingType::kSoil:
						{
							const auto Effective_Rate = Caculation_Effective_Rate(CharacterAttributes.WoodElement.GetCurrentValue(), ElementIter.Get<1>());
							ElementIter.Get<2>() = ElementIter.Get<2>() * Effective_Rate;
						}
						break;
						default:
							break;
						}
					}
				}

				return true;
			}
		};
		AddReceviedEventModify(MakeShared<FMyStruct>(100));
	}

	// 群体效果衰减
	{
		struct FMyStruct : public IGAEventModifyReceivedInterface
		{
			FMyStruct(int32 InPriority) :
				IGAEventModifyReceivedInterface(InPriority)
			{
			}

			virtual bool Modify(FGameplayAbilityTargetData_GAReceivedEvent& GameplayAbilityTargetData_GAEvent)override
			{
				if (GameplayAbilityTargetData_GAEvent.Data.TargetCharacterPtr.IsValid())
				{
					auto SelfCharacterAttributesSPtr =
						GameplayAbilityTargetData_GAEvent.Data.TargetCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();

					auto TargetCharacterAttributesSPtr =
						GameplayAbilityTargetData_GAEvent.TriggerCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();

					{
						const auto Rate = (TargetCharacterAttributesSPtr.HitRate.GetCurrentValue() - TargetCharacterAttributesSPtr.Evade.GetCurrentValue()) /
							static_cast<float>(TargetCharacterAttributesSPtr.HitRate.GetMaxValue());

						GameplayAbilityTargetData_GAEvent.Data.HitRate = FMath::FRand() <= Rate ? 100 : 0;
					}
				}

				return true;
			}
		};
		AddReceviedEventModify(MakeShared<FMyStruct>(101));
	}
}

void UBaseFeatureComponent::OnSendEventModifyData(FGameplayAbilityTargetData_GASendEvent& OutGAEventData)
{
	for (auto Iter : SendEventModifysMap)
	{
		Iter->Modify(OutGAEventData);
	}
}

void UBaseFeatureComponent::OnReceivedEventModifyData(FGameplayAbilityTargetData_GAReceivedEvent& OutGAEventData)
{
	TArray<decltype(ReceivedEventModifysMap)::iterator> NeedRemoveIterAry;;

	for (auto Iter = ReceivedEventModifysMap.begin();Iter != ReceivedEventModifysMap.end(); Iter++)
	{
		if ((*Iter)->Modify(OutGAEventData) && (*Iter)->bIsOnceTime)
		{
			NeedRemoveIterAry.Add(Iter);
		}
	}

	for (auto Iter : NeedRemoveIterAry)
	{
		ReceivedEventModifysMap.erase(Iter);
	}
}

TMap<ECharacterPropertyType, FBaseProperty> GetAllData()
{
	TMap<ECharacterPropertyType, FBaseProperty>Result;

	Result.Add(ECharacterPropertyType::LiDao, 0);
	Result.Add(ECharacterPropertyType::GenGu, 0);
	Result.Add(ECharacterPropertyType::ShenFa, 0);
	Result.Add(ECharacterPropertyType::DongCha, 0);
	Result.Add(ECharacterPropertyType::TianZi, 0);

	Result.Add(ECharacterPropertyType::GoldElement, 0);
	Result.Add(ECharacterPropertyType::WoodElement, 0);
	Result.Add(ECharacterPropertyType::WaterElement, 0);
	Result.Add(ECharacterPropertyType::FireElement, 0);
	Result.Add(ECharacterPropertyType::SoilElement, 0);

	Result.Add(ECharacterPropertyType::AD, 0);
	Result.Add(ECharacterPropertyType::AD_Penetration, 0);
	Result.Add(ECharacterPropertyType::AD_PercentPenetration, 0);
	Result.Add(ECharacterPropertyType::Resistance, 0);
	Result.Add(ECharacterPropertyType::GAPerformSpeed, 0);
	Result.Add(ECharacterPropertyType::Evade, 0);
	Result.Add(ECharacterPropertyType::HitRate, 0);
	Result.Add(ECharacterPropertyType::Toughness, 0);
	Result.Add(ECharacterPropertyType::CriticalHitRate, 0);
	Result.Add(ECharacterPropertyType::CriticalDamage, 0);
	Result.Add(ECharacterPropertyType::MoveSpeed, 0);

	return Result;
}
