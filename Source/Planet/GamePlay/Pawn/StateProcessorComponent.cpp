
#include "StateProcessorComponent.h"

#include <queue>
#include <map>

#include "GameplayAbilitySpec.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Components/CapsuleComponent.h"

#include "GravityMovementComponent.h"

#include "CharacterBase.h"
#include "GenerateType.h"
#include "PlanetPlayerState.h"
#include "Weapon_Base.h"
#include "Skill_Base.h"
#include "GameplayTagsLibrary.h"
#include "HumanAnimInstance.h"
#include "CharacterAbilitySystemComponent.h"
#include "CollisionDataStruct.h"
#include "GroupManagger.h"
#include "TeamMatesHelperComponent.h"

FName UStateProcessorComponent::ComponentName = TEXT("StateProcessorComponent");

UStateProcessorComponent::UStateProcessorComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;
	
	SetIsReplicatedByDefault(true);
}

void UStateProcessorComponent::FocusTarget()
{
}

void UStateProcessorComponent::SetFocusCharactersAry(
	ACharacterBase* TargetCharacterPtr
)
{
#if UE_EDITOR || UE_CLIENT
	if (GetNetMode() == NM_Client)
	{
		SetFocusCharactersAry_Server(TargetCharacterPtr);
	}
#endif
	
	auto CharacterPtr = GetOwner<FOwnerPawnType>();
	if (CharacterPtr)
	{
		CharacterPtr->GetGroupManagger()->GetTeamMatesHelperComponent()->SetFocusCharactersAry(TargetCharacterPtr);
	}
}

void UStateProcessorComponent::ClearFocusCharactersAry()
{
#if UE_EDITOR || UE_CLIENT
	if (GetNetMode() == NM_Client)
	{
		ClearFocusCharactersAry_Server();
	}
#endif
	
	auto CharacterPtr = GetOwner<FOwnerPawnType>();
	if (CharacterPtr)
	{
		CharacterPtr->GetGroupManagger()->GetTeamMatesHelperComponent()->ClearFocusCharactersAry();
	}
}

TArray<ACharacterBase*> UStateProcessorComponent::GetFocusCharactersAry() const
{
	TArray<ACharacterBase*> Result;
	auto CharacterPtr = GetOwner<FOwnerPawnType>();
	if (CharacterPtr)
	{
		if (auto GroupManaggerPtr = CharacterPtr->GetGroupManagger())
		{
			auto ForceKnowCharater =GroupManaggerPtr->GetTeamMatesHelperComponent()->GetForceKnowCharater();
			if (ForceKnowCharater.IsValid())
			{
				Result.Add( ForceKnowCharater.Get());
			}
		}
	}
	return Result;
}

void UStateProcessorComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UStateProcessorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	auto CharacterPtr = GetOwner<FOwnerPawnType>();
	if (CharacterPtr)
	{
		auto GASCompPtr = CharacterPtr->GetCharacterAbilitySystemComponent();
		GASCompPtr->RegisterGenericGameplayTagEvent().Remove(OnGameplayEffectTagCountChangedHandle);
	}

	Super::EndPlay(EndPlayReason);
}

void UStateProcessorComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;

	Params.Condition = COND_None;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, CharacterStateInfo_FASI_Container, Params);
}

void UStateProcessorComponent::OnGroupManaggerReady(AGroupManagger* NewGroupSharedInfoPtr)
{
#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		auto CharacterPtr = GetOwner<FOwnerPawnType>();
		if (CharacterPtr)
		{
			auto GASCompPtr = CharacterPtr->GetCharacterAbilitySystemComponent();
			OnGameplayEffectTagCountChangedHandle = GASCompPtr->RegisterGenericGameplayTagEvent().AddUObject(
				this, &ThisClass::OnGameplayEffectTagCountChanged
			);
		}
	}
#endif
}

TSharedPtr<FCharacterStateInfo> UStateProcessorComponent::GetCharacterState(const FGameplayTag& CSTag) const
{
	for (const auto& Iter : StateDisplayMap)
	{
		if (Iter.Value && Iter.Value->Tag == CSTag)
		{
			return Iter.Value;
		}
	}

	return nullptr;
}

auto UStateProcessorComponent::BindCharacterStateChanged(const std::function<void(ECharacterStateType, UCS_Base*)>& Func)
->UStateProcessorComponent::FCharacterStateChanged::FCallbackHandleSPtr
{
	auto CallbackHandle = CharacterStateChangedContainer.AddCallback(Func);

	for (const auto& Iter : CharacterStateMap)
	{
		CharacterStateChangedContainer(ECharacterStateType::kActive, Iter.Value);
	}

	return CallbackHandle;
}

auto UStateProcessorComponent::BindCharacterStateMapChanged(const std::function<void(const TSharedPtr<FCharacterStateInfo>&, bool)>& Func)
-> UStateProcessorComponent::FCharacterStateMapChanged::FCallbackHandleSPtr
{
	auto CallbackHandle = CharacterStateMapChanged.AddCallback(Func);

	for (const auto& Iter : StateDisplayMap)
	{
		CharacterStateMapChanged(Iter.Value, true);
	}

	return CallbackHandle;
}

void UStateProcessorComponent::InitializeComponent()
{
	Super::InitializeComponent();

	UWorld* World = GetWorld();
	if ((World->IsGameWorld()))
	{
		CharacterStateInfo_FASI_Container.StateProcessorComponent = this;
	}
}

void UStateProcessorComponent::OnGameplayEffectTagCountChanged(const FGameplayTag Tag, int32 Count)
{
	auto Lambda = [&]
		{
			const auto Value = Count > 0;
			return Value;
		};

	if (Tag.MatchesTag(UGameplayTagsLibrary::RootMotion))
	{
		if (Tag.MatchesTagExact(UGameplayTagsLibrary::State_RootMotion_FlyAway))
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
	}
	else if (Tag.MatchesTag(UGameplayTagsLibrary::MovementStateAble))
	{
		if (Tag.MatchesTagExact(UGameplayTagsLibrary::MovementStateAble_CantJump))
		{
			auto CharacterPtr = GetOwner<FOwnerPawnType>();
			if (CharacterPtr)
			{
				auto CharacterMovementPtr = CharacterPtr->GetGravityMovementComponent();
				CharacterMovementPtr->MovementState.bCanJump = !Lambda();
			}
		}
		else if (Tag.MatchesTagExact(UGameplayTagsLibrary::MovementStateAble_CantPathFollowMove))
		{
			auto CharacterPtr = GetOwner<FOwnerPawnType>();
			if (CharacterPtr)
			{
				auto CharacterMovementPtr = CharacterPtr->GetGravityMovementComponent();
				CharacterMovementPtr->bSkip_PathFollow = Lambda();
			}
		}
		else if (Tag.MatchesTagExact(UGameplayTagsLibrary::MovementStateAble_CantPlayerInputMove))
		{
			auto CharacterPtr = GetOwner<FOwnerPawnType>();
			if (CharacterPtr)
			{
				auto CharacterMovementPtr = CharacterPtr->GetGravityMovementComponent();
				CharacterMovementPtr->bSkip_PlayerInput = Lambda();
			}
		}
		else if (Tag.MatchesTagExact(UGameplayTagsLibrary::MovementStateAble_CantRootMotion))
		{
			auto CharacterPtr = GetOwner<FOwnerPawnType>();
			if (CharacterPtr)
			{
				auto CharacterMovementPtr = CharacterPtr->GetGravityMovementComponent();
				CharacterMovementPtr->bSkip_RootMotion = Lambda();
			}
		}
		if (Tag.MatchesTagExact(UGameplayTagsLibrary::MovementStateAble_CantRotation))
		{
			auto CharacterPtr = GetOwner<FOwnerPawnType>();
			if (CharacterPtr)
			{
				auto CharacterMovementPtr = CharacterPtr->GetGravityMovementComponent();
				CharacterMovementPtr->bSkip_Rotation = Lambda();
			}
		}
		else if (Tag.MatchesTagExact(UGameplayTagsLibrary::MovementStateAble_Orient2Acce))
		{
			auto CharacterPtr = GetOwner<FOwnerPawnType>();
			if (CharacterPtr)
			{
				auto CharacterMovementPtr = CharacterPtr->GetGravityMovementComponent();
				CharacterMovementPtr->SetIsOrientRotationToMovement_RPC(Lambda());
			}
		}
		else if (Tag.MatchesTagExact(UGameplayTagsLibrary::MovementStateAble_UseCustomRotation))
		{
			auto CharacterPtr = GetOwner<FOwnerPawnType>();
			if (CharacterPtr)
			{
				CharacterPtr->bUseCustomRotation = Lambda();
			}
		}
	}
	else if (Tag.MatchesTagExact(UGameplayTagsLibrary::State_ReleasingSkill))
	{
		auto CharacterPtr = GetOwner<FOwnerPawnType>();
		if (CharacterPtr)
		{
			auto AnimInsPtr = CharacterPtr->GetAnimationIns<UHumanAnimInstance>();
			AnimInsPtr->SetIsMelee(Lambda());
		}
	}
	else if (Tag.MatchesTagExact(UGameplayTagsLibrary::State_Buff_CantBeSlected))
	{
		auto CharacterPtr = GetOwner<FOwnerPawnType>();
		if (CharacterPtr)
		{
			auto BaseFeatureComponentPtr = CharacterPtr->GetCharacterAbilitySystemComponent();
			BaseFeatureComponentPtr->SwitchCantBeSelect(Lambda());
		}
	}
	else if (Tag.MatchesTagExact(UGameplayTagsLibrary::State_NoPhy))
	{
		auto CharacterPtr = GetOwner<FOwnerPawnType>();
		if (CharacterPtr)
		{
			CharacterPtr->GetCapsuleComponent()->SetCollisionResponseToChannel(
				Pawn_Object, Lambda() ? ECollisionResponse::ECR_Overlap : ECollisionResponse::ECR_Block
			);
		}
	}
}

void UStateProcessorComponent::ClearFocusCharactersAry_Server_Implementation()
{
	ClearFocusCharactersAry();
}

void UStateProcessorComponent::SetFocusCharactersAry_Server_Implementation(
	ACharacterBase* TargetCharacterPtr
)
{
	SetFocusCharactersAry(TargetCharacterPtr);
}

void UStateProcessorComponent::OnCharacterStateChanged(ECharacterStateType CharacterStateType, UCS_Base* CharacterStatePtr)
{
}

void UStateProcessorComponent::ExcuteEffects(
	const TSharedPtr<FGameplayAbilityTargetData_StateModify>& GameplayAbilityTargetDataSPtr
)
{
}

void UStateProcessorComponent::ExcuteEffects(
	const TSharedPtr<FGameplayAbilityTargetData_RootMotion>& GameplayAbilityTargetDataSPtr
)
{
}

FGameplayEventData* UStateProcessorComponent::MakeTargetData(
	const TSharedPtr<FGameplayAbilityTargetData_CS_Base>& GameplayAbilityTargetDataSPtr
)
{
	FGameplayEventData* Payload = new FGameplayEventData;
	return Payload;
}

FGameplayAbilitySpec UStateProcessorComponent::MakeSpec(
	const TSharedPtr<FGameplayAbilityTargetData_CS_Base>& GameplayAbilityTargetDataSPtr,
	TSubclassOf<UPlanetGameplayAbility> InAbilityClass,
	int32 InputID
)
{
	// 	auto GACDOPtr = InAbilityClass.GetDefaultObject();
	// 
	// 	// 仅针对
	// 	GACDOPtr->InitalDefaultTags();

	FGameplayAbilitySpec Spec(InAbilityClass, 1, InputID);

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
}
