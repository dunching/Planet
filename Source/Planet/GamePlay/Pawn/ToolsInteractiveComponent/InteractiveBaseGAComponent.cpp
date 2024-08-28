
#include "InteractiveBaseGAComponent.h"

#include <queue>
#include <map>

#include "GameplayAbilitySpec.h"
#include "AbilitySystemBlueprintLibrary.h"

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
#include "CS_PeriodicStateModify.h"
#include "CS_RootMotion_FlyAway.h"
#include "CS_RootMotion_TornadoTraction.h"
#include "CS_RootMotion_MoveAlongSpline.h"
#include "CS_RootMotion_KnockDown.h"
#include "CS_PeriodicStateModify_Stun.h"
#include "CS_PeriodicStateModify_Charm.h"

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

FGameplayAbilitySpecHandle UInteractiveBaseGAComponent::AddTemporaryTag(
	ACharacterBase* TargetCharacterPtr,
	FGameplayAbilityTargetData_AddTemporaryTag* GameplayAbilityTargetDataPtr
)
{
	FGameplayAbilitySpecHandle GAToolPeriodicHandle;

	return GAToolPeriodicHandle;
}

void UInteractiveBaseGAComponent::ClearData2Other(
	const TMap<ACharacterBase*, TMap<ECharacterPropertyType, FBaseProperty>>& ModifyPropertyMap, const FGameplayTag& DataSource
)
{
	auto OnwerActorPtr = GetOwner<ACharacterBase>();
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

void UInteractiveBaseGAComponent::ClearData2Self(
	const TMap<ECharacterPropertyType, FBaseProperty>& InModifyPropertyMap, const FGameplayTag& DataSource
)
{
	auto OnwerActorPtr = GetOwner<ACharacterBase>();
	if (!OnwerActorPtr)
	{
		return;
	}

	TPair<ACharacterBase*, TMap<ECharacterPropertyType, FBaseProperty>>ModifyPropertyMap{ OnwerActorPtr, InModifyPropertyMap };

	ClearData2Other({ ModifyPropertyMap }, DataSource);
}

void UInteractiveBaseGAComponent::ExcuteEffects(
	TSharedPtr<FGameplayAbilityTargetData_PropertyModify> GameplayAbilityTargetDataSPtr
)
{
	auto OnwerActorPtr = GetOwner<ACharacterBase>();
	if (OnwerActorPtr)
	{
		auto ASCPtr = OnwerActorPtr->GetAbilitySystemComponent();
		if (CharacterStateMap.Contains(GameplayAbilityTargetDataSPtr->Tag))
		{
			auto GAPtr = Cast<UCS_PeriodicPropertyModify>(CharacterStateMap[GameplayAbilityTargetDataSPtr->Tag]);
			if (GAPtr)
			{
				GAPtr->SetCache(GameplayAbilityTargetDataSPtr);
				GAPtr->UpdateDuration();
				return;
			}
		}

		FGameplayAbilitySpec Spec(UCS_PeriodicPropertyModify::StaticClass(), 1);

		auto GAHandle = ASCPtr->GiveAbilityAndActivateOnce(
			Spec,
			MkeSpec(GameplayAbilityTargetDataSPtr)
		);

		// 为什么不在此处直接获取 GetPrimaryInstance？因为在GA过程中调用时会pending，导致返回为nullptr
// 		auto GameplayAbilitySpecPtr = ASCPtr->FindAbilitySpecFromHandle(GAHandle);
// 		if (GameplayAbilitySpecPtr)
// 		{
// 			ResultPtr = Cast<UCS_PeriodicPropertyModify>(GameplayAbilitySpecPtr->GetPrimaryInstance());
// 			if (ResultPtr)
// 			{
// 				PeriodicPropertyModifyMap.Add(GameplayAbilityTargetDataSPtr->Tag, ResultPtr);
// 			}
// 		}
	}
}

void UInteractiveBaseGAComponent::ExcuteEffects(
	TSharedPtr<FGameplayAbilityTargetData_StateModify> GameplayAbilityTargetDataSPtr
)
{
	auto OnwerActorPtr = GameplayAbilityTargetDataSPtr->TargetCharacterPtr.Get();
	if (OnwerActorPtr)
	{
		auto ASCPtr = OnwerActorPtr->GetAbilitySystemComponent();

		if (PeriodicStateTagModifyMap.Contains(GameplayAbilityTargetDataSPtr->Tag))
		{
			auto GAPtr = Cast<UCS_PeriodicStateModify>(CharacterStateMap[GameplayAbilityTargetDataSPtr->Tag]);
			if (GAPtr)
			{
				GAPtr->SetCache(GameplayAbilityTargetDataSPtr);
				GAPtr->UpdateDuration();
			}
		}

		if (GameplayAbilityTargetDataSPtr->Tag.MatchesTagExact(UGameplayTagsSubSystem::GetInstance()->State_Debuff_Stun))
		{
			FGameplayAbilitySpec Spec(UCS_PeriodicStateModify_Stun::StaticClass(), 1);

			ASCPtr->GiveAbilityAndActivateOnce(
				Spec,
				MkeSpec(GameplayAbilityTargetDataSPtr)
			);
		}
		else if (GameplayAbilityTargetDataSPtr->Tag.MatchesTagExact(UGameplayTagsSubSystem::GetInstance()->State_Debuff_Charm))
		{
			FGameplayAbilitySpec Spec(UCS_PeriodicStateModify_Charm::StaticClass(), 1);

			ASCPtr->GiveAbilityAndActivateOnce(
				Spec,
				MkeSpec(GameplayAbilityTargetDataSPtr)
			);
		}
	}
}

void UInteractiveBaseGAComponent::ExcuteEffects(
	TSharedPtr<FGameplayAbilityTargetData_RootMotion> GameplayAbilityTargetDataSPtr
)
{
	auto OnwerActorPtr = GameplayAbilityTargetDataSPtr->TargetCharacterPtr.Get();
	if (OnwerActorPtr)
	{
		auto ASCPtr = OnwerActorPtr->GetAbilitySystemComponent();

		if (GameplayAbilityTargetDataSPtr->Tag.MatchesTag(UGameplayTagsSubSystem::GetInstance()->RootMotion))
		{
			if (CharacterStateMap.Contains(GameplayAbilityTargetDataSPtr->Tag))
			{
				auto GAPtr = CharacterStateMap[GameplayAbilityTargetDataSPtr->Tag];
				if (GAPtr)
				{
					GAPtr->UpdateDuration();
				}
			}
			else
			{
				if (GameplayAbilityTargetDataSPtr->Tag.MatchesTagExact(UGameplayTagsSubSystem::GetInstance()->KnockDown))
				{
					BreakOhterState(
						GameplayAbilityTargetDataSPtr,
						UGameplayTagsSubSystem::GetInstance()->KnockDown, 
						{ 
							UGameplayTagsSubSystem::GetInstance()->FlyAway,
							UGameplayTagsSubSystem::GetInstance()->TornadoTraction,
							UGameplayTagsSubSystem::GetInstance()->MoveAlongSpline,
						}
						);

					FGameplayAbilitySpec Spec(CS_RootMotion_KnockDownClass, 1);

					ASCPtr->GiveAbilityAndActivateOnce(
						Spec,
						MkeSpec(GameplayAbilityTargetDataSPtr)
					);
				}
				else if (GameplayAbilityTargetDataSPtr->Tag.MatchesTagExact(UGameplayTagsSubSystem::GetInstance()->FlyAway))
				{
					BreakOhterState(
						GameplayAbilityTargetDataSPtr,
						UGameplayTagsSubSystem::GetInstance()->FlyAway,
						{
							UGameplayTagsSubSystem::GetInstance()->KnockDown,
							UGameplayTagsSubSystem::GetInstance()->TornadoTraction,
							UGameplayTagsSubSystem::GetInstance()->MoveAlongSpline,
						}
						);

					FGameplayAbilitySpec Spec(UCS_RootMotion_FlyAway::StaticClass(), 1);

					ASCPtr->GiveAbilityAndActivateOnce(
						Spec,
						MkeSpec(GameplayAbilityTargetDataSPtr)
					);
				}
				else if (GameplayAbilityTargetDataSPtr->Tag.MatchesTagExact(UGameplayTagsSubSystem::GetInstance()->TornadoTraction))
				{
					FGameplayAbilitySpec Spec(UCS_RootMotion_TornadoTraction::StaticClass(), 1);

					ASCPtr->GiveAbilityAndActivateOnce(
						Spec,
						MkeSpec(GameplayAbilityTargetDataSPtr)
					);
				}
				else if (GameplayAbilityTargetDataSPtr->Tag.MatchesTagExact(UGameplayTagsSubSystem::GetInstance()->MoveAlongSpline))
				{
					FGameplayAbilitySpec Spec(UCS_RootMotion_MoveAlongSpline::StaticClass(), 1);

					ASCPtr->GiveAbilityAndActivateOnce(
						Spec,
						MkeSpec(GameplayAbilityTargetDataSPtr)
					);
				}
				else if (GameplayAbilityTargetDataSPtr->Tag.MatchesTagExact(UGameplayTagsSubSystem::GetInstance()->Traction))
				{
					FGameplayAbilitySpec Spec(UCS_RootMotion_MoveAlongSpline::StaticClass(), 1);

					ASCPtr->GiveAbilityAndActivateOnce(
						Spec,
						MkeSpec(GameplayAbilityTargetDataSPtr)
					);
				}
			}
		}
	}
}

FGameplayEventData* UInteractiveBaseGAComponent::MkeSpec(
	const TSharedPtr<FGameplayAbilityTargetData_CS_Base>& GameplayAbilityTargetDataSPtr
) 
{
	auto ClonePtr = GameplayAbilityTargetDataSPtr->Clone();

	if (!ClonePtr->DefaultIcon)
	{
		ClonePtr->DefaultIcon =
			USceneUnitExtendInfoMap::GetInstance()->GetTableRowUnit_TagExtendInfo(ClonePtr->Tag)->DefaultIcon;
	}

	auto Handle = ClonePtr->CharacterStateChanged.AddCallback(
		std::bind(&ThisClass::OnCharacterStateChanged, this, std::placeholders::_1, std::placeholders::_2)
	);
	Handle->bIsAutoUnregister = false;

	FGameplayEventData* Payload = new FGameplayEventData;
	Payload->TargetData.Add(ClonePtr);

	return Payload;
}

void UInteractiveBaseGAComponent::BreakOhterState(
	const TSharedPtr<FGameplayAbilityTargetData_CS_Base>& GameplayAbilityTargetDataSPtr, 
	const FGameplayTag& ThisTag, 
	const TArray<FGameplayTag>& CancelTags
)
{
	// 会中断其他跟运动的状态
	FGameplayTagContainer GameplayTagContainer;
	for (const auto Iter : CancelTags)
	{
		GameplayTagContainer.AddTag(Iter);
	}
	if (GameplayAbilityTargetDataSPtr->Tag.MatchesAnyExact(GameplayTagContainer))
	{
		auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
		auto ASCPtr = OnwerActorPtr->GetAbilitySystemComponent();

		const auto TempCharacterStateMap = CharacterStateMap;
		for (const auto Iter : TempCharacterStateMap)
		{
			if (Iter.Key.MatchesTagExact(UGameplayTagsSubSystem::GetInstance()->FlyAway))
			{
				ASCPtr->CancelAbilityHandle(
					Iter.Value->GetCurrentAbilitySpecHandle()
				);
			}
		}
	}
}

void UInteractiveBaseGAComponent::ExcuteAttackedEffect(EAffectedDirection AffectedDirection)
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

UCS_Base* UInteractiveBaseGAComponent::GetCharacterState(const FGameplayTag& CSTag) const
{
	if (CharacterStateMap.Contains(CSTag))
	{
		return CharacterStateMap[CSTag];
	}

	return nullptr;
}

void UInteractiveBaseGAComponent::SendEventImp(
	FGameplayAbilityTargetData_GASendEvent* GAEventDataPtr
)
{
	auto OnwerActorPtr = GetOwner<ACharacterBase>();
	if (OnwerActorPtr)
	{
		FGameplayEventData Payload;

		Payload.TargetData.Add(new FGameplayAbilityTargetData_GAEventType(FGameplayAbilityTargetData_GAEventType::EEventType::kNormal));
		Payload.TargetData.Add(GAEventDataPtr);

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

void UInteractiveBaseGAComponent::SendEventImp(
	FGameplayAbilityTargetData_RootMotion* GameplayAbilityTargetDataPtr
)
{
	auto OnwerActorPtr = GetOwner<ACharacterBase>();
	if (OnwerActorPtr)
	{
		FGameplayEventData Payload;

		Payload.TargetData.Add(new FGameplayAbilityTargetData_GAEventType(FGameplayAbilityTargetData_GAEventType::EEventType::kRootMotion));
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

void UInteractiveBaseGAComponent::SendEventImp(
	FGameplayAbilityTargetData_PropertyModify* GameplayAbilityTargetDataPtr
)
{
	auto OnwerActorPtr = GetOwner<ACharacterBase>();
	if (OnwerActorPtr)
	{
		FGameplayEventData Payload;

		Payload.TargetData.Add(new FGameplayAbilityTargetData_GAEventType(FGameplayAbilityTargetData_GAEventType::EEventType::kPeriodic_PropertyModify));
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

void UInteractiveBaseGAComponent::SendEventImp(
	FGameplayAbilityTargetData_StateModify* GameplayAbilityTargetDataPtr
)
{
	auto OnwerActorPtr = GetOwner<ACharacterBase>();
	if (OnwerActorPtr)
	{
		FGameplayEventData Payload;

		Payload.TargetData.Add(new FGameplayAbilityTargetData_GAEventType(FGameplayAbilityTargetData_GAEventType::EEventType::kPeriodic_StateTagModify));
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

void UInteractiveBaseGAComponent::SendEvent2Other(
	const TMap<ACharacterBase*, TMap<ECharacterPropertyType, FBaseProperty>>& ModifyPropertyMap,
	const FGameplayTag& DataSource
)
{
	auto OnwerActorPtr = GetOwner<ACharacterBase>();
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

void UInteractiveBaseGAComponent::SendEvent2Self(
	const TMap<ECharacterPropertyType, FBaseProperty>& InModifyPropertyMap,
	const FGameplayTag& DataSource
)
{
	auto OnwerActorPtr = GetOwner<ACharacterBase>();
	if (!OnwerActorPtr)
	{
		return;
	}

	TPair<ACharacterBase*, TMap<ECharacterPropertyType, FBaseProperty>>ModifyPropertyMap{ OnwerActorPtr, InModifyPropertyMap };

	SendEvent2Other({ ModifyPropertyMap }, DataSource);
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

bool UInteractiveBaseGAComponent::SwitchWalkState(bool bIsRun)
{
	if (bIsRun)
	{
		auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
		if (OnwerActorPtr)
		{
			if (OnwerActorPtr->GetAbilitySystemComponent()->K2_HasMatchingGameplayTag(UGameplayTagsSubSystem::GetInstance()->Running))
			{
				return true;
			}

			return OnwerActorPtr->GetAbilitySystemComponent()->TryActivateAbilitiesByTag(
				FGameplayTagContainer{ UGameplayTagsSubSystem::GetInstance()->Running }
			);
		}
	}
	else
	{
		auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
		if (OnwerActorPtr)
		{
			if (OnwerActorPtr->GetAbilitySystemComponent()->K2_HasMatchingGameplayTag(UGameplayTagsSubSystem::GetInstance()->Running))
			{
				FGameplayTagContainer GameplayTagContainer{ UGameplayTagsSubSystem::GetInstance()->Running };
				OnwerActorPtr->GetAbilitySystemComponent()->CancelAbilities(&GameplayTagContainer);
			}

			return !OnwerActorPtr->GetAbilitySystemComponent()->K2_HasMatchingGameplayTag(UGameplayTagsSubSystem::GetInstance()->Running);
		}
	}

	return false;
}

bool UInteractiveBaseGAComponent::Dash(EDashDirection DashDirection)
{
	FGameplayEventData Payload;
	auto GameplayAbilityTargetData_DashPtr = new FGameplayAbilityTargetData_Dash;
	GameplayAbilityTargetData_DashPtr->DashDirection = DashDirection;

	Payload.TargetData.Add(GameplayAbilityTargetData_DashPtr);

	auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
	if (OnwerActorPtr)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OnwerActorPtr, UGameplayTagsSubSystem::GetInstance()->Dash, Payload);
	}

	return true;
}

void UInteractiveBaseGAComponent::MoveToAttackDistance(
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

void UInteractiveBaseGAComponent::BreakMoveToAttackDistance()
{
	auto OnwerActorPtr = GetOwner<ACharacterBase>();
	if (OnwerActorPtr)
	{
		auto GASPtr = OnwerActorPtr->GetAbilitySystemComponent();

		FGameplayTagContainer GameplayTagContainer{ UGameplayTagsSubSystem::GetInstance()->State_MoveToAttaclArea };
		GASPtr->CancelAbilities(&GameplayTagContainer);
	}
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
					auto CharacterAttributesSPtr =
						GameplayAbilityTargetData_GAEvent.TriggerCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();

					std::map<int32, EWuXingType, std::greater<int>> ElementMap;
					ElementMap.emplace(CharacterAttributesSPtr->Element.GoldElement.GetCurrentValue(), EWuXingType::kGold);
					ElementMap.emplace(CharacterAttributesSPtr->Element.WoodElement.GetCurrentValue(), EWuXingType::kWood);
					ElementMap.emplace(CharacterAttributesSPtr->Element.WaterElement.GetCurrentValue(), EWuXingType::kWater);
					ElementMap.emplace(CharacterAttributesSPtr->Element.FireElement.GetCurrentValue(), EWuXingType::kFire);
					ElementMap.emplace(CharacterAttributesSPtr->Element.SoilElement.GetCurrentValue(), EWuXingType::kSoil);

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
				auto CharacterAttributesSPtr =
					DataRef.TargetCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();

				std::map<int32, EWuXingType, std::greater<int>> ElementMap;
				ElementMap.emplace(CharacterAttributesSPtr->Element.GoldElement.GetCurrentValue(), EWuXingType::kGold);
				ElementMap.emplace(CharacterAttributesSPtr->Element.WoodElement.GetCurrentValue(), EWuXingType::kWood);
				ElementMap.emplace(CharacterAttributesSPtr->Element.WaterElement.GetCurrentValue(), EWuXingType::kWater);
				ElementMap.emplace(CharacterAttributesSPtr->Element.FireElement.GetCurrentValue(), EWuXingType::kFire);
				ElementMap.emplace(CharacterAttributesSPtr->Element.SoilElement.GetCurrentValue(), EWuXingType::kSoil);

				const auto Effective_Rate = Caculation_Effective_Rate(ElementMap.begin()->first, 0);

				DataRef.BaseDamage = DataRef.BaseDamage * Effective_Rate;
			}
			else
			{
				auto CharacterAttributesSPtr =
					GameplayAbilityTargetData_GAEvent.TriggerCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();

				for (auto& ElementIter : DataRef.ElementSet)
				{
					// ľ          ˮ  ˮ ˻𣬻 ˽𣬽  ľ
					switch (ElementIter.Get<0>())
					{
					case EWuXingType::kGold:
					{
						const auto Effective_Rate = Caculation_Effective_Rate(CharacterAttributesSPtr->Element.FireElement.GetCurrentValue(), ElementIter.Get<1>());
						ElementIter.Get<2>() = ElementIter.Get<2>() * Effective_Rate;
					}
					break;
					case EWuXingType::kWood:
					{
						const auto Effective_Rate = Caculation_Effective_Rate(CharacterAttributesSPtr->Element.GoldElement.GetCurrentValue(), ElementIter.Get<1>());
						ElementIter.Get<2>() = ElementIter.Get<2>() * Effective_Rate;
					}
					break;
					case EWuXingType::kWater:
					{
						const auto Effective_Rate = Caculation_Effective_Rate(CharacterAttributesSPtr->Element.SoilElement.GetCurrentValue(), ElementIter.Get<1>());
						ElementIter.Get<2>() = ElementIter.Get<2>() * Effective_Rate;
					}
					break;
					case EWuXingType::kFire:
					{
						const auto Effective_Rate = Caculation_Effective_Rate(CharacterAttributesSPtr->Element.WaterElement.GetCurrentValue(), ElementIter.Get<1>());
						ElementIter.Get<2>() = ElementIter.Get<2>() * Effective_Rate;
					}
					break;
					case EWuXingType::kSoil:
					{
						const auto Effective_Rate = Caculation_Effective_Rate(CharacterAttributesSPtr->Element.WoodElement.GetCurrentValue(), ElementIter.Get<1>());
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
				auto SelfCharacterAttributesSPtr =
					GameplayAbilityTargetData_GAEvent.Data.TargetCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();

				 auto TargetCharacterAttributesSPtr =
					GameplayAbilityTargetData_GAEvent.TriggerCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();

				{
					const auto Rate = (TargetCharacterAttributesSPtr->HitRate.GetCurrentValue() - TargetCharacterAttributesSPtr->Evade.GetCurrentValue()) /
						static_cast<float>(TargetCharacterAttributesSPtr->HitRate.GetMaxValue());

					GameplayAbilityTargetData_GAEvent.Data.HitRate = FMath::FRand() <= Rate ? 100 : 0;
				}
			}
		}
	};
	AddReceviedEventModify(MakeShared<GAEventModify_MultyTarget>(9998));
}

void UInteractiveBaseGAComponent::OnCharacterStateChanged(ECharacterStateType CharacterStateType, UCS_Base* CharacterStatePtr)
{
	CharacterStateChangedContainer.ExcuteCallback(CharacterStateType, CharacterStatePtr);
	switch (CharacterStateType)
	{
	case ECharacterStateType::kActive:
	{
		CharacterStateMap.Add(CharacterStatePtr->GameplayAbilityTargetDataBaseSPtr->Tag, CharacterStatePtr);
	}
	break;
	case ECharacterStateType::kEnd:
	{
		if (CharacterStateMap.Contains(CharacterStatePtr->GameplayAbilityTargetDataBaseSPtr->Tag))
		{
			CharacterStateMap.Remove(CharacterStatePtr->GameplayAbilityTargetDataBaseSPtr->Tag);
		}
	}
	break;
	default:
		break;
	}
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

	Result.Add(ECharacterPropertyType::BaseAttackPower, 0);
	Result.Add(ECharacterPropertyType::Penetration, 0);
	Result.Add(ECharacterPropertyType::PercentPenetration, 0);
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
