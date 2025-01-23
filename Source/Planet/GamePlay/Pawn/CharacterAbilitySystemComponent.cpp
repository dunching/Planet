#include "CharacterAbilitySystemComponent.h"

#include <queue>
#include <map>

#include "GameplayAbilitySpec.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AssetRefMap.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayEffectExecutionCalculation.h"
#include "AttributeSet.h"

#include "GAEvent_Helper.h"
#include "CharacterBase.h"
#include "CharacterAttributesComponent.h"
#include "GenerateType.h"
#include "PlanetPlayerState.h"
#include "GAEvent_Send.h"
#include "GAEvent_Received.h"
#include "Skill_Base.h"
#include "CS_RootMotion.h"
#include "CS_PeriodicPropertyModify.h"
#include "AS_Character.h"
#include "HumanRegularProcessor.h"
#include "GameplayTagsLibrary.h"
#include "BasicFutures_Dash.h"
#include "BasicFutures_MoveToAttaclArea.h"
#include "BasicFutures_Affected.h"
#include "CS_PeriodicStateModify.h"
#include "CS_PeriodicPropertyTag.h"
#include "CharacterTitle.h"
#include "GameOptions.h"
#include "GE_Common.h"

UCharacterAbilitySystemComponent::UCharacterAbilitySystemComponent(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
	SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
}

void UCharacterAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();

#if UE_EDITOR || UE_SERVER
	if (GetOwnerRole() == ROLE_Authority)
	{
		OnGameplayEffectAppliedDelegateToTarget.AddUObject(
			this, &ThisClass::OnGEAppliedDelegateToTarget);
		OnGameplayEffectAppliedDelegateToSelf.AddUObject(
			this, &ThisClass::OnActiveGEAddedDelegateToSelf);
	}
#endif
}

bool UCharacterAbilitySystemComponent::IsInDeath() const
{
	auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
	return OnwerActorPtr->GetCharacterAbilitySystemComponent()->HasMatchingGameplayTag(
		UGameplayTagsLibrary::DeathingTag
	);
}

bool UCharacterAbilitySystemComponent::IsUnSelected() const
{
	return false;
}

bool UCharacterAbilitySystemComponent::IsRunning() const
{
	auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
	return OnwerActorPtr->GetCharacterAbilitySystemComponent()->K2_HasMatchingGameplayTag(
		UGameplayTagsLibrary::State_Locomotion_Run
	);
}

bool UCharacterAbilitySystemComponent::IsRootMotion() const
{
	auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
	return OnwerActorPtr->GetCharacterMovement()->HasRootMotionSources();
}

bool UCharacterAbilitySystemComponent::IsInFighting() const
{
	auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
	return OnwerActorPtr->GetCharacterAbilitySystemComponent()->HasMatchingGameplayTag(
		UGameplayTagsLibrary::InFightingTag
	);
}

void UCharacterAbilitySystemComponent::AddSendEventModify(const TSharedPtr<IGAEventModifySendInterface>& GAEventModifySPtr)
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

void UCharacterAbilitySystemComponent::RemoveSendEventModify(const TSharedPtr<IGAEventModifySendInterface>& GAEventModifySPtr)
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

void UCharacterAbilitySystemComponent::AddReceviedEventModify(const TSharedPtr<IGAEventModifyReceivedInterface>& GAEventModifySPtr)
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

void UCharacterAbilitySystemComponent::RemoveReceviedEventModify(
	const TSharedPtr<IGAEventModifyReceivedInterface>& GAEventModifySPtr)
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

void UCharacterAbilitySystemComponent::SendEventImp(
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
		auto ASCPtr = OnwerActorPtr->GetCharacterAbilitySystemComponent();
		ASCPtr->GiveAbilityAndActivateOnce(
			Spec,
			Payload
		);
	}
#endif
}

void UCharacterAbilitySystemComponent::SendEventImp(
	FGameplayAbilityTargetData_GASendEvent* GAEventDataPtr
)
{
#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		if (GAEventDataPtr)
		{
			for (auto& Iter : GAEventDataPtr->DataAry)
			{
				if (!Iter.DataSource.IsValid())
				{
					Iter.DataSource = UGameplayTagsLibrary::DataSource_Character;;
				}
			}
		}

		auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
		if (OnwerActorPtr)
		{
			FGameplayEventData Payload;

			Payload.TargetData.Add(new FGameplayAbilityTargetData_GAEventType(EEventType::kNormal));
			Payload.TargetData.Add(GAEventDataPtr);

			auto ASCPtr = OnwerActorPtr->GetCharacterAbilitySystemComponent();
			ASCPtr->TriggerAbilityFromGameplayEvent(
				SendEventHandle,
				ASCPtr->AbilityActorInfo.Get(),
				UGameplayTagsLibrary::BaseFeature_Send,
				&Payload,
				*ASCPtr
			);
		}
	}
#endif
}

void UCharacterAbilitySystemComponent::SendEventImp(
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

			auto ASCPtr = OnwerActorPtr->GetCharacterAbilitySystemComponent();
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

void UCharacterAbilitySystemComponent::SendEventImp(
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

			auto ASCPtr = OnwerActorPtr->GetCharacterAbilitySystemComponent();
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

void UCharacterAbilitySystemComponent::SendEventImp(
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

			auto ASCPtr = OnwerActorPtr->GetCharacterAbilitySystemComponent();
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

void UCharacterAbilitySystemComponent::ClearData2Other(
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
		GAEventData.bIsClearData = true;

		GAEventDataPtr->DataAry.Add(GAEventData);
	}
	SendEventImp(GAEventDataPtr);
}

void UCharacterAbilitySystemComponent::ClearData2Self(
	const TMap<ECharacterPropertyType, FBaseProperty>& InModifyPropertyMap, const FGameplayTag& DataSource
)
{
	auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
	if (!OnwerActorPtr)
	{
		return;
	}

	TPair<FOwnerPawnType*, TMap<ECharacterPropertyType, FBaseProperty>> ModifyPropertyMap{
		OnwerActorPtr, InModifyPropertyMap
	};

	ClearData2Other({ModifyPropertyMap}, DataSource);
}

void UCharacterAbilitySystemComponent::ExcuteAttackedEffect(const FGameplayAbilityTargetData_GAReceivedEvent& GAEvent)
{
	const auto IsNotDeathingTag = !IsInDeath();
	if (IsNotDeathingTag && GAEvent.Data.GetIsHited())
	{
		switch (GAEvent.Data.AttackEffectType)
		{
		case EAttackEffectType::kNone:
			break;
		case EAttackEffectType::kNormalAttackEffect:
			{
				FGameplayEventData Payload;
				auto GameplayAbilityTargetDataPtr = new FGameplayAbilityTargetData_Affected;
				GameplayAbilityTargetDataPtr->AffectedDirection =
					GetAffectedDirection(GAEvent.Data.TargetCharacterPtr.Get(), GAEvent.TriggerCharacterPtr.Get());

				Payload.TargetData.Add(GameplayAbilityTargetDataPtr);

				auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
				if (OnwerActorPtr)
				{
					UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
						OnwerActorPtr, UGameplayTagsLibrary::Affected, Payload
					);
				}
			}
			break;
		case EAttackEffectType::kAttackEffectAndRepel:
			{
				FGameplayEventData Payload;
				auto GameplayAbilityTargetDataPtr = new FGameplayAbilityTargetData_Affected;
				GameplayAbilityTargetDataPtr->AffectedDirection =
					GetAffectedDirection(GAEvent.Data.TargetCharacterPtr.Get(), GAEvent.TriggerCharacterPtr.Get());
				GameplayAbilityTargetDataPtr->RepelDistance = GAEvent.Data.RepelDistance > 0
					                                              ? GAEvent.Data.RepelDistance
					                                              : 50;
				if (!GAEvent.Data.RepelDirection.IsNearlyZero())
				{
					GameplayAbilityTargetDataPtr->RepelDirection = GAEvent.Data.RepelDirection;
				}
				GameplayAbilityTargetDataPtr->TriggerCharacterPtr = GAEvent.Data.TriggerCharacterPtr.Get();

				Payload.TargetData.Add(GameplayAbilityTargetDataPtr);

				auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
				if (OnwerActorPtr)
				{
					UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
						OnwerActorPtr, UGameplayTagsLibrary::Affected, Payload
					);
				}
			}
			break;
		}
	}
}

void UCharacterAbilitySystemComponent::SendEvent2Other(
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

void UCharacterAbilitySystemComponent::SendEvent2Self(
	const TMap<ECharacterPropertyType, FBaseProperty>& InModifyPropertyMap,
	const FGameplayTag& DataSource
)
{
	auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
	if (!OnwerActorPtr)
	{
		return;
	}

	TPair<FOwnerPawnType*, TMap<ECharacterPropertyType, FBaseProperty>> ModifyPropertyMap{
		OnwerActorPtr, InModifyPropertyMap
	};

	SendEvent2Other({ModifyPropertyMap}, DataSource);
}

void UCharacterAbilitySystemComponent::InitialBaseGAs()
{
#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
		if (OnwerActorPtr)
		{
			auto GASPtr = OnwerActorPtr->GetCharacterAbilitySystemComponent();

			// SendEventHandle = GASPtr->GiveAbility(
			// 	FGameplayAbilitySpec(UGAEvent_Send::StaticClass(), 1)
			// );
			//
			// ReceivedEventHandle = GASPtr->GiveAbility(
			// 	FGameplayAbilitySpec(UGAEvent_Received::StaticClass(), 1)
			// );

			// 五行技能
			// GASPtr->GiveAbility(
			// 	FGameplayAbilitySpec(
			// 		Skill_Element_GoldClass
			// 	)
			// );

			// 			MoveToAttaclAreaHandle = GASPtr->GiveAbility(
			// 				FGameplayAbilitySpec(UBasicFutures_MoveToAttaclArea::StaticClass(), 1)
			// 			);
		}

		for (auto Iter : CharacterAbilitiesAry)
		{
			GiveAbility(
				FGameplayAbilitySpec(Iter, 1)
			);
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

void UCharacterAbilitySystemComponent::SwitchWalkState_Implementation(bool bIsRun)
{
	if (bIsRun)
	{
		auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
		if (OnwerActorPtr)
		{
			if (OnwerActorPtr->GetCharacterAbilitySystemComponent()->K2_HasMatchingGameplayTag(
				UGameplayTagsLibrary::State_Locomotion_Run))
			{
				return;
			}

			OnwerActorPtr->GetCharacterAbilitySystemComponent()->TryActivateAbilitiesByTag(
				FGameplayTagContainer{UGameplayTagsLibrary::State_Locomotion_Run}
			);
		}
	}
	else
	{
		auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
		if (OnwerActorPtr)
		{
			if (OnwerActorPtr->GetCharacterAbilitySystemComponent()->K2_HasMatchingGameplayTag(
				UGameplayTagsLibrary::State_Locomotion_Run))
			{
				FGameplayTagContainer GameplayTagContainer{UGameplayTagsLibrary::State_Locomotion_Run};
				OnwerActorPtr->GetCharacterAbilitySystemComponent()->CancelAbilities(&GameplayTagContainer);
			}

			OnwerActorPtr->GetCharacterAbilitySystemComponent()->K2_HasMatchingGameplayTag(
				UGameplayTagsLibrary::State_Locomotion_Run);
		}
	}
}

void UCharacterAbilitySystemComponent::Dash_Implementation(EDashDirection DashDirection)
{
	FGameplayEventData Payload;
	auto GameplayAbilityTargetData_DashPtr = new FGameplayAbilityTargetData_Dash;
	GameplayAbilityTargetData_DashPtr->DashDirection = DashDirection;

	Payload.TargetData.Add(GameplayAbilityTargetData_DashPtr);

	auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
	if (OnwerActorPtr)
	{
		// 		auto ASCPtr = OnwerActorPtr->GetCharacterAbilitySystemComponent();
		// 
		// 		ASCPtr->TriggerAbilityFromGameplayEvent(
		// 			FGameplayAbilitySpecHandle(),
		// 			ASCPtr->AbilityActorInfo.Get(),
		// 			UGameplayTagsLibrary::Dash,
		// 			&Payload,
		// 			*ASCPtr
		// 		);	

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			OnwerActorPtr,
			UGameplayTagsLibrary::Dash,
			Payload
		);
	}
}

void UCharacterAbilitySystemComponent::Jump_Implementation()
{
	auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
	if (OnwerActorPtr)
	{
		OnwerActorPtr->GetCharacterAbilitySystemComponent()->TryActivateAbilitiesByTag(
			FGameplayTagContainer{UGameplayTagsLibrary::Jump}
		);
	}
}

void UCharacterAbilitySystemComponent::SwitchCantBeSelect(bool bIsCanBeSelect)
{
	auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
	if (OnwerActorPtr)
	{
#if UE_EDITOR || UE_CLIENT
		if (GetOwnerRole() < ROLE_Authority)
		{
			// TODO
		}
#endif
	}
}

void UCharacterAbilitySystemComponent::Respawn()
{
	auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
	if (OnwerActorPtr)
	{
		FGameplayEventData Payload;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			OnwerActorPtr,
			UGameplayTagsLibrary::BaseFeature_Respawn,
			Payload
		);
	}
}

void UCharacterAbilitySystemComponent::MoveToAttackDistance(
	FGameplayAbilityTargetData_MoveToAttaclArea* MoveToAttaclAreaPtr
)
{
	FGameplayEventData Payload;
	Payload.TargetData.Add(MoveToAttaclAreaPtr);

	auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
	if (OnwerActorPtr)
	{
		// 		auto ASCPtr = OnwerActorPtr->GetCharacterAbilitySystemComponent();
		// 		ASCPtr->TriggerAbilityFromGameplayEvent(
		// 			MoveToAttaclAreaHandle,
		// 			ASCPtr->AbilityActorInfo.Get(),
		// 			UGameplayTagsLibrary::State_MoveToAttaclArea,
		// 			&Payload,
		// 			*ASCPtr
		// 		);

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			OnwerActorPtr,
			UGameplayTagsLibrary::State_MoveToAttaclArea,
			Payload
		);
	}
}

void UCharacterAbilitySystemComponent::BreakMoveToAttackDistance()
{
	auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
	if (OnwerActorPtr)
	{
		auto GASPtr = OnwerActorPtr->GetCharacterAbilitySystemComponent();

		FGameplayTagContainer GameplayTagContainer{UGameplayTagsLibrary::State_MoveToAttaclArea};
		GASPtr->CancelAbilities(&GameplayTagContainer);
	}
}

void UCharacterAbilitySystemComponent::OnGroupSharedInfoReady(AGroupSharedInfo* NewGroupSharedInfoPtr)
{
#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		// auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
		// auto GASPtr = OnwerActorPtr->GetCharacterAbilitySystemComponent();

		// GA全部通过Server注册
		ClearAllAbilities();
		InitialBaseGAs();
	}
#endif
}

void UCharacterAbilitySystemComponent::AddSendBaseModify()
{
#if UE_EDITOR || UE_SERVER
	if (GetNetMode() < NM_DedicatedServer)
	{
		return;
	}
#endif

	{
		// 群体伤害衰减
		struct FMyStruct : public IGAEventModifySendInterface
		{
			FMyStruct(int32 InPriority) :
				IGAEventModifySendInterface(InPriority)
			{
			}

			virtual bool Modify(TMap<FGameplayTag, float>& SetByCallerTagMagnitudes) override
			{
				// if (GameplayAbilityTargetData_GAEvent.DataAry.Num() > 1)
				// {
				// 	for (auto& Iter : GameplayAbilityTargetData_GAEvent.DataAry)
				// 	{
				// 		Iter.TrueDamage =
				// 			Iter.TrueDamage / GameplayAbilityTargetData_GAEvent.DataAry.Num();
				//
				// 		Iter.BaseDamage =
				// 			Iter.BaseDamage / GameplayAbilityTargetData_GAEvent.DataAry.Num();
				//
				// 		for (auto& ElementIter : Iter.ElementSet)
				// 		{
				// 			ElementIter.Get<2>() =
				// 				ElementIter.Get<2>() / GameplayAbilityTargetData_GAEvent.DataAry.Num();
				// 		}
				//
				// 		if (Iter.DataModify.Contains(ECharacterPropertyType::HP))
				// 		{
				// 			Iter.DataModify[ECharacterPropertyType::HP].CurrentValue =
				// 				Iter.DataModify[ECharacterPropertyType::HP].CurrentValue /
				// 				GameplayAbilityTargetData_GAEvent.DataAry.Num();
				// 		}
				// 	}
				// }
				return true;
			}
		};
		AddSendEventModify(MakeShared<FMyStruct>(100));
	}
	{
		// 元素伤害
		struct FMyStruct : public IGAEventModifySendInterface
		{
			FMyStruct(int32 InPriority) :
				IGAEventModifySendInterface(InPriority)
			{
			}

			virtual bool Modify(TMap<FGameplayTag, float>& SetByCallerTagMagnitudes) override
			{
				// for (auto& Iter : GameplayAbilityTargetData_GAEvent.DataAry)
				// {
				// 	if (Iter.ElementSet.IsEmpty())
				// 	{
				// 		const auto& CharacterAttributes =
				// 			GameplayAbilityTargetData_GAEvent.TriggerCharacterPtr->GetCharacterAttributesComponent()->
				// 			                                  GetCharacterAttributes();
				//
				// 		std::map<int32, EWuXingType, std::greater<int>> ElementMap;
				// 		// ElementMap.emplace(CharacterAttributes.GoldElement.GetCurrentValue(), EWuXingType::kGold);
				// 		// ElementMap.emplace(CharacterAttributes.WoodElement.GetCurrentValue(), EWuXingType::kWood);
				// 		// ElementMap.emplace(CharacterAttributes.WaterElement.GetCurrentValue(), EWuXingType::kWater);
				// 		// ElementMap.emplace(CharacterAttributes.FireElement.GetCurrentValue(), EWuXingType::kFire);
				// 		// ElementMap.emplace(CharacterAttributes.SoilElement.GetCurrentValue(), EWuXingType::kSoil);
				//
				// 		if (ElementMap.begin()->first > 0)
				// 		{
				// 			const auto Tuple = MakeTuple(
				// 				ElementMap.begin()->second,
				// 				ElementMap.begin()->first,
				// 				Iter.BaseDamage
				// 			);
				// 			Iter.ElementSet.Add(Tuple);
				// 		}
				// 	}
				// }
				return true;
			}
		};
		AddSendEventModify(MakeShared<FMyStruct>(101));
	}
	{
		// 自身属性
		struct FMyStruct : public IGAEventModifySendInterface
		{
			FMyStruct(int32 InPriority) :
				IGAEventModifySendInterface(InPriority)
			{
			}

			virtual bool Modify(TMap<FGameplayTag, float>& SetByCallerTagMagnitudes) override
			{
				// const auto& CharacterAttributes =
				// 	GameplayAbilityTargetData_GAEvent.TriggerCharacterPtr.Get()->GetCharacterAttributesComponent()->
				// 	                                  GetCharacterAttributes();
				// for (auto& Iter : GameplayAbilityTargetData_GAEvent.DataAry)
				// {
				// 	// Iter.AD_Penetration += CharacterAttributes.AD_Penetration.GetCurrentValue();
				// 	// Iter.AD_PercentPenetration += CharacterAttributes.AD_PercentPenetration.GetCurrentValue();
				// 	// Iter.HitRate += CharacterAttributes.HitRate.GetCurrentValue();
				// 	// Iter.CriticalHitRate += CharacterAttributes.CriticalHitRate.GetCurrentValue();
				// 	// Iter.CriticalDamage += CharacterAttributes.CriticalDamage.GetCurrentValue();
				// }
				return true;
			}
		};
		AddSendEventModify(MakeShared<FMyStruct>(102));
	}
}

void UCharacterAbilitySystemComponent::AddReceivedBaseModify()
{
#if UE_EDITOR || UE_SERVER
	if (GetNetMode() < NM_DedicatedServer)
	{
		return;
	}
#endif

	// // 元素克制衰减
	// {
	// 	struct FMyStruct : public IGAEventModifyReceivedInterface
	// 	{
	// 		FMyStruct(int32 InPriority) :
	// 			IGAEventModifyReceivedInterface(InPriority)
	// 		{
	// 		}
	//
	// 		virtual bool Modify(FGameplayAbilityTargetData_GAReceivedEvent& GameplayAbilityTargetData_GAEvent)override
	// 		{
	// 			const auto Caculation_Effective_Rate = [](int32 SelfLevel, int32 TargetLevel) {
	//
	// 				// 
	// 				const auto Offset = (SelfLevel - TargetLevel) / 3;
	// 				const auto Effective_Rate = 1.f + (Offset * 0.25f);
	// 				return Effective_Rate;
	// 				};
	//
	// 			auto& DataRef = GameplayAbilityTargetData_GAEvent.Data;
	// 			if (DataRef.ElementSet.IsEmpty() && DataRef.TargetCharacterPtr.IsValid())
	// 			{
	// 				auto CharacterAttributes =
	// 					DataRef.TargetCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
	//
	// 				std::map<int32, EWuXingType, std::greater<int>> ElementMap;
	// 				// ElementMap.emplace(CharacterAttributes.GoldElement.GetCurrentValue(), EWuXingType::kGold);
	// 				// ElementMap.emplace(CharacterAttributes.WoodElement.GetCurrentValue(), EWuXingType::kWood);
	// 				// ElementMap.emplace(CharacterAttributes.WaterElement.GetCurrentValue(), EWuXingType::kWater);
	// 				// ElementMap.emplace(CharacterAttributes.FireElement.GetCurrentValue(), EWuXingType::kFire);
	// 				// ElementMap.emplace(CharacterAttributes.SoilElement.GetCurrentValue(), EWuXingType::kSoil);
	//
	// 				const auto Effective_Rate = Caculation_Effective_Rate(ElementMap.begin()->first, 0);
	//
	// 				DataRef.BaseDamage = DataRef.BaseDamage * Effective_Rate;
	// 			}
	// 			else
	// 			{
	// 				const auto& CharacterAttributes =
	// 					GameplayAbilityTargetData_GAEvent.TriggerCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
	//
	// 				for (auto& ElementIter : DataRef.ElementSet)
	// 				{
	// 					// switch (ElementIter.Get<0>())
	// 					// {
	// 					// case EWuXingType::kGold:
	// 					// {
	// 					// 	const auto Effective_Rate = Caculation_Effective_Rate(CharacterAttributes.FireElement.GetCurrentValue(), ElementIter.Get<1>());
	// 					// 	ElementIter.Get<2>() = ElementIter.Get<2>() * Effective_Rate;
	// 					// }
	// 					// break;
	// 					// case EWuXingType::kWood:
	// 					// {
	// 					// 	const auto Effective_Rate = Caculation_Effective_Rate(CharacterAttributes.GoldElement.GetCurrentValue(), ElementIter.Get<1>());
	// 					// 	ElementIter.Get<2>() = ElementIter.Get<2>() * Effective_Rate;
	// 					// }
	// 					// break;
	// 					// case EWuXingType::kWater:
	// 					// {
	// 					// 	const auto Effective_Rate = Caculation_Effective_Rate(CharacterAttributes.SoilElement.GetCurrentValue(), ElementIter.Get<1>());
	// 					// 	ElementIter.Get<2>() = ElementIter.Get<2>() * Effective_Rate;
	// 					// }
	// 					// break;
	// 					// case EWuXingType::kFire:
	// 					// {
	// 					// 	const auto Effective_Rate = Caculation_Effective_Rate(CharacterAttributes.WaterElement.GetCurrentValue(), ElementIter.Get<1>());
	// 					// 	ElementIter.Get<2>() = ElementIter.Get<2>() * Effective_Rate;
	// 					// }
	// 					// break;
	// 					// case EWuXingType::kSoil:
	// 					// {
	// 					// 	const auto Effective_Rate = Caculation_Effective_Rate(CharacterAttributes.WoodElement.GetCurrentValue(), ElementIter.Get<1>());
	// 					// 	ElementIter.Get<2>() = ElementIter.Get<2>() * Effective_Rate;
	// 					// }
	// 					// break;
	// 					// default:
	// 					// 	break;
	// 					// }
	// 				}
	// 			}
	//
	// 			return true;
	// 		}
	// 	};
	// 	AddReceviedEventModify(MakeShared<FMyStruct>(100));
	// }
	//
	// // 确认概率类型数值
	// {
	// 	struct FMyStruct : public IGAEventModifyReceivedInterface
	// 	{
	// 		FMyStruct(int32 InPriority) :
	// 			IGAEventModifyReceivedInterface(InPriority)
	// 		{
	// 		}
	//
	// 		virtual bool Modify(FGameplayAbilityTargetData_GAReceivedEvent& GameplayAbilityTargetData_GAEvent)override
	// 		{
	// 			if (GameplayAbilityTargetData_GAEvent.Data.TargetCharacterPtr.IsValid())
	// 			{
	// 				auto Lambda = [&GameplayAbilityTargetData_GAEvent]
	// 					{
	// 						// auto& SelfCharacterAttributesRef =
	// 						// 	GameplayAbilityTargetData_GAEvent.Data.TargetCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
	// 						//
	// 						// const auto& TargetCharacterAttributesRef =
	// 						// 	GameplayAbilityTargetData_GAEvent.TriggerCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
	// 						//
	// 						// // 命中率
	// 						// {
	// 						// 	if (GameplayAbilityTargetData_GAEvent.Data.bIsCantEvade)
	// 						// 	{
	// 						// 		const auto TrigggerHitRate = TargetCharacterAttributesRef.HitRate.GetCurrentValue() + GameplayAbilityTargetData_GAEvent.Data.HitRate;
	// 						// 		const auto SelfEvadeRate = TargetCharacterAttributesRef.Evade.GetCurrentValue();
	// 						//
	// 						// 		const auto HitRate =
	// 						// 			(
	// 						// 				TrigggerHitRate *
	// 						// 				(100 - SelfEvadeRate)
	// 						// 				) / 100;
	// 						//
	// 						// 		GameplayAbilityTargetData_GAEvent.Data.HitRate = 100;
	// 						// 	}
	// 						// 	else
	// 						// 	{
	// 						// 		const auto TrigggerHitRate = TargetCharacterAttributesRef.HitRate.GetCurrentValue() + GameplayAbilityTargetData_GAEvent.Data.HitRate;
	// 						// 		const auto SelfEvadeRate = TargetCharacterAttributesRef.Evade.GetCurrentValue();
	// 						//
	// 						// 		const auto HitRate =
	// 						// 			(
	// 						// 				TrigggerHitRate *
	// 						// 				(100 - SelfEvadeRate)
	// 						// 				) / 100;
	// 						//
	// 						// 		GameplayAbilityTargetData_GAEvent.Data.HitRate =
	// 						// 			((FMath::RandRange(0, 100) <= HitRate) ? 100 : 0);
	// 						// 	}
	// 						//}
	//
	// 						// 会心伤害
	// 						{
	// 							if (GameplayAbilityTargetData_GAEvent.Data.GetIsHited())
	// 							{
	// 								// GameplayAbilityTargetData_GAEvent.Data.CriticalHitRate =
	// 								// 	((FMath::RandRange(0, 100) <= TargetCharacterAttributesRef.CriticalHitRate.GetCurrentValue()) ? 100 : 0);
	// 								//
	// 								// if (GameplayAbilityTargetData_GAEvent.Data.GetIsCriticalHited())
	// 								// {
	// 								// 	const float CurCriticalDamage = (100 + GameplayAbilityTargetData_GAEvent.Data.CriticalDamage) / 100.f;
	// 								//
	// 								// 	GameplayAbilityTargetData_GAEvent.Data.BaseDamage = GameplayAbilityTargetData_GAEvent.Data.BaseDamage * CurCriticalDamage;
	// 								// 	GameplayAbilityTargetData_GAEvent.Data.TrueDamage = GameplayAbilityTargetData_GAEvent.Data.TrueDamage * CurCriticalDamage;
	// 								// 	for (auto& Iter : GameplayAbilityTargetData_GAEvent.Data.ElementSet)
	// 								// 	{
	// 								// 		Iter.Get<2>() = Iter.Get<2>() * CurCriticalDamage;
	// 								// 	}
	// 								// }
	// 							}
	// 						}
	// 					};
	//
	// 				if (GameplayAbilityTargetData_GAEvent.Data.BaseDamage > 0)
	// 				{
	// 					Lambda();
	// 				}
	// 				else if (GameplayAbilityTargetData_GAEvent.Data.TrueDamage > 0)
	// 				{
	// 					Lambda();
	// 				}
	// 				else
	// 				{
	// 					for (auto& Iter : GameplayAbilityTargetData_GAEvent.Data.ElementSet)
	// 					{
	// 						if (Iter.Get<2>() > 0)
	// 						{
	// 							Lambda();
	// 							break;;
	// 						}
	// 					}
	// 				}
	// 			}
	//
	// 			return true;
	// 		}
	// 	};
	// 	AddReceviedEventModify(MakeShared<FMyStruct>(101));
	// }
	//
	// // 抗性
	// {
	// 	struct FMyStruct : public IGAEventModifyReceivedInterface
	// 	{
	// 		FMyStruct(int32 InPriority) :
	// 			IGAEventModifyReceivedInterface(InPriority)
	// 		{
	// 		}
	//
	// 		virtual bool Modify(FGameplayAbilityTargetData_GAReceivedEvent& GameplayAbilityTargetData_GAEvent)override
	// 		{
	// 			if (GameplayAbilityTargetData_GAEvent.Data.TargetCharacterPtr.IsValid())
	// 			{
	// 				// auto& SelfCharacterAttributesRef =
	// 				// 	GameplayAbilityTargetData_GAEvent.Data.TargetCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
	// 				//
	// 				// const auto& TargetCharacterAttributesRef =
	// 				// 	GameplayAbilityTargetData_GAEvent.TriggerCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
	// 				//
	// 				// if (
	// 				// 	(GameplayAbilityTargetData_GAEvent.Data.GetIsHited())
	// 				// 	)
	// 				// {
	// 				// 	const auto Self_Resistance = SelfCharacterAttributesRef.AD_Resistance.GetCurrentValue();
	// 				// 	const auto SelfCurrent_Resistance = FMath::Max(
	// 				// 		0,
	// 				// 		Self_Resistance -
	// 				// 		TargetCharacterAttributesRef.AD_Penetration.GetCurrentValue() -
	// 				// 		((TargetCharacterAttributesRef.AD_PercentPenetration.GetCurrentValue() / 100.f) * Self_Resistance)
	// 				// 	);
	// 				// 	const float DR = 1 - (SelfCurrent_Resistance / (SelfCurrent_Resistance + 100.f));
	// 				//
	// 				// 	auto& Ref = GameplayAbilityTargetData_GAEvent.Data.BaseDamage;
	// 				// 	if (Ref > 0)
	// 				// 	{
	// 				// 		Ref = Ref * DR;
	// 				// 	}
	// 				// }
	// 			}
	//
	// 			return true;
	// 		}
	// 	};
	// 	AddReceviedEventModify(MakeShared<FMyStruct>(102));
	// }
	//
	// // 护盾扣除
	// {
	// 	struct FMyStruct : public IGAEventModifyReceivedInterface
	// 	{
	// 		FMyStruct(int32 InPriority) :
	// 			IGAEventModifyReceivedInterface(InPriority)
	// 		{
	// 		}
	//
	// 		virtual bool Modify(FGameplayAbilityTargetData_GAReceivedEvent& GameplayAbilityTargetData_GAEvent)override
	// 		{
	// 			if (GameplayAbilityTargetData_GAEvent.Data.TargetCharacterPtr.IsValid())
	// 			{
	// 				// auto& SelfCharacterAttributesRef =
	// 				// 	GameplayAbilityTargetData_GAEvent.Data.TargetCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
	// 				//
	// 				// const auto& TargetCharacterAttributesRef =
	// 				// 	GameplayAbilityTargetData_GAEvent.TriggerCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
	// 				//
	// 				// auto ShieldValue = SelfCharacterAttributesRef.Shield.GetCurrentValue();
	// 				//
	// 				// if (
	// 				// 	(ShieldValue > 0) &&
	// 				// 	(GameplayAbilityTargetData_GAEvent.Data.GetIsHited())
	// 				// 	)
	// 				// {
	// 				// 	auto& Ref = GameplayAbilityTargetData_GAEvent.Data.BaseDamage;
	// 				// 	if (Ref > 0)
	// 				// 	{
	// 				// 		if (ShieldValue > Ref)
	// 				// 		{
	// 				// 			Ref = 0;
	// 				// 			ShieldValue = ShieldValue - Ref;
	// 				// 		}
	// 				// 		else if (ShieldValue <= Ref)
	// 				// 		{
	// 				// 			Ref = Ref - ShieldValue;
	// 				// 			ShieldValue = 0;
	// 				// 		}
	// 				// 	}
	// 				//
	// 				// 	SelfCharacterAttributesRef.Shield.SetValue(ShieldValue);
	// 				// }
	// 			}
	//
	// 			return true;
	// 		}
	// 	};
	// 	AddReceviedEventModify(MakeShared<FMyStruct>(150));
	// }
}

EAffectedDirection UCharacterAbilitySystemComponent::GetAffectedDirection(ACharacterBase* TargetCharacterPtr,
                                                               ACharacterBase* TriggerCharacterPtr
)
{
	const FVector Vec = (TriggerCharacterPtr->GetActorLocation() - TargetCharacterPtr->GetActorLocation()).
		GetSafeNormal();

	auto ForwardDot = FVector::DotProduct(Vec, TargetCharacterPtr->GetActorForwardVector());
	if (ForwardDot > .5f)
	{
		return EAffectedDirection::kForward;
	}
	else if (ForwardDot < -.5f)
	{
		return EAffectedDirection::kBackward;
	}

	auto RightDot = FVector::DotProduct(Vec, TargetCharacterPtr->GetActorRightVector());
	if (RightDot > .5f)
	{
		return EAffectedDirection::kRight;
	}
	else if (RightDot < -.5f)
	{
		return EAffectedDirection::kLeft;
	}

	return EAffectedDirection::kForward;
}

void UCharacterAbilitySystemComponent::UpdateMap(
	const FGameplayTag& Tag,
	float Value,
	int32 MinValue,
	int32 MaxValue,
	const FGameplayEffectSpec& Spec,
	const FGameplayAttributeData* GameplayAttributeDataPtr
)
{
	FGameplayTagContainer AllAssetTags;
	Spec.GetAllAssetTags(AllAssetTags);

	if (!ValueMap.Contains(GameplayAttributeDataPtr))
	{
		ValueMap.Add(
			GameplayAttributeDataPtr,
			{
				{
					UGameplayTagsLibrary::DataSource_Regular,
					FMath::Clamp(GameplayAttributeDataPtr->GetCurrentValue(), MinValue, MaxValue)
				}
			}
		);
	}

	if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyType_BaseValue_Addtive))
	{
		auto& GameplayAttributeDataMap = ValueMap[GameplayAttributeDataPtr];

		if (GameplayAttributeDataMap.Contains(UGameplayTagsLibrary::DataSource_Regular))
		{
			GameplayAttributeDataMap[UGameplayTagsLibrary::DataSource_Regular] += Value;
		}
		else
		{
			GameplayAttributeDataMap.Add(UGameplayTagsLibrary::DataSource_Regular, Value);
		}

		GameplayAttributeDataMap[UGameplayTagsLibrary::DataSource_Regular] =
			FMath::Clamp(GameplayAttributeDataMap[UGameplayTagsLibrary::DataSource_Regular], MinValue, MaxValue);
	}
	else if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyType_Immediate_Override))
	{
		auto& GameplayAttributeDataMap = ValueMap[GameplayAttributeDataPtr];

		if (GameplayAttributeDataMap.Contains(UGameplayTagsLibrary::DataSource_Regular))
		{
			GameplayAttributeDataMap[UGameplayTagsLibrary::DataSource_Regular] = Value;
		}
		else
		{
			GameplayAttributeDataMap.Add(UGameplayTagsLibrary::DataSource_Regular, Value);
		}

		GameplayAttributeDataMap[UGameplayTagsLibrary::DataSource_Regular] =
			FMath::Clamp(GameplayAttributeDataMap[UGameplayTagsLibrary::DataSource_Regular], MinValue, MaxValue);
	}
	else if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyType_Temporary))
	{
		auto& GameplayAttributeDataMap = ValueMap[GameplayAttributeDataPtr];
		GameplayAttributeDataMap.Add(Tag, Value);
	}
	else if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyType_RemoveTemporary))
	{
		auto& GameplayAttributeDataMap = ValueMap[GameplayAttributeDataPtr];
		if (GameplayAttributeDataMap.Contains(Tag))
		{
			GameplayAttributeDataMap.Remove(Tag);
		}
	}
}

float UCharacterAbilitySystemComponent::GetMapValue(
	const FGameplayEffectSpec& Spec,
	const FGameplayAttributeData* GameplayAttributeDataPtr
) const
{
	float Result = 0.f;

	if (!ValueMap.Contains(GameplayAttributeDataPtr))
	{
		return Result;
	}

	auto& GameplayAttributeDataMap = ValueMap[GameplayAttributeDataPtr];

	for (auto Iter : GameplayAttributeDataMap)
	{
		Result += Iter.Value;
	}

	return Result;
}

void UCharacterAbilitySystemComponent::OnSendEventModifyData(FGameplayAbilityTargetData_GASendEvent& OutGAEventData)
{
	for (auto Iter : SendEventModifysMap)
	{
		// Iter->Modify(OutGAEventData);
	}
}

void UCharacterAbilitySystemComponent::OnSendEventModifyData(
	UAbilitySystemComponent* AbilitySystemComponentPtr,
	FGameplayEffectSpecHandle GameplayEffectSpecHandle
)
{
	// 根据自身的效果对【输出】进行一些修正
	TArray<decltype(SendEventModifysMap)::iterator> NeedRemoveIterAry;;

	for (auto Iter = SendEventModifysMap.begin(); Iter != SendEventModifysMap.end(); Iter++)
	{
		if ((*Iter)->Modify(GameplayEffectSpecHandle.Data->SetByCallerTagMagnitudes) && (*Iter)->bIsOnceTime)
		{
			NeedRemoveIterAry.Add(Iter);
		}
	}

	for (auto Iter : NeedRemoveIterAry)
	{
		SendEventModifysMap.erase(Iter);
	}
}

void UCharacterAbilitySystemComponent::OnReceivedEventModifyData(
	const TMap<FGameplayTag, float>& CustomMagnitudes,
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput
)
{
	// 获得对应GE对象
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FGameplayEffectContextHandle Context = Spec.GetContext();

	auto TargetCharacterPtr = Cast<ACharacterBase>(ExecutionParams.GetTargetAbilitySystemComponent()->GetOwnerActor());
	
	auto Instigator = Cast<ACharacterBase>(Context.GetInstigator());
	auto EffectCauser = Cast<ACharacterBase>(Context.GetEffectCauser());

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	// 获得来源AttributeSet
	const auto SourceASCPtr = Cast<ThisClass>(
		ExecutionParams.GetSourceAbilitySystemComponent());

	// 获得来源AttributeSet
	const auto SourceSet = Cast<UAS_Character>(
		ExecutionParams.GetSourceAbilitySystemComponent()->GetAttributeSet(UAS_Character::StaticClass()));

	// 获得目标AttributeSet
	const auto TargetSet = Cast<UAS_Character>(
		ExecutionParams.GetTargetAbilitySystemComponent()->GetAttributeSet(UAS_Character::StaticClass()));

	TMap<FGameplayTag, float> SetByCallerTagMagnitudes = CustomMagnitudes;
	SetByCallerTagMagnitudes.Append(Spec.SetByCallerTagMagnitudes);

	FGameplayTagContainer AllAssetTags;
	Spec.GetAllAssetTags(AllAssetTags);

	// 如果是此类，数据在 SetByCallerTagMagnitudes
	if (
		AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyType_BaseValue_Addtive) ||
		AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyType_Immediate_Override)
	)
	{
		for (const auto& Iter : SetByCallerTagMagnitudes)
		{
			if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_HP))
			{
				UpdateMap(
					Iter.Key,
					Iter.Value,
					0.f,
					SourceSet->GetMax_HP(),
					Spec,
					UAS_Character::GetHPAttribute().GetGameplayAttributeData(TargetSet)
				);
			}
			else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_PP))
			{
				UpdateMap(
					Iter.Key,
					Iter.Value,
					0.f,
					SourceSet->GetMax_PP(),
					Spec,
					UAS_Character::GetPPAttribute().GetGameplayAttributeData(TargetSet)
				);
			}
			else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Mana))
			{
				UpdateMap(
					Iter.Key,
					Iter.Value,
					0.f,
					SourceSet->GetMax_Mana(),
					Spec,
					UAS_Character::GetManaAttribute().GetGameplayAttributeData(TargetSet)
				);
			}
			else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Damage_Base))
			{
				UpdateMap(
					Iter.Key,
					-Iter.Value,
					0.f,
					SourceSet->GetMax_HP(),
					Spec,
					UAS_Character::GetHPAttribute().GetGameplayAttributeData(TargetSet)
				);
			}
		}
	}
	// 如果是此类 SetByCallerTagMagnitudes 仅为一条，且Key为数据的组成
	else if (
		AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyType_Temporary) ||
		AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyType_RemoveTemporary)
	)
	{
		if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyItem_MoveSpeed))
		{
			for (const auto& Iter : SetByCallerTagMagnitudes)
			{
				UpdateMap(
					Iter.Key,
					Iter.Value,
					0.f,
					UGameOptions::GetInstance()->MaxMoveSpeed,
					Spec,
					UAS_Character::GetMoveSpeedAttribute().GetGameplayAttributeData(TargetSet)
				);
			}
		}
		else if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyItem_PerformSpeed))
		{
			for (const auto& Iter : SetByCallerTagMagnitudes)
			{
				UpdateMap(
					Iter.Key,
					Iter.Value,
					0.f,
					UGameOptions::GetInstance()->MaxPerformSpeed,
					Spec,
					UAS_Character::GetPerformSpeedAttribute().GetGameplayAttributeData(TargetSet)
				);
			}
		}
	}

	// 根据自身的效果对【输入】进行一些修正
	TArray<decltype(ReceivedEventModifysMap)::iterator> NeedRemoveIterAry;;

	for (auto Iter = ReceivedEventModifysMap.begin(); Iter != ReceivedEventModifysMap.end(); Iter++)
	{
		if ((*Iter)->Modify(SetByCallerTagMagnitudes) && (*Iter)->bIsOnceTime)
		{
			NeedRemoveIterAry.Add(Iter);
		}
	}

	for (auto Iter : NeedRemoveIterAry)
	{
		ReceivedEventModifysMap.erase(Iter);
	}

	// 如果是此类，数据在 SetByCallerTagMagnitudes
	if (
		AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyType_BaseValue_Addtive) ||
		AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyType_Immediate_Override)
	)
	{
		for (const auto& Iter : SetByCallerTagMagnitudes)
		{
			if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_HP))
			{
				const auto NewValue = GetMapValue(Spec, UAS_Character::GetHPAttribute().GetGameplayAttributeData(TargetSet));
				
				OutExecutionOutput.AddOutputModifier(
					FGameplayModifierEvaluatedData(
						UAS_Character::GetHPAttribute(),
						EGameplayModOp::Override,
						NewValue
					)
				);
			}
			else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_PP))
			{
				OutExecutionOutput.AddOutputModifier(
					FGameplayModifierEvaluatedData(
						UAS_Character::GetPPAttribute(),
						EGameplayModOp::Override,
						GetMapValue(Spec, UAS_Character::GetPPAttribute().GetGameplayAttributeData(TargetSet))
					)
				);
			}
			else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Mana))
			{
				OutExecutionOutput.AddOutputModifier(
					FGameplayModifierEvaluatedData(
						UAS_Character::GetManaAttribute(),
						EGameplayModOp::Override,
						GetMapValue(Spec, UAS_Character::GetManaAttribute().GetGameplayAttributeData(TargetSet))
					)
				);
			}
			else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Damage_Base))
			{
				const auto  NewValue = 
						GetMapValue(Spec, UAS_Character::GetHPAttribute().GetGameplayAttributeData(TargetSet));
				OutExecutionOutput.AddOutputModifier(
					FGameplayModifierEvaluatedData(
						UAS_Character::GetHPAttribute(),
						EGameplayModOp::Override,
						NewValue
					)
				);
				
				if (SourceASCPtr)
				{
					SourceASCPtr->MakedDamageDelegate(GetOwner<ACharacterBase>(), SetByCallerTagMagnitudes);
				}

				// 回执
				if (NewValue <= 0.f)
				{
					FGameplayEffectSpecHandle SpecHandle =
						MakeOutgoingSpec(UAssetRefMap::GetInstance()->DamageCallbackClass, 1, MakeEffectContext());
					SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_Damage_Callback);

					SpecHandle.Data.Get()->SetSetByCallerMagnitude(UGameplayTagsLibrary::GEData_Damage_Callback_IsDeath,
																   1);

					TArray<TWeakObjectPtr<AActor> >Ary;
					Ary.Add(Instigator);
					FGameplayAbilityTargetDataHandle TargetData;

					auto GameplayAbilityTargetData_ActorArrayPtr = new FGameplayAbilityTargetData_ActorArray;
					GameplayAbilityTargetData_ActorArrayPtr->SetActors(Ary);

					TargetData.Add(GameplayAbilityTargetData_ActorArrayPtr);
					const auto GEHandleAry = ApplyGameplayEffectSpecToTarget(
					*SpecHandle.Data,
					Instigator->GetAbilitySystemComponent()
					);
				}
			}
		}
	}
	// 如果是此类 SetByCallerTagMagnitudes 仅为一条，且Key为数据的组成
	else if (
		AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyType_Temporary) ||
		AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyType_RemoveTemporary)
	)
	{
		if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyItem_MoveSpeed))
		{
			OutExecutionOutput.AddOutputModifier(
				FGameplayModifierEvaluatedData(
					UAS_Character::GetMoveSpeedAttribute(),
					EGameplayModOp::Override,
					GetMapValue(Spec, UAS_Character::GetMoveSpeedAttribute().GetGameplayAttributeData(TargetSet))
				)
			);
		}
		else if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyItem_PerformSpeed))
		{
			OutExecutionOutput.AddOutputModifier(
				FGameplayModifierEvaluatedData(
					UAS_Character::GetPerformSpeedAttribute(),
					EGameplayModOp::Override,
					GetMapValue(Spec, UAS_Character::GetPerformSpeedAttribute().GetGameplayAttributeData(TargetSet))
				)
			);
		}
	}
}

TMap<ECharacterPropertyType, FBaseProperty> GetAllData()
{
	TMap<ECharacterPropertyType, FBaseProperty> Result;

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

	Result.Add(ECharacterPropertyType::Shield, 0);

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

	for (int32 Index = 0; Index < static_cast<int32>(ECharacterPropertyType::kMax); Index++)
	{
		Result.Add(static_cast<ECharacterPropertyType>(Index), 0);
	}

	return Result;
}

void UCharacterAbilitySystemComponent::OnGEAppliedDelegateToTarget(
	UAbilitySystemComponent* AbilitySystemComponentPtr,
	const FGameplayEffectSpec& GameplayEffectSpec,
	FActiveGameplayEffectHandle InActiveGameplayEffectHandle
)
{
	auto ActiveGameplayEffectPtr = AbilitySystemComponentPtr->GetActiveGameplayEffect(InActiveGameplayEffectHandle);

	FGameplayTagContainer OutContainer;
	GameplayEffectSpec.GetAllAssetTags(OutContainer);

	if (OutContainer.HasTag(UGameplayTagsLibrary::GEData_Damage))
	{
		
	}
}

void UCharacterAbilitySystemComponent::OnActiveGEAddedDelegateToSelf(
	UAbilitySystemComponent* AbilitySystemComponentPtr,
	const FGameplayEffectSpec& GameplayEffectSpec,
	FActiveGameplayEffectHandle InActiveGameplayEffectHandle
)
{
	auto ActiveGameplayEffectPtr = AbilitySystemComponentPtr->GetActiveGameplayEffect(InActiveGameplayEffectHandle);

	FGameplayTagContainer OutContainer;
	GameplayEffectSpec.GetAllAssetTags(OutContainer);

	if (OutContainer.HasTag(UGameplayTagsLibrary::GEData_Damage))
	{
	}
	else if (OutContainer.HasTag(UGameplayTagsLibrary::GEData_Damage_Callback))
	{
		const auto Value = GameplayEffectSpec.GetSetByCallerMagnitude(UGameplayTagsLibrary::GEData_Damage_Callback_IsDeath);
		if (Value)
		{
			
		}
		else
		{
			
		}
	}
}
