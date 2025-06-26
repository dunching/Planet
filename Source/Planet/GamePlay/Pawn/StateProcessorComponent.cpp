#include "StateProcessorComponent.h"

#include <queue>
#include <map>

#include "GameplayAbilitySpec.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Components/CapsuleComponent.h"

#include "GravityMovementComponent.h"

#include "CharacterBase.h"
#include "GenerateTypes.h"
#include "PlanetPlayerState.h"
#include "Weapon_Base.h"
#include "Skill_Base.h"
#include "GameplayTagsLibrary.h"
#include "HumanAnimInstance.h"
#include "CharacterAbilitySystemComponent.h"
#include "CollisionDataStruct.h"
#include "GroupManagger.h"
#include "HumanCharacter_Player.h"
#include "KismetGravityLibrary.h"
#include "TeamMatesHelperComponentBase.h"
#include "Kismet/KismetMathLibrary.h"

FName UStateProcessorComponent::ComponentName = TEXT("StateProcessorComponent");

UStateProcessorComponent::UStateProcessorComponent(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;

	SetIsReplicatedByDefault(true);
}

TArray<TWeakObjectPtr<ACharacterBase>> UStateProcessorComponent::GetTargetCharactersAry() const
{
	TArray<TWeakObjectPtr<ACharacterBase>> Result;
	return Result;
}

TArray<TPair<FRotator, bool>> UStateProcessorComponent::GetOrient() const
{
	TArray<TPair<FRotator, bool>> Result;

	for (const auto& Iter : GetOrientFuncMap)
	{
		if (Iter.Value)
		{
			FRotator Rot;
			bool bIsImmediatelyRot = false;
			if (Iter.Value(Rot, bIsImmediatelyRot))
			{
				Result.Add({Rot, bIsImmediatelyRot});
				break;
			}
		}
	}

	return Result;
}

void UStateProcessorComponent::AddGetOrientFunc(
	int32 Prority,
	const FGetOrientFunc& Func
	)
{
	GetOrientFuncMap.Add(Prority, Func);
}

void UStateProcessorComponent::BeginPlay()
{
	Super::BeginPlay();

	GetOrientFuncMap.Add(
	                     GetOrientPrority,
	                     std::bind(&ThisClass::GetOrientDefautl, this, std::placeholders::_1, std::placeholders::_2)
	                    );
}

void UStateProcessorComponent::EndPlay(
	const EEndPlayReason::Type EndPlayReason
	)
{
	auto CharacterPtr = GetOwner<FOwnerPawnType>();
	if (CharacterPtr)
	{
		auto GASCompPtr = CharacterPtr->GetCharacterAbilitySystemComponent();
		GASCompPtr->RegisterGenericGameplayTagEvent().Remove(OnGETagCountChangedHandle);
	}

	Super::EndPlay(EndPlayReason);
}

void UStateProcessorComponent::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
	) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UStateProcessorComponent::OnGroupManaggerReady(
	AGroupManagger* NewGroupSharedInfoPtr
	)
{
#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
	}
#endif
	auto CharacterPtr = GetOwner<FOwnerPawnType>();
	if (CharacterPtr)
	{
		auto GASCompPtr = CharacterPtr->GetCharacterAbilitySystemComponent();
		OnGETagCountChangedHandle = GASCompPtr->RegisterGenericGameplayTagEvent().AddUObject(
			 this,
			 &ThisClass::OnGameplayEffectTagCountChanged
			);
	}
}

void UStateProcessorComponent::RemoveGetOrientFunc(
	int32 Prority
	)
{
	if (GetOrientFuncMap.Contains(Prority))
	{
		GetOrientFuncMap.Remove(Prority);
	}
}

TSharedPtr<FCharacterStateInfo> UStateProcessorComponent::GetCharacterState(
	const FGameplayTag& CSTag
	) const
{
	return nullptr;
}

auto UStateProcessorComponent::BindCharacterStateChanged(
	const std::function<void(
		ECharacterStateType,
		UCS_Base*


		
		)>& Func
	)
	-> UStateProcessorComponent::FCharacterStateChanged::FCallbackHandleSPtr
{
	auto CallbackHandle = CharacterStateChangedContainer.AddCallback(Func);

	for (const auto& Iter : CharacterStateMap)
	{
		CharacterStateChangedContainer(ECharacterStateType::kActive, Iter.Value);
	}

	return CallbackHandle;
}

auto UStateProcessorComponent::BindCharacterStateMapChanged(
	const std::function<void(
		const TSharedPtr<FCharacterStateInfo>&,
		bool
		)>& Func
	)
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
	}
}

void UStateProcessorComponent::OnGameplayEffectTagCountChanged(
	const FGameplayTag Tag,
	int32 Count
	)
{
	auto Lambda = [&]
	{
		const auto Value = Count > 0;
		return Value;
	};

	if (Tag.MatchesTag(UGameplayTagsLibrary::State_RootMotion))
	{
		if (Tag.MatchesTagExact(UGameplayTagsLibrary::State_RootMotion_FlyAway))
		{
			auto CharacterPtr = GetOwner<FOwnerPawnType>();
			if (CharacterPtr)
			{
				auto CharacterMovementPtr = CharacterPtr->GetGravityMovementComponent();
				CharacterMovementPtr->SetMovementMode(
				                                      Lambda() ?
					                                      EMovementMode::MOVE_Flying :
					                                      EMovementMode::MOVE_Falling
				                                     );
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
		else if (Tag.MatchesTagExact(UGameplayTagsLibrary::MovementStateAble_CantRotation_All))
		{
			auto CharacterPtr = GetOwner<FOwnerPawnType>();
			if (CharacterPtr)
			{
				auto CharacterMovementPtr = CharacterPtr->GetGravityMovementComponent();
				CharacterMovementPtr->bSkip_Rotation_All = Lambda();
			}
		}
		else if (Tag.MatchesTagExact(UGameplayTagsLibrary::MovementStateAble_CantRotation_OrientToMovement))
		{
			auto CharacterPtr = GetOwner<FOwnerPawnType>();
			if (CharacterPtr)
			{
				auto CharacterMovementPtr = CharacterPtr->GetGravityMovementComponent();
				CharacterMovementPtr->bSkip_Rotation_OrientToMovement = Lambda();
			}
		}
		else if (Tag.MatchesTagExact(UGameplayTagsLibrary::MovementStateAble_CantRotation_Controller))
		{
			auto CharacterPtr = GetOwner<FOwnerPawnType>();
			if (CharacterPtr)
			{
				auto CharacterMovementPtr = CharacterPtr->GetGravityMovementComponent();
				CharacterMovementPtr->bSkip_Rotation_Controller = Lambda();
			}
		}
		else if (Tag.MatchesTagExact(UGameplayTagsLibrary::MovementStateAble_SkipSlideAlongSurface))
		{
			auto CharacterPtr = GetOwner<FOwnerPawnType>();
			if (CharacterPtr)
			{
				auto CharacterMovementPtr = CharacterPtr->GetGravityMovementComponent();
				CharacterMovementPtr->bSkip_SkipSlideAlongSurface = Lambda();
			}
		}
		else if (Tag.MatchesTagExact(UGameplayTagsLibrary::MovementStateAble_SkipFlyingCheck))
		{
			auto CharacterPtr = GetOwner<FOwnerPawnType>();
			if (CharacterPtr)
			{
				auto CharacterMovementPtr = CharacterPtr->GetGravityMovementComponent();
				CharacterMovementPtr->bSkip_SkipFlyingCheck = Lambda();
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
	else if (Tag.MatchesTagExact(UGameplayTagsLibrary::State_IgnoreLookInput))
	{
#if UE_EDITOR || UE_CLIENT
		if (GetOwnerRole() == ROLE_AutonomousProxy)
		{
			auto CharacterPtr = GetOwner<FOwnerPawnType>();
			if (CharacterPtr)
			{
				CharacterPtr->GetController()->SetIgnoreLookInput(Lambda());
			}
		}
#endif
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
			                                                                   Pawn_Object,
			                                                                   Lambda() ?
				                                                                   ECollisionResponse::ECR_Overlap :
				                                                                   ECollisionResponse::ECR_Block
			                                                                  );
		}
	}
	else if (Tag.MatchesTagExact(UGameplayTagsLibrary::State_Invisible))
	{
		auto CharacterPtr = GetOwner<FOwnerPawnType>();
		if (CharacterPtr)
		{
			CharacterPtr->GetCharacterAbilitySystemComponent()->SwitchInvisible(Lambda());
		}
	}
	else if (Tag.MatchesTagExact(UGameplayTagsLibrary::State_Debuff_Suppress))
	{
		auto CharacterPtr = GetOwner<FOwnerPawnType>();
		if (CharacterPtr)
		{
			auto AnimInsPtr = CharacterPtr->GetAnimationIns<UHumanAnimInstance>();
			AnimInsPtr->SetIsMelee(Lambda());
		}
	}
	else if (Tag.MatchesTagExact(UGameplayTagsLibrary::State_Debuff_Stun))
	{
		auto CharacterPtr = GetOwner<FOwnerPawnType>();
		if (CharacterPtr)
		{
		}
	}
}

bool UStateProcessorComponent::GetOrientDefautl(
	FRotator& DesiredRotation,
	bool& bIsImmediatelyRot
	)
{
	auto FocusCharactersAry = GetTargetCharactersAry();
	if (FocusCharactersAry.IsValidIndex(0) && FocusCharactersAry[0].IsValid())
	{
		auto CharacterPtr = GetOwner<FOwnerPawnType>();
		if (CharacterPtr)
		{
			const auto CurrentLocation = CharacterPtr->GetActorLocation();
			// Normalized
			const auto Z = -UKismetGravityLibrary::GetGravity();
			DesiredRotation = UKismetMathLibrary::MakeRotFromZX(
			                                                    Z,
			                                                    FocusCharactersAry[0]->GetActorLocation() -
			                                                    CurrentLocation
			                                                   );
			return true;
		}
	}

	return false;
}

void UStateProcessorComponent::OnCharacterStateChanged(
	ECharacterStateType CharacterStateType,
	UCS_Base* CharacterStatePtr
	)
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
