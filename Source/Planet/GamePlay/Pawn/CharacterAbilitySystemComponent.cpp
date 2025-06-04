#include "CharacterAbilitySystemComponent.h"

#include <queue>
#include <map>

#include "GameplayAbilitySpec.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTask_Tornado.h"
#include "AssetRefMap.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayEffectExecutionCalculation.h"
#include "AttributeSet.h"


#include "CharacterBase.h"
#include "CharacterAttributesComponent.h"
#include "GenerateTypes.h"
#include "AS_Character.h"
#include "BasicFutures_Affected.h"
#include "Skill_Element_Metal.h"
#include "GameplayTagsLibrary.h"
#include "BasicFutures_Dash.h"
#include "BasicFutures_HasBeenFlyAway.h"
#include "BasicFutures_HasbeenSuppress.h"
#include "BasicFutures_HasbeenTornodo.h"
#include "BasicFutures_HasbeenTraction.h"
#include "BasicFutures_MoveToAttaclArea.h"
#include "EventSubjectComponent.h"
#include "Tornado.h"
#include "OnEffectedTawrgetCallback.h"
#include "PlanetWeapon_Base.h"
#include "Weapon_Base.h"
#include "Kismet/KismetMathLibrary.h"

UCharacterAbilitySystemComponent::UCharacterAbilitySystemComponent(
	const FObjectInitializer& ObjectInitializer
	):
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
		                                                   this,
		                                                   &ThisClass::OnGEAppliedDelegateToTarget
		                                                  );
		OnGameplayEffectAppliedDelegateToSelf.AddUObject(
		                                                 this,
		                                                 &ThisClass::OnActiveGEAddedDelegateToSelf
		                                                );


		AddOutputModify(MakeShared<IOutputData_ProbabilityConfirmation_ModifyInterface>(100));

		AddInputModify(MakeShared<IInputData_ProbabilityConfirmation_ModifyInterface>(100));
		AddInputModify(MakeShared<IInputData_BasicData_ModifyInterface>(101));
		AddInputModify(MakeShared<IInputData_Shield_ModifyInterface>(102));
	}
#endif
}

bool UCharacterAbilitySystemComponent::IsCantBeDamage() const
{
	return IsInDeath() || IsUnSelected();
}

bool UCharacterAbilitySystemComponent::IsInDeath() const
{
	auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
	return OnwerActorPtr->GetCharacterAbilitySystemComponent()->HasMatchingGameplayTag(
		 UGameplayTagsLibrary::State_Dying
		);
}

bool UCharacterAbilitySystemComponent::IsUnSelected() const
{
	auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
	return OnwerActorPtr->GetCharacterAbilitySystemComponent()->HasMatchingGameplayTag(
		 UGameplayTagsLibrary::State_Buff_CantBeSlected
		);
}

bool UCharacterAbilitySystemComponent::IsRunning() const
{
	auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
	return OnwerActorPtr->GetCharacterAbilitySystemComponent()->K2_HasMatchingGameplayTag(
		 UGameplayTagsLibrary::BaseFeature_Run
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

		FGameplayAbilitySpec AbilitySpec(
		                                 Skill_Element_GoldClass
		                                );
		GiveAbilityAndActivateOnce(AbilitySpec);
#pragma region 结算效果修正

#pragma endregion 结算效果修正
	}
#endif
}

void UCharacterAbilitySystemComponent::SwitchWalkState_Implementation(
	bool bIsRun
	)
{
	if (bIsRun)
	{
		auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
		if (OnwerActorPtr)
		{
			if (OnwerActorPtr->GetCharacterAbilitySystemComponent()->K2_HasMatchingGameplayTag(
				 UGameplayTagsLibrary::State_Running
				))
			{
				return;
			}

			OnwerActorPtr->GetCharacterAbilitySystemComponent()->TryActivateAbilitiesByTag(
				 FGameplayTagContainer{UGameplayTagsLibrary::BaseFeature_Run}
				);
		}
	}
	else
	{
		auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
		if (OnwerActorPtr)
		{
			if (OnwerActorPtr->GetCharacterAbilitySystemComponent()->K2_HasMatchingGameplayTag(
				 UGameplayTagsLibrary::State_Running
				))
			{
				FGameplayTagContainer GameplayTagContainer{UGameplayTagsLibrary::BaseFeature_Run};
				OnwerActorPtr->GetCharacterAbilitySystemComponent()->CancelAbilities(&GameplayTagContainer);
			}
		}
	}
}

void UCharacterAbilitySystemComponent::Dash_Implementation(
	EDashDirection DashDirection
	)
{
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

		FGameplayEventData Payload;
		auto GameplayAbilityTargetData_DashPtr = new FGameplayAbilityTargetData_Dash;
		GameplayAbilityTargetData_DashPtr->DashDirection = DashDirection;

		Payload.TargetData.Add(GameplayAbilityTargetData_DashPtr);

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		                                                         OnwerActorPtr,
		                                                         UGameplayTagsLibrary::BaseFeature_Dash,
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
			 FGameplayTagContainer{UGameplayTagsLibrary::BaseFeature_Jump}
			);
	}
}

void UCharacterAbilitySystemComponent::HasBeenFlayAway_Implementation(
	int32 Height
	)
{
	auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
	if (OnwerActorPtr)
	{
		FGameplayEventData Payload;
		auto GameplayAbilityTargetData_DashPtr = new FGameplayAbilityTargetData_ActiveParam_HasBeenFlyAway;
		GameplayAbilityTargetData_DashPtr->Height = Height;

		Payload.TargetData.Add(GameplayAbilityTargetData_DashPtr);

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		                                                         OnwerActorPtr,
		                                                         UGameplayTagsLibrary::BaseFeature_HasBeenFlyAway,
		                                                         Payload
		                                                        );
	}
}

void UCharacterAbilitySystemComponent::SwitchWeapon_Implementation()
{
	auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
	if (OnwerActorPtr)
	{
		FGameplayEventData Payload;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		                                                         OnwerActorPtr,
		                                                         UGameplayTagsLibrary::BaseFeature_SwitchWeapon,
		                                                         Payload
		                                                        );
	}
}

void UCharacterAbilitySystemComponent::HasbeenTornodo(
	const TWeakObjectPtr<ATornado>& TornadoPtr
	)
{
	auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
	if (OnwerActorPtr)
	{
		FGameplayEventData Payload;
		auto GameplayAbilityTargetData_DashPtr = new FGameplayAbilityTargetData_ActiveParam_HasbeenTornodo;
		GameplayAbilityTargetData_DashPtr->TornadoPtr = TornadoPtr;

		Payload.TargetData.Add(GameplayAbilityTargetData_DashPtr);

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		                                                         OnwerActorPtr,
		                                                         UGameplayTagsLibrary::BaseFeature_HasbeenTornodo,
		                                                         Payload
		                                                        );
	}
}

void UCharacterAbilitySystemComponent::HasbeenTraction(
	const TWeakObjectPtr<ATractionPoint>& TractionPointPtr
	)
{
	auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
	if (OnwerActorPtr)
	{
		FGameplayEventData Payload;
		auto GameplayAbilityTargetData_DashPtr = new FGameplayAbilityTargetData_ActiveParam_HasbeenTraction;
		GameplayAbilityTargetData_DashPtr->TractionPoint = TractionPointPtr;

		Payload.TargetData.Add(GameplayAbilityTargetData_DashPtr);

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		                                                         OnwerActorPtr,
		                                                         UGameplayTagsLibrary::BaseFeature_HasbeenTraction,
		                                                         Payload
		                                                        );
	}
}

void UCharacterAbilitySystemComponent::HasbeenSuppress(
	const TWeakObjectPtr<ACharacterBase>& InstigatorPtr,
	const TSoftObjectPtr<UAnimMontage>& Montage,
	float Duration
	)
{
	auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
	if (OnwerActorPtr)
	{
		FGameplayEventData Payload;
		auto GameplayAbilityTargetData_DashPtr = new FGameplayAbilityTargetData_ActiveParam_HasbeenSuppress;
		GameplayAbilityTargetData_DashPtr->InstigatorPtr = InstigatorPtr;
		GameplayAbilityTargetData_DashPtr->MontageRef = Montage;

		Payload.TargetData.Add(GameplayAbilityTargetData_DashPtr);

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		                                                         OnwerActorPtr,
		                                                         UGameplayTagsLibrary::BaseFeature_HasbeenSuppress,
		                                                         Payload
		                                                        );
	}
}

void UCharacterAbilitySystemComponent::HasbeenAttacked(
	const TWeakObjectPtr<ACharacterBase>& InstigatorPtr,
	FVector RepelDirection
	)
{
	auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
	if (OnwerActorPtr)
	{
		FGameplayEventData Payload;
		auto GameplayAbilityTargetData = new FGameplayAbilityTargetData_Affected;
		GameplayAbilityTargetData->TriggerCharacterPtr = InstigatorPtr;
		GameplayAbilityTargetData->RepelDirection = RepelDirection;

		Payload.TargetData.Add(GameplayAbilityTargetData);

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		                                                         OnwerActorPtr,
		                                                         UGameplayTagsLibrary::BaseFeature_HasBeenAffected,
		                                                         Payload
		                                                        );
	}
}

void UCharacterAbilitySystemComponent::HasBeenRepel(
	const TWeakObjectPtr<ACharacterBase>& InstigatorPtr,
	FVector RepelDirection,
	int32 RepelDistance
	)
{
	auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
	if (OnwerActorPtr)
	{
		FGameplayEventData Payload;
		auto GameplayAbilityTargetData = new FGameplayAbilityTargetData_Affected;
		GameplayAbilityTargetData->TriggerCharacterPtr = InstigatorPtr;
		GameplayAbilityTargetData->RepelDirection = RepelDirection;

		Payload.TargetData.Add(GameplayAbilityTargetData);

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		                                                         OnwerActorPtr,
		                                                         UGameplayTagsLibrary::BaseFeature_HasBeenAffected,
		                                                         Payload
		                                                        );
	}
}

void UCharacterAbilitySystemComponent::SwitchCantBeSelect(
	bool bIsCanBeSelect
	)
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

void UCharacterAbilitySystemComponent::SwitchInvisible(
	bool bIsInvisible
	)
{
	auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
	if (OnwerActorPtr)
	{
		TArray<AActor*> OutActors;
		OnwerActorPtr->GetAttachedActors(OutActors);

		for (auto ActorIter : OutActors)
		{
			auto WeaponPtr = Cast<APlanetWeapon_Base>(ActorIter);
			if (WeaponPtr)
			{
				WeaponPtr->SetActorHiddenInGame(bIsInvisible);
			}
		}

		OnwerActorPtr->GetCopyPoseMesh()->SetHiddenInGame(bIsInvisible);
	}
}

void UCharacterAbilitySystemComponent::Respawn_Implementation()
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

		FGameplayEventData Payload;
		Payload.TargetData.Add(MoveToAttaclAreaPtr);

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		                                                         OnwerActorPtr,
		                                                         UGameplayTagsLibrary::BaseFeature_MoveToLocation,
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

		FGameplayTagContainer GameplayTagContainer{UGameplayTagsLibrary::BaseFeature_MoveToLocation};
		GASPtr->CancelAbilities(&GameplayTagContainer);
	}
}

void UCharacterAbilitySystemComponent::OnGroupManaggerReady(
	AGroupManagger* NewGroupSharedInfoPtr
	)
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

EAffectedDirection UCharacterAbilitySystemComponent::GetAffectedDirection(
	ACharacterBase* TargetCharacterPtr,
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

void UCharacterAbilitySystemComponent::UpdateMapBaseValue(
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
		FDataComposition DataComposition;

		DataComposition.DataMap = {
			{
				UGameplayTagsLibrary::DataSource_Character,
				FMath::Clamp(GameplayAttributeDataPtr->GetCurrentValue(), MinValue, MaxValue)
			}
		};

		ValueMap.Add(
		             GameplayAttributeDataPtr,
		             DataComposition
		            );
	}

	if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyType_BaseValue_Addtive))
	{
		auto& GameplayAttributeDataMap = ValueMap[GameplayAttributeDataPtr];

		if (GameplayAttributeDataMap.DataMap.Contains(UGameplayTagsLibrary::DataSource_Character))
		{
			GameplayAttributeDataMap.DataMap[UGameplayTagsLibrary::DataSource_Character] += Value;
		}
		else
		{
			GameplayAttributeDataMap.DataMap.Add(UGameplayTagsLibrary::DataSource_Character, Value);
		}

		GameplayAttributeDataMap.DataMap[UGameplayTagsLibrary::DataSource_Character] =
			FMath::Clamp(
			             GameplayAttributeDataMap.DataMap[UGameplayTagsLibrary::DataSource_Character],
			             MinValue,
			             MaxValue
			            );
	}
	else if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyType_BaseValue_Override))
	{
		auto& GameplayAttributeDataMap = ValueMap[GameplayAttributeDataPtr];

		if (GameplayAttributeDataMap.DataMap.Contains(UGameplayTagsLibrary::DataSource_Character))
		{
			GameplayAttributeDataMap.DataMap[UGameplayTagsLibrary::DataSource_Character] = Value;
		}
		else
		{
			GameplayAttributeDataMap.DataMap.Add(UGameplayTagsLibrary::DataSource_Character, Value);
		}

		GameplayAttributeDataMap.DataMap[UGameplayTagsLibrary::DataSource_Character] =
			FMath::Clamp(
			             GameplayAttributeDataMap.DataMap[UGameplayTagsLibrary::DataSource_Character],
			             MinValue,
			             MaxValue
			            );
	}
}

void UCharacterAbilitySystemComponent::UpdateMapTemporary(
	const FGameplayTag& Tag,
	float Value,
	const FGameplayEffectSpec& Spec,
	const FGameplayAttributeData* GameplayAttributeDataPtr
	)
{
	FGameplayTagContainer AllAssetTags;
	Spec.GetAllAssetTags(AllAssetTags);

	if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyType_Temporary_Data))
	{
		if (ValueMap.Contains(GameplayAttributeDataPtr))
		{
			auto& GameplayAttributeDataMap = ValueMap[GameplayAttributeDataPtr];
			GameplayAttributeDataMap.DataMap.Add(Tag, Value);
		}
		else
		{
			FDataComposition DataComposition;

			DataComposition.DataMap = {
				{
					Tag, Value
				}
			};

			ValueMap.Add(
						 GameplayAttributeDataPtr,
						 DataComposition
						);
		}

		return;
	}

	if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyType_RemoveTemporary_Data))
	{
		if (ValueMap.Contains(GameplayAttributeDataPtr))
		{
			auto& GameplayAttributeDataMap = ValueMap[GameplayAttributeDataPtr];
			if (GameplayAttributeDataMap.DataMap.Contains(Tag))
			{
				GameplayAttributeDataMap.DataMap.Remove(Tag);
			}
		}

		return;
	}

	if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyType_Temporary_Percent))
	{
		if (ValueMap.Contains(GameplayAttributeDataPtr))
		{
			auto& GameplayAttributeDataMap = ValueMap[GameplayAttributeDataPtr];
			GameplayAttributeDataMap.MagnitudeMap.Add(Tag, Value);
		}
		else
		{
			FDataComposition DataComposition;

			DataComposition.MagnitudeMap = {
				{
					Tag, Value
				}
			};

			ValueMap.Add(
						 GameplayAttributeDataPtr,
						 DataComposition
						);
		}

		return;
	}

	if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyType_RemoveTemporary_Percent))
	{
		if (ValueMap.Contains(GameplayAttributeDataPtr))
		{
			auto& GameplayAttributeDataMap = ValueMap[GameplayAttributeDataPtr];
			if (GameplayAttributeDataMap.MagnitudeMap.Contains(Tag))
			{
				GameplayAttributeDataMap.MagnitudeMap.Remove(Tag);
			}
		}

		return;
	}
}

void UCharacterAbilitySystemComponent::UpdateMapTemporary(
	const FGameplayTag& ModifyTypeTag,
	float Value,
	const FGameplayAttributeData* GameplayAttributeDataPtr
	)
{
	if (ValueMap.Contains(GameplayAttributeDataPtr))
	{
		if (ModifyTypeTag.MatchesTag(UGameplayTagsLibrary::GEData_ModifyType_BaseValue_Addtive))
		{
			auto& GameplayAttributeDataMap = ValueMap[GameplayAttributeDataPtr];
			for (auto& Iter : GameplayAttributeDataMap.DataMap)
			{
				if (Value > Iter.Value)
				{
					Value -= Iter.Value;
					if (Value <= 0)
					{
						break;
					}
				}
				else
				{
					Iter.Value = Iter.Value
					             + Value;
					break;
				}
			}
		}
		else if (ModifyTypeTag.MatchesTag(UGameplayTagsLibrary::GEData_ModifyType_BaseValue_Override))
		{
			auto& GameplayAttributeDataMap = ValueMap[GameplayAttributeDataPtr];
			GameplayAttributeDataMap.Empty();
		}
	}

	UpdateValueMap();
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
		const auto Value = GameplayEffectSpec.GetSetByCallerMagnitude(
		                                                              UGameplayTagsLibrary::GEData_Damage_Callback_IsDeath
		                                                             );
		if (Value)
		{
		}
		else
		{
		}
	}
}

void UCharacterAbilitySystemComponent::ApplyInputData(
	const FGameplayTagContainer& AllAssetTags,
	TSet<FGameplayTag>& NeedModifySet,
	const TMap<FGameplayTag, float>& CustomMagnitudes,
	const TSet<EAdditionalModify>& AdditionalModifyAry,
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput
	)
{
	auto TargetCharacterPtr = Cast<ACharacterBase>(ExecutionParams.GetTargetAbilitySystemComponent()->GetOwnerActor());

	// 获得对应GE对象
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FGameplayEffectContextHandle Context = Spec.GetContext();

	auto Instigator = Cast<ACharacterBase>(Context.GetInstigator());

	// 获得目标AttributeSet
	const auto TargetSet = Cast<UAS_Character>(
	                                           ExecutionParams.GetTargetAbilitySystemComponent()->GetAttributeSet(
		                                            UAS_Character::StaticClass()
		                                           )
	                                          );

	// 回执
	FOnEffectedTawrgetCallback ReceivedEventModifyDataCallback;

	ReceivedEventModifyDataCallback.InstigatorCharacterPtr = Instigator;
	ReceivedEventModifyDataCallback.TargetCharacterPtr = TargetCharacterPtr;
	ReceivedEventModifyDataCallback.AllAssetTags = AllAssetTags;
	ReceivedEventModifyDataCallback.SetByCallerTagMagnitudes = CustomMagnitudes;

	for (const auto Iter : AdditionalModifyAry)
	{
		switch (Iter)
		{
		case EAdditionalModify::kIsCritical:
			{
				ReceivedEventModifyDataCallback.bIsCritical = true;
			}
			break;
		case EAdditionalModify::kIsEvade:
			{
				ReceivedEventModifyDataCallback.bIsEvade = true;
			}
			break;
		}
	}

#pragma region 确认最终要应用的值
	// 如果是此类，数据在 SetByCallerTagMagnitudes
	if (
		AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyType_BaseValue_Addtive) ||
		AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyType_BaseValue_Override)
	)
	{
		auto Lambda = [&ReceivedEventModifyDataCallback, &NeedModifySet, this, TargetSet, &Spec](
			const TPair<FGameplayTag, float>& Iter,
			const float MaxValue,
			const FGameplayAttribute& Attribute
			)
		{
			NeedModifySet.Add(Iter.Key);
			UpdateMapBaseValue(
			                   UGameplayTagsLibrary::DataSource_Character,
			                   Iter.Value,
			                   0.f,
			                   MaxValue,
			                   Spec,
			                   Attribute.GetGameplayAttributeData(TargetSet)
			                  );

			ReceivedEventModifyDataCallback.TherapeuticalDose = Iter.Value;
		};
		for (const auto& Iter : CustomMagnitudes)
		{
			if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_HP))
			{
				Lambda(Iter, TargetSet->GetMax_HP(), UAS_Character::GetHPAttribute());
			}
			else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Stamina))
			{
				Lambda(Iter, TargetSet->GetMax_Stamina(), UAS_Character::GetStaminaAttribute());
			}
			else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Mana))
			{
				Lambda(Iter, TargetSet->GetMax_Mana(), UAS_Character::GetManaAttribute());
			}

			else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_MaxHP))
			{
				Lambda(Iter, TargetSet->Max_Max_HP, UAS_Character::GetMax_HPAttribute());
			}
			else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_MaxStamina))
			{
				Lambda(Iter, TargetSet->Max_Max_Stamina, UAS_Character::GetMax_StaminaAttribute());
			}
			else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_MaxMana))
			{
				Lambda(Iter, TargetSet->Max_Max_Mana, UAS_Character::GetMax_ManaAttribute());
			}

			else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Metal_Value))
			{
				Lambda(Iter, TargetSet->MaxElementalValue, UAS_Character::GetMetalValueAttribute());
			}
			else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Metal_Level))
			{
				Lambda(Iter, TargetSet->MaxElementalLevel, UAS_Character::GetMetalLevelAttribute());
			}
			else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Metal_Penetration))
			{
				Lambda(Iter, TargetSet->MaxElementalPenetration, UAS_Character::GetMetalPenetrationAttribute());
			}
			else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Metal_PercentPenetration))
			{
				Lambda(
				       Iter,
				       TargetSet->MaxElementalPercentPenetration,
				       UAS_Character::GetMetalPercentPenetrationAttribute()
				      );
			}
			else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Metal_Resistance))
			{
				Lambda(Iter, TargetSet->MaxElementalResistance, UAS_Character::GetMetalResistanceAttribute());
			}

			else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Wood_Value))
			{
				Lambda(Iter, TargetSet->MaxElementalValue, UAS_Character::GetWoodValueAttribute());
			}
			else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Wood_Level))
			{
				Lambda(Iter, TargetSet->MaxElementalLevel, UAS_Character::GetWoodLevelAttribute());
			}
			else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Wood_Penetration))
			{
				Lambda(Iter, TargetSet->MaxElementalPenetration, UAS_Character::GetWoodPenetrationAttribute());
			}
			else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Wood_PercentPenetration))
			{
				Lambda(
				       Iter,
				       TargetSet->MaxElementalPercentPenetration,
				       UAS_Character::GetWoodPercentPenetrationAttribute()
				      );
			}
			else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Wood_Resistance))
			{
				Lambda(Iter, TargetSet->MaxElementalResistance, UAS_Character::GetWoodResistanceAttribute());
			}

			else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Water_Value))
			{
				Lambda(Iter, TargetSet->MaxElementalValue, UAS_Character::GetWaterValueAttribute());
			}
			else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Water_Level))
			{
				Lambda(Iter, TargetSet->MaxElementalLevel, UAS_Character::GetWaterLevelAttribute());
			}
			else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Water_Penetration))
			{
				Lambda(Iter, TargetSet->MaxElementalPenetration, UAS_Character::GetWaterPenetrationAttribute());
			}
			else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Water_PercentPenetration))
			{
				Lambda(
				       Iter,
				       TargetSet->MaxElementalPercentPenetration,
				       UAS_Character::GetWaterPercentPenetrationAttribute()
				      );
			}
			else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Water_Resistance))
			{
				Lambda(Iter, TargetSet->MaxElementalResistance, UAS_Character::GetWaterResistanceAttribute());
			}

			else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Fire_Value))
			{
				Lambda(Iter, TargetSet->MaxElementalValue, UAS_Character::GetFireValueAttribute());
			}
			else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Fire_Level))
			{
				Lambda(Iter, TargetSet->MaxElementalLevel, UAS_Character::GetFireLevelAttribute());
			}
			else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Fire_Penetration))
			{
				Lambda(Iter, TargetSet->MaxElementalPenetration, UAS_Character::GetFirePenetrationAttribute());
			}
			else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Fire_PercentPenetration))
			{
				Lambda(
				       Iter,
				       TargetSet->MaxElementalPercentPenetration,
				       UAS_Character::GetFirePercentPenetrationAttribute()
				      );
			}
			else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Fire_Resistance))
			{
				Lambda(Iter, TargetSet->MaxElementalResistance, UAS_Character::GetFireResistanceAttribute());
			}

			else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Earth_Value))
			{
				Lambda(Iter, TargetSet->MaxElementalValue, UAS_Character::GetEarthValueAttribute());
			}
			else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Earth_Level))
			{
				Lambda(Iter, TargetSet->MaxElementalLevel, UAS_Character::GetEarthLevelAttribute());
			}
			else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Earth_Penetration))
			{
				Lambda(Iter, TargetSet->MaxElementalPenetration, UAS_Character::GetEarthPenetrationAttribute());
			}
			else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Earth_PercentPenetration))
			{
				Lambda(
				       Iter,
				       TargetSet->MaxElementalPercentPenetration,
				       UAS_Character::GetEarthPercentPenetrationAttribute()
				      );
			}
			else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Earth_Resistance))
			{
				Lambda(Iter, TargetSet->MaxElementalResistance, UAS_Character::GetEarthResistanceAttribute());
			}

			else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Damage_Metal))
			{
				auto Temp = Iter;
				Temp.Value = 0 - Iter.Value;
				Lambda(Temp, TargetSet->GetMax_HP(), UAS_Character::GetHPAttribute());
				ReceivedEventModifyDataCallback.Damage = Iter.Value;
			}
			else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Damage_Wood))
			{
				auto Temp = Iter;
				Temp.Value = 0 - Iter.Value;
				Lambda(Temp, TargetSet->GetMax_HP(), UAS_Character::GetHPAttribute());
				ReceivedEventModifyDataCallback.Damage = Iter.Value;
			}
			else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Damage_Water))
			{
				auto Temp = Iter;
				Temp.Value = 0 - Iter.Value;
				Lambda(Temp, TargetSet->GetMax_HP(), UAS_Character::GetHPAttribute());
				ReceivedEventModifyDataCallback.Damage = Iter.Value;
			}
			else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Damage_Fire))
			{
				auto Temp = Iter;
				Temp.Value = 0 - Iter.Value;
				Lambda(Temp, TargetSet->GetMax_HP(), UAS_Character::GetHPAttribute());
				ReceivedEventModifyDataCallback.Damage = Iter.Value;
			}
			else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Damage_Earth))
			{
				auto Temp = Iter;
				Temp.Value = 0 - Iter.Value;
				Lambda(Temp, TargetSet->GetMax_HP(), UAS_Character::GetHPAttribute());
				ReceivedEventModifyDataCallback.Damage = Iter.Value;
			}
		}
	}
	// 如果是此类 SetByCallerTagMagnitudes 仅为一条，且Key为数据的组成
	else if (
		AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyType_Temporary_Data) ||
		AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyType_Temporary_Percent) ||
		AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyType_RemoveTemporary_Data) ||
		AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyType_RemoveTemporary_Percent)
	)
	{
		auto Lambda = [&NeedModifySet, this, TargetSet, &Spec, &CustomMagnitudes](
			const FGameplayTag& ModifyItemTag,
			const FGameplayAttribute& Attribute
			)
		{
			NeedModifySet.Add(ModifyItemTag);
			for (const auto& Iter : CustomMagnitudes)
			{
				UpdateMapTemporary(
				                   Iter.Key,
				                   Iter.Value,
				                   Spec,
				                   Attribute.GetGameplayAttributeData(TargetSet)
				                  );
			}
		};
		if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyItem_MoveSpeed))
		{
			Lambda(UGameplayTagsLibrary::GEData_ModifyItem_MoveSpeed, UAS_Character::GetMoveSpeedAttribute());
		}
		else if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyItem_PerformSpeed))
		{
			Lambda(UGameplayTagsLibrary::GEData_ModifyItem_PerformSpeed, UAS_Character::GetPerformSpeedAttribute());
		}
		else if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyItem_Shield))
		{
			Lambda(UGameplayTagsLibrary::GEData_ModifyItem_Shield, UAS_Character::GetShieldAttribute());
		}

		else if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyItem_CriticalDamage))
		{
			Lambda(UGameplayTagsLibrary::GEData_ModifyItem_CriticalDamage, UAS_Character::GetCriticalDamageAttribute());
		}
		else if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyItem_CriticalHitRate))
		{
			Lambda(
			       UGameplayTagsLibrary::GEData_ModifyItem_CriticalHitRate,
			       UAS_Character::GetCriticalHitRateAttribute()
			      );
		}
		else if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyItem_HitRate))
		{
			Lambda(UGameplayTagsLibrary::GEData_ModifyItem_HitRate, UAS_Character::GetHitRateAttribute());
		}
		else if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyItem_EvadeRate))
		{
			Lambda(UGameplayTagsLibrary::GEData_ModifyItem_EvadeRate, UAS_Character::GetEvadeRateAttribute());
		}

		else if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyItem_Metal_Value))
		{
			Lambda(UGameplayTagsLibrary::GEData_ModifyItem_Metal_Value, UAS_Character::GetMetalValueAttribute());
		}
		else if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyItem_Metal_Level))
		{
			Lambda(UGameplayTagsLibrary::GEData_ModifyItem_Metal_Level, UAS_Character::GetMetalLevelAttribute());
		}
		else if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyItem_Metal_Penetration))
		{
			Lambda(
			       UGameplayTagsLibrary::GEData_ModifyItem_Metal_Penetration,
			       UAS_Character::GetMetalPenetrationAttribute()
			      );
		}
		else if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyItem_Metal_PercentPenetration))
		{
			Lambda(
			       UGameplayTagsLibrary::GEData_ModifyItem_Metal_PercentPenetration,
			       UAS_Character::GetMetalPercentPenetrationAttribute()
			      );
		}
		else if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyItem_Metal_Resistance))
		{
			Lambda(
			       UGameplayTagsLibrary::GEData_ModifyItem_Metal_Resistance,
			       UAS_Character::GetMetalResistanceAttribute()
			      );
		}

		else if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyItem_Wood_Value))
		{
			Lambda(UGameplayTagsLibrary::GEData_ModifyItem_Wood_Value, UAS_Character::GetWoodValueAttribute());
		}
		else if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyItem_Wood_Level))
		{
			Lambda(UGameplayTagsLibrary::GEData_ModifyItem_Wood_Level, UAS_Character::GetWoodLevelAttribute());
		}
		else if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyItem_Wood_Penetration))
		{
			Lambda(
			       UGameplayTagsLibrary::GEData_ModifyItem_Wood_Penetration,
			       UAS_Character::GetWoodPenetrationAttribute()
			      );
		}
		else if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyItem_Wood_PercentPenetration))
		{
			Lambda(
			       UGameplayTagsLibrary::GEData_ModifyItem_Wood_PercentPenetration,
			       UAS_Character::GetWoodPercentPenetrationAttribute()
			      );
		}
		else if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyItem_Wood_Resistance))
		{
			Lambda(
			       UGameplayTagsLibrary::GEData_ModifyItem_Wood_Resistance,
			       UAS_Character::GetWoodResistanceAttribute()
			      );
		}

		else if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyItem_Water_Value))
		{
			Lambda(UGameplayTagsLibrary::GEData_ModifyItem_Water_Value, UAS_Character::GetWaterValueAttribute());
		}
		else if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyItem_Water_Level))
		{
			Lambda(UGameplayTagsLibrary::GEData_ModifyItem_Water_Level, UAS_Character::GetWaterLevelAttribute());
		}
		else if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyItem_Water_Penetration))
		{
			Lambda(
			       UGameplayTagsLibrary::GEData_ModifyItem_Water_Penetration,
			       UAS_Character::GetWaterPenetrationAttribute()
			      );
		}
		else if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyItem_Water_PercentPenetration))
		{
			Lambda(
			       UGameplayTagsLibrary::GEData_ModifyItem_Water_PercentPenetration,
			       UAS_Character::GetWaterPercentPenetrationAttribute()
			      );
		}
		else if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyItem_Water_Resistance))
		{
			Lambda(
			       UGameplayTagsLibrary::GEData_ModifyItem_Water_Resistance,
			       UAS_Character::GetWaterResistanceAttribute()
			      );
		}

		else if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyItem_Fire_Value))
		{
			Lambda(UGameplayTagsLibrary::GEData_ModifyItem_Fire_Value, UAS_Character::GetFireValueAttribute());
		}
		else if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyItem_Fire_Level))
		{
			Lambda(UGameplayTagsLibrary::GEData_ModifyItem_Fire_Level, UAS_Character::GetFireLevelAttribute());
		}
		else if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyItem_Fire_Penetration))
		{
			Lambda(
			       UGameplayTagsLibrary::GEData_ModifyItem_Fire_Penetration,
			       UAS_Character::GetFirePenetrationAttribute()
			      );
		}
		else if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyItem_Fire_PercentPenetration))
		{
			Lambda(
			       UGameplayTagsLibrary::GEData_ModifyItem_Fire_PercentPenetration,
			       UAS_Character::GetFirePercentPenetrationAttribute()
			      );
		}
		else if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyItem_Fire_Resistance))
		{
			Lambda(
			       UGameplayTagsLibrary::GEData_ModifyItem_Fire_Resistance,
			       UAS_Character::GetFireResistanceAttribute()
			      );
		}

		else if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyItem_Earth_Value))
		{
			Lambda(UGameplayTagsLibrary::GEData_ModifyItem_Earth_Value, UAS_Character::GetEarthValueAttribute());
		}
		else if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyItem_Earth_Level))
		{
			Lambda(UGameplayTagsLibrary::GEData_ModifyItem_Earth_Level, UAS_Character::GetEarthLevelAttribute());
		}
		else if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyItem_Earth_Penetration))
		{
			Lambda(
			       UGameplayTagsLibrary::GEData_ModifyItem_Earth_Penetration,
			       UAS_Character::GetEarthPenetrationAttribute()
			      );
		}
		else if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyItem_Earth_PercentPenetration))
		{
			Lambda(
			       UGameplayTagsLibrary::GEData_ModifyItem_Earth_PercentPenetration,
			       UAS_Character::GetEarthPercentPenetrationAttribute()
			      );
		}
		else if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyItem_Earth_Resistance))
		{
			Lambda(
			       UGameplayTagsLibrary::GEData_ModifyItem_Earth_Resistance,
			       UAS_Character::GetEarthResistanceAttribute()
			      );
		}
	}
#pragma endregion

#pragma region 把值写进GE里面
	for (const auto& Iter : NeedModifySet)
	{
		auto Lambda = [this, TargetSet, &Spec, &CustomMagnitudes, &OutExecutionOutput](
			const FGameplayAttribute& Attribute
			)
		{
			const auto NewValue = GetBaseValueMaps(
			                                       Spec,
			                                       Attribute.GetGameplayAttributeData(
				                                        TargetSet
				                                       )
			                                      );

			OutExecutionOutput.AddOutputModifier(
			                                     FGameplayModifierEvaluatedData(
			                                                                    Attribute,
			                                                                    EGameplayModOp::Override,
			                                                                    NewValue
			                                                                   )
			                                    );
			return NewValue;
		};
		if (Iter.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_HP))
		{
			const auto NewValue = Lambda(UAS_Character::GetHPAttribute());
			ReceivedEventModifyDataCallback.bIsDeath = NewValue <= 0.f;
		}
		else if (Iter.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Stamina))
		{
			const auto NewValue = Lambda(UAS_Character::GetStaminaAttribute());
		}
		else if (Iter.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Mana))
		{
			const auto NewValue = Lambda(UAS_Character::GetManaAttribute());
		}

		else if (Iter.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_MaxHP))
		{
			const auto NewValue = Lambda(UAS_Character::GetMax_HPAttribute());
		}
		else if (Iter.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_MaxStamina))
		{
			const auto NewValue = Lambda(UAS_Character::GetMax_StaminaAttribute());
		}
		else if (Iter.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_MaxMana))
		{
			const auto NewValue = Lambda(UAS_Character::GetMax_ManaAttribute());
		}

		else if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyItem_CriticalDamage))
		{
			const auto NewValue = Lambda(UAS_Character::GetCriticalDamageAttribute());
		}
		else if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyItem_CriticalHitRate))
		{
			const auto NewValue = Lambda(UAS_Character::GetCriticalHitRateAttribute());
		}
		else if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyItem_HitRate))
		{
			const auto NewValue = Lambda(UAS_Character::GetHitRateAttribute());
		}
		else if (AllAssetTags.HasTag(UGameplayTagsLibrary::GEData_ModifyItem_EvadeRate))
		{
			const auto NewValue = Lambda(UAS_Character::GetEvadeRateAttribute());
		}

		else if (Iter.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Shield))
		{
			const auto NewValue = Lambda(UAS_Character::GetShieldAttribute());
			ReceivedEventModifyDataCallback.bIsDeath = NewValue <= 0.f;
		}
		else if (Iter.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_MoveSpeed))
		{
			const auto NewValue = Lambda(UAS_Character::GetMoveSpeedAttribute());
		}
		else if (Iter.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_PerformSpeed))
		{
			const auto NewValue = Lambda(UAS_Character::GetPerformSpeedAttribute());
		}

		else if (Iter.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Metal_Value))
		{
			const auto NewValue = Lambda(UAS_Character::GetMetalValueAttribute());
		}
		else if (Iter.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Metal_Level))
		{
			const auto NewValue = Lambda(UAS_Character::GetMetalLevelAttribute());
		}
		else if (Iter.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Metal_Penetration))
		{
			const auto NewValue = Lambda(UAS_Character::GetMetalPenetrationAttribute());
		}
		else if (Iter.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Metal_PercentPenetration))
		{
			const auto NewValue = Lambda(UAS_Character::GetMetalPercentPenetrationAttribute());
		}
		else if (Iter.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Metal_Resistance))
		{
			const auto NewValue = Lambda(UAS_Character::GetMetalResistanceAttribute());
		}

		else if (Iter.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Wood_Value))
		{
			const auto NewValue = Lambda(UAS_Character::GetWoodValueAttribute());
		}
		else if (Iter.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Wood_Level))
		{
			const auto NewValue = Lambda(UAS_Character::GetWoodLevelAttribute());
		}
		else if (Iter.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Wood_Penetration))
		{
			const auto NewValue = Lambda(UAS_Character::GetWoodPenetrationAttribute());
		}
		else if (Iter.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Wood_PercentPenetration))
		{
			const auto NewValue = Lambda(UAS_Character::GetWoodPercentPenetrationAttribute());
		}
		else if (Iter.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Wood_Resistance))
		{
			const auto NewValue = Lambda(UAS_Character::GetWoodResistanceAttribute());
		}

		else if (Iter.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Water_Value))
		{
			const auto NewValue = Lambda(UAS_Character::GetWaterValueAttribute());
		}
		else if (Iter.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Water_Level))
		{
			const auto NewValue = Lambda(UAS_Character::GetWaterLevelAttribute());
		}
		else if (Iter.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Water_Penetration))
		{
			const auto NewValue = Lambda(UAS_Character::GetWaterPenetrationAttribute());
		}
		else if (Iter.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Water_PercentPenetration))
		{
			const auto NewValue = Lambda(UAS_Character::GetWaterPercentPenetrationAttribute());
		}
		else if (Iter.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Water_Resistance))
		{
			const auto NewValue = Lambda(UAS_Character::GetWaterResistanceAttribute());
		}

		else if (Iter.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Fire_Value))
		{
			const auto NewValue = Lambda(UAS_Character::GetFireValueAttribute());
		}
		else if (Iter.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Fire_Level))
		{
			const auto NewValue = Lambda(UAS_Character::GetFireLevelAttribute());
		}
		else if (Iter.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Fire_Penetration))
		{
			const auto NewValue = Lambda(UAS_Character::GetFirePenetrationAttribute());
		}
		else if (Iter.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Fire_PercentPenetration))
		{
			const auto NewValue = Lambda(UAS_Character::GetFirePercentPenetrationAttribute());
		}
		else if (Iter.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Fire_Resistance))
		{
			const auto NewValue = Lambda(UAS_Character::GetFireResistanceAttribute());
		}

		else if (Iter.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Earth_Value))
		{
			const auto NewValue = Lambda(UAS_Character::GetEarthValueAttribute());
		}
		else if (Iter.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Earth_Level))
		{
			const auto NewValue = Lambda(UAS_Character::GetEarthLevelAttribute());
		}
		else if (Iter.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Earth_Penetration))
		{
			const auto NewValue = Lambda(UAS_Character::GetEarthPenetrationAttribute());
		}
		else if (Iter.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Earth_PercentPenetration))
		{
			const auto NewValue = Lambda(UAS_Character::GetEarthPercentPenetrationAttribute());
		}
		else if (Iter.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Earth_Resistance))
		{
			const auto NewValue = Lambda(UAS_Character::GetEarthResistanceAttribute());
		}

		else if (Iter.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Damage_Metal))
		{
			const auto NewValue = Lambda(UAS_Character::GetHPAttribute());
			ReceivedEventModifyDataCallback.bIsDeath = NewValue <= 0.f;
			ReceivedEventModifyDataCallback.ElementalType = EElementalType::kMetal;
		}
		else if (Iter.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Damage_Wood))
		{
			const auto NewValue = Lambda(UAS_Character::GetHPAttribute());
			ReceivedEventModifyDataCallback.bIsDeath = NewValue <= 0.f;
			ReceivedEventModifyDataCallback.ElementalType = EElementalType::kWood;
		}
		else if (Iter.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Damage_Water))
		{
			const auto NewValue = Lambda(UAS_Character::GetHPAttribute());
			ReceivedEventModifyDataCallback.bIsDeath = NewValue <= 0.f;
			ReceivedEventModifyDataCallback.ElementalType = EElementalType::kWater;
		}
		else if (Iter.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Damage_Fire))
		{
			const auto NewValue = Lambda(UAS_Character::GetHPAttribute());
			ReceivedEventModifyDataCallback.bIsDeath = NewValue <= 0.f;
			ReceivedEventModifyDataCallback.ElementalType = EElementalType::kFire;
		}
		else if (Iter.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Damage_Earth))
		{
			const auto NewValue = Lambda(UAS_Character::GetHPAttribute());
			ReceivedEventModifyDataCallback.bIsDeath = NewValue <= 0.f;
			ReceivedEventModifyDataCallback.ElementalType = EElementalType::kEarth;
		}
	}
#pragma endregion

	Instigator->GetCharacterAbilitySystemComponent()->OnEffectOhterCharacter(
	                                                                         ReceivedEventModifyDataCallback
	                                                                        );

	if (Instigator != TargetCharacterPtr)
	{
		TargetCharacterPtr->GetCharacterAbilitySystemComponent()->OnEffectOhterCharacter(
			 ReceivedEventModifyDataCallback
			);

		if (ReceivedEventModifyDataCallback.bIsDeath)
		{
		}
		else if (ReceivedEventModifyDataCallback.bIsEvade)
		{
		}
		else
		{
			if (ReceivedEventModifyDataCallback.Damage > 0)
			{
				const auto HitResultPtr = Context.GetHitResult();
				if (HitResultPtr)
				{
					TargetCharacterPtr->GetCharacterAbilitySystemComponent()->HasbeenAttacked(
						 Instigator,
						 HitResultPtr->ImpactNormal
						);
				}
			}
		}
	}
}

void UCharacterAbilitySystemComponent::OnEffectOhterCharacter_Implementation(
	const FOnEffectedTawrgetCallback& ReceivedEventModifyDataCallback
	)
{
	MakedDamageDelegate(ReceivedEventModifyDataCallback);
}
