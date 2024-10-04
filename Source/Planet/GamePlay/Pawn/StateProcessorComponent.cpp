
#include "StateProcessorComponent.h"

#include <queue>
#include <map>

#include "GameplayAbilitySpec.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Net/UnrealNetwork.h"

#include "GravityMovementComponent.h"
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
#include "CS_PeriodicStateModify_Ice.h"
#include "CS_PeriodicPropertyTag.h"
#include "CS_PeriodicStateModify_Slow.h"
#include "CS_PeriodicStateModify_Fear.h"
#include "CS_PeriodicStateModify_SuperArmor.h"
#include "HumanAnimInstance.h"

FName UStateProcessorComponent::ComponentName = TEXT("StateProcessorComponent");

UStateProcessorComponent::UStateProcessorComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UStateProcessorComponent::BeginPlay()
{
	Super::BeginPlay();

	auto CharacterPtr = GetOwner<FOwnerPawnType>();
	if (CharacterPtr)
	{
		auto GASCompPtr = CharacterPtr->GetAbilitySystemComponent();
		OnGameplayEffectTagCountChangedHandle = GASCompPtr->RegisterGenericGameplayTagEvent().AddUObject(
			this, &ThisClass::OnGameplayEffectTagCountChanged
		);
	}
}

void UStateProcessorComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;

	Params.Condition = COND_OwnerOnly;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, CharacterStateInfo_FASI_Container, Params);
}

TSharedPtr<FCharacterStateInfo>  UStateProcessorComponent::GetCharacterState(const FGameplayTag& CSTag) const
{
	if (StateDisplayMap.Contains(CSTag))
	{
		return StateDisplayMap[CSTag];
	}

	return nullptr;
}

void UStateProcessorComponent::AddStateDisplay(const TSharedPtr<FCharacterStateInfo>& StateDisplayInfo)
{
#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		CharacterStateInfo_FASI_Container.AddItem(StateDisplayInfo);
	}
#endif

	StateDisplayMap.Add(StateDisplayInfo->Tag, StateDisplayInfo);
	CharacterStateMapChanged(StateDisplayInfo, true);

#if UE_EDITOR || UE_CLIENT
	if (GetNetMode() == NM_Client)
	{
	}
#endif
}

void UStateProcessorComponent::ChangeStateDisplay(const TSharedPtr<FCharacterStateInfo>& StateDisplayInfo)
{
#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		CharacterStateInfo_FASI_Container.UpdateItem(StateDisplayInfo);
	}
#endif

	if (StateDisplayMap.Contains(StateDisplayInfo->Tag))
	{
		StateDisplayMap[StateDisplayInfo->Tag]->DataChanged();
	}

#if UE_EDITOR || UE_CLIENT
	if (GetNetMode() == NM_Client)
	{
	}
#endif
}

void UStateProcessorComponent::RemoveStateDisplay(const TSharedPtr<FCharacterStateInfo>& StateDisplayInfo)
{
#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		CharacterStateInfo_FASI_Container.RemoveItem(StateDisplayInfo);
	}
#endif

	if (StateDisplayMap.Contains(StateDisplayInfo->Tag))
	{
		StateDisplayMap.Remove(StateDisplayInfo->Tag);
		CharacterStateMapChanged(StateDisplayInfo, false);
	}

#if UE_EDITOR || UE_CLIENT
	if (GetNetMode() == NM_Client)
	{
	}
#endif
}

void UStateProcessorComponent::OnGameplayEffectTagCountChanged(const FGameplayTag Tag, int32 Count)
{
	auto Lambda = [&]
		{
			const auto Value = Count > 0;
			return Value;
		};

	if (Tag.MatchesTag(UGameplayTagsSubSystem::GetInstance()->RootMotion))
	{

	}
	else if (Tag.MatchesTag(UGameplayTagsSubSystem::GetInstance()->MovementStateAble))
	{
		if (Tag.MatchesTagExact(UGameplayTagsSubSystem::GetInstance()->MovementStateAble_CantJump))
		{
			auto CharacterPtr = GetOwner<FOwnerPawnType>();
			if (CharacterPtr)
			{
				auto CharacterMovementPtr = CharacterPtr->GetGravityMovementComponent();
				CharacterMovementPtr->MovementState.bCanJump = !Lambda();
			}
		}
		else if (Tag.MatchesTagExact(UGameplayTagsSubSystem::GetInstance()->MovementStateAble_CantPathFollowMove))
		{
			auto CharacterPtr = GetOwner<FOwnerPawnType>();
			if (CharacterPtr)
			{
				auto CharacterMovementPtr = CharacterPtr->GetGravityMovementComponent();
				CharacterMovementPtr->bSkip_PathFollow = Lambda();
			}
		}
		else if (Tag.MatchesTagExact(UGameplayTagsSubSystem::GetInstance()->MovementStateAble_CantPlayerInputMove))
		{
			auto CharacterPtr = GetOwner<FOwnerPawnType>();
			if (CharacterPtr)
			{
				auto CharacterMovementPtr = CharacterPtr->GetGravityMovementComponent();
				CharacterMovementPtr->bSkip_PlayerInput = Lambda();
			}
		}
		else if (Tag.MatchesTagExact(UGameplayTagsSubSystem::GetInstance()->MovementStateAble_CantRootMotion))
		{
			auto CharacterPtr = GetOwner<FOwnerPawnType>();
			if (CharacterPtr)
			{
				auto CharacterMovementPtr = CharacterPtr->GetGravityMovementComponent();
				CharacterMovementPtr->bSkip_RootMotion = Lambda();
			}
		}
		if (Tag.MatchesTagExact(UGameplayTagsSubSystem::GetInstance()->MovementStateAble_CantRotation))
		{
			auto CharacterPtr = GetOwner<FOwnerPawnType>();
			if (CharacterPtr)
			{
				auto CharacterMovementPtr = CharacterPtr->GetGravityMovementComponent();
				CharacterMovementPtr->bSkip_Rotation = Lambda();
			}
		}
		else if (Tag.MatchesTagExact(UGameplayTagsSubSystem::GetInstance()->MovementStateAble_IntoFly))
		{
			auto CharacterPtr = GetOwner<FOwnerPawnType>();
			if (CharacterPtr)
			{
				auto CharacterMovementPtr = CharacterPtr->GetGravityMovementComponent();
				CharacterMovementPtr->SetMovementMode(Lambda() ? EMovementMode::MOVE_Flying : EMovementMode::MOVE_Falling);
				if (Lambda())
				{
					CharacterMovementPtr->MaxFlySpeed = CharacterMovementPtr->MaxWalkSpeed;
					CharacterMovementPtr->BrakingDecelerationFlying = 2048;
				}
			}
		}
		else if (Tag.MatchesTagExact(UGameplayTagsSubSystem::GetInstance()->MovementStateAble_Orient2Acce))
		{
			auto CharacterPtr = GetOwner<FOwnerPawnType>();
			if (CharacterPtr)
			{
				auto CharacterMovementPtr = CharacterPtr->GetGravityMovementComponent();
				CharacterMovementPtr->SetIsOrientRotationToMovement_RPC(Lambda());
			}
		}
	}
	else if (Tag.MatchesTagExact(UGameplayTagsSubSystem::GetInstance()->State_ReleasingSkill))
	{
		auto CharacterPtr = GetOwner<FOwnerPawnType>();
		if (CharacterPtr)
		{
			auto AnimInsPtr = CharacterPtr->GetAnimationIns<UHumanAnimInstance>();
			AnimInsPtr->SetIsMelee(Lambda());
		}
	}
}

void UStateProcessorComponent::OnCharacterStateChanged(ECharacterStateType CharacterStateType, UCS_Base* CharacterStatePtr)
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

void UStateProcessorComponent::ExcuteEffects(
	const TSharedPtr<FGameplayAbilityTargetData_PropertyModify>& GameplayAbilityTargetDataSPtr
)
{
	auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
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
			MakeTargetData(GameplayAbilityTargetDataSPtr)
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

void UStateProcessorComponent::ExcuteEffects(
	const TSharedPtr<FGameplayAbilityTargetData_StateModify>& GameplayAbilityTargetDataSPtr
)
{
	auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
	if (OnwerActorPtr)
	{
		auto ASCPtr = OnwerActorPtr->GetAbilitySystemComponent();

		if (CharacterStateMap.Contains(GameplayAbilityTargetDataSPtr->Tag))
		{
			auto GAPtr = Cast<UCS_PeriodicStateModify>(CharacterStateMap[GameplayAbilityTargetDataSPtr->Tag]);
			if (GAPtr && GAPtr->CanActivateAbility(GAPtr->GetCurrentAbilitySpecHandle(), GAPtr->GetCurrentActorInfo()))
			{
				GAPtr->SetCache(GameplayAbilityTargetDataSPtr);
				GAPtr->UpdateDuration();
				return;
			}
		}

		if (GameplayAbilityTargetDataSPtr->Tag.MatchesTagExact(UGameplayTagsSubSystem::GetInstance()->State_Debuff_Stun))
		{
			auto  Spec = MakeSpec(GameplayAbilityTargetDataSPtr, UCS_PeriodicStateModify_Stun::StaticClass());
			ASCPtr->GiveAbilityAndActivateOnce(
				Spec,
				MakeTargetData(GameplayAbilityTargetDataSPtr)
			);
		}
		else if (GameplayAbilityTargetDataSPtr->Tag.MatchesTagExact(UGameplayTagsSubSystem::GetInstance()->State_Debuff_Charm))
		{
			FGameplayAbilitySpec Spec(UCS_PeriodicStateModify_Charm::StaticClass(), 1);

			ASCPtr->GiveAbilityAndActivateOnce(
				Spec,
				MakeTargetData(GameplayAbilityTargetDataSPtr)
			);
		}
		else if (GameplayAbilityTargetDataSPtr->Tag.MatchesTagExact(UGameplayTagsSubSystem::GetInstance()->State_Debuff_Fear))
		{
			FGameplayAbilitySpec Spec(UCS_PeriodicStateModify_Fear::StaticClass(), 1);

			ASCPtr->GiveAbilityAndActivateOnce(
				Spec,
				MakeTargetData(GameplayAbilityTargetDataSPtr)
			);
		}
		else if (GameplayAbilityTargetDataSPtr->Tag.MatchesTagExact(UGameplayTagsSubSystem::GetInstance()->State_Debuff_Ice))
		{
			FGameplayAbilitySpec Spec(UCS_PeriodicStateModify_Ice::StaticClass(), 1);

			ASCPtr->GiveAbilityAndActivateOnce(
				Spec,
				MakeTargetData(GameplayAbilityTargetDataSPtr)
			);
		}
		else if (GameplayAbilityTargetDataSPtr->Tag.MatchesTagExact(UGameplayTagsSubSystem::GetInstance()->State_Debuff_Slow))
		{
			FGameplayAbilitySpec Spec(UCS_PeriodicStateModify_Slow::StaticClass(), 1);

			ASCPtr->GiveAbilityAndActivateOnce(
				Spec,
				MakeTargetData(GameplayAbilityTargetDataSPtr)
			);
		}
		else if (GameplayAbilityTargetDataSPtr->Tag.MatchesTagExact(UGameplayTagsSubSystem::GetInstance()->State_Buff_SuperArmor))
		{
			FGameplayAbilitySpec Spec(UCS_PeriodicStateModify_SuperArmor::StaticClass(), 1);

			ASCPtr->GiveAbilityAndActivateOnce(
				Spec,
				MakeTargetData(GameplayAbilityTargetDataSPtr)
			);
		}
	}
}

void UStateProcessorComponent::ExcuteEffects(
	const TSharedPtr<FGameplayAbilityTargetData_RootMotion>& GameplayAbilityTargetDataSPtr
)
{
	auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
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
						MakeTargetData(GameplayAbilityTargetDataSPtr)
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
						MakeTargetData(GameplayAbilityTargetDataSPtr)
					);
				}
				else if (GameplayAbilityTargetDataSPtr->Tag.MatchesTagExact(UGameplayTagsSubSystem::GetInstance()->TornadoTraction))
				{
					FGameplayAbilitySpec Spec(UCS_RootMotion_TornadoTraction::StaticClass(), 1);

					ASCPtr->GiveAbilityAndActivateOnce(
						Spec,
						MakeTargetData(GameplayAbilityTargetDataSPtr)
					);
				}
				else if (GameplayAbilityTargetDataSPtr->Tag.MatchesTagExact(UGameplayTagsSubSystem::GetInstance()->MoveAlongSpline))
				{
					FGameplayAbilitySpec Spec(UCS_RootMotion_MoveAlongSpline::StaticClass(), 1);

					ASCPtr->GiveAbilityAndActivateOnce(
						Spec,
						MakeTargetData(GameplayAbilityTargetDataSPtr)
					);
				}
				else if (GameplayAbilityTargetDataSPtr->Tag.MatchesTagExact(UGameplayTagsSubSystem::GetInstance()->Traction))
				{
					FGameplayAbilitySpec Spec(UCS_RootMotion_MoveAlongSpline::StaticClass(), 1);

					ASCPtr->GiveAbilityAndActivateOnce(
						Spec,
						MakeTargetData(GameplayAbilityTargetDataSPtr)
					);
				}
			}
		}
	}
}

FGameplayEventData* UStateProcessorComponent::MakeTargetData(
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

FGameplayAbilitySpec UStateProcessorComponent::MakeSpec(
	const TSharedPtr<FGameplayAbilityTargetData_CS_Base>& GameplayAbilityTargetDataSPtr,
	TSubclassOf<UGameplayAbility> InAbilityClass
)
{
	auto GACDOPtr = InAbilityClass.GetDefaultObject();

	GACDOPtr->AbilityTags.AddTag(GameplayAbilityTargetDataSPtr->Tag);

	FGameplayAbilitySpec Spec(GACDOPtr, 1);

	return Spec;
}

void UStateProcessorComponent::BreakOhterState(
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

		const auto TempCharacterStateMap = OnwerActorPtr->GetStateProcessorComponent()->CharacterStateMap;
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
