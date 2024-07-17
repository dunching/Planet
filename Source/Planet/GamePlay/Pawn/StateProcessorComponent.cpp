
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
	if (Tag.MatchesTagExact(UGameplayTagsSubSystem::GetInstance()->FlyAway))
	{

	}
	else if (Tag.MatchesTagExact(UGameplayTagsSubSystem::GetInstance()->Silent))
	{

	}
	else if (Tag.MatchesTagExact(UGameplayTagsSubSystem::GetInstance()->Stun))
	{
		auto CharacterPtr = GetOwner<ACharacterBase>();
		if (CharacterPtr)
		{
			auto CharacterMovementPtr = CharacterPtr->GetGravityMovementComponent();
			CharacterMovementPtr->bSkilPerformMovement = Count > 0;
		}
	}
}
