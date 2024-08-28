
#include "StateProcessorComponent.h"

#include <GameFramework/CharacterMovementComponent.h>

#include "GravityMovementComponent.h"

#include "CharacterBase.h"
#include "SceneElement.h"
#include "HoldingItemsComponent.h"
#include "CharacterAttributesComponent.h"
#include "CharacterAttibutes.h"
#include "InteractiveSkillComponent.h"
#include "AssetRefMap.h"
#include "GameplayTagsSubSystem.h"

FName UStateProcessorComponent::ComponentName = TEXT("StateProcessorComponent");

void UStateProcessorComponent::BeginPlay()
{
	Super::BeginPlay();

	auto CharacterPtr = GetOwner<ACharacterBase>();
	if (CharacterPtr)
	{
		auto GASCompPtr = CharacterPtr->GetAbilitySystemComponent();
		OnGameplayEffectTagCountChangedHandle = GASCompPtr->RegisterGenericGameplayTagEvent().AddUObject(
			this, &ThisClass::OnGameplayEffectTagCountChanged
		);
	}
}

void UStateProcessorComponent::OnGameplayEffectTagCountChanged(const FGameplayTag Tag, int32 Count)
{
	const auto Value = Count > 0;
	if (Tag.MatchesTag(UGameplayTagsSubSystem::GetInstance()->RootMotion))
	{

	}
	else if (Tag.MatchesTag(UGameplayTagsSubSystem::GetInstance()->MovementStateAble))
	{
		if (Tag.MatchesTagExact(UGameplayTagsSubSystem::GetInstance()->MovementStateAble_CantJump))
		{
			auto CharacterPtr = GetOwner<ACharacterBase>();
			if (CharacterPtr)
			{
				auto CharacterMovementPtr = CharacterPtr->GetGravityMovementComponent();
				CharacterMovementPtr->MovementState.bCanJump = !Value;
			}
		}
		else if (Tag.MatchesTagExact(UGameplayTagsSubSystem::GetInstance()->MovementStateAble_CantPathFollowMove))
		{
			auto CharacterPtr = GetOwner<ACharacterBase>();
			if (CharacterPtr)
			{
				auto CharacterMovementPtr = CharacterPtr->GetGravityMovementComponent();
				CharacterMovementPtr->bSkip_PathFollow = Value;
			}
		}
		else if (Tag.MatchesTagExact(UGameplayTagsSubSystem::GetInstance()->MovementStateAble_CantPlayerInputMove))
		{
			auto CharacterPtr = GetOwner<ACharacterBase>();
			if (CharacterPtr)
			{
				auto CharacterMovementPtr = CharacterPtr->GetGravityMovementComponent();
				CharacterMovementPtr->bSkip_PlayerInput = Value;
			}
		}
		else if (Tag.MatchesTagExact(UGameplayTagsSubSystem::GetInstance()->MovementStateAble_CantRootMotion))
		{
			auto CharacterPtr = GetOwner<ACharacterBase>();
			if (CharacterPtr)
			{
				auto CharacterMovementPtr = CharacterPtr->GetGravityMovementComponent();
				CharacterMovementPtr->bSkipRootMotion = Value;
			}
		}
		else if (Tag.MatchesTagExact(UGameplayTagsSubSystem::GetInstance()->MovementStateAble_CantRotation))
		{
			auto CharacterPtr = GetOwner<ACharacterBase>();
			if (CharacterPtr)
			{
				auto CharacterMovementPtr = CharacterPtr->GetGravityMovementComponent();
				CharacterMovementPtr->bSkipRotation = Value;
			}
		}
	}
}
