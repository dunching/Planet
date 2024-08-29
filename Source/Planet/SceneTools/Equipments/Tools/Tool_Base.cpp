
#include "Tool_Base.h"

#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"

#include "CharacterBase.h"
#include "HumanCharacter.h"
#include "Animation/AnimInstanceBase.h"
#include "PlanetGameplayAbility.h"
#include "ToolFuture_Base.h"

void ATool_Base::DoActionByCharacter(AHumanCharacter* CharacterPtr, EEquipmentActionType ActionType)
{
}

void ATool_Base::AttachToCharacter(ACharacterBase* CharacterPtr)
{
	EquipmentAbilitieHandle = CharacterPtr->GetAbilitySystemComponent()->GiveAbility(
		FGameplayAbilitySpec(
			EquipmentAbilities,
			1
		)
	);
}

void ATool_Base::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (EndPlayReason == EEndPlayReason::Destroyed)
	{
		auto CharacterPtr = Cast<AHumanCharacter>(GetOwner());

		CharacterPtr->GetAbilitySystemComponent()->ClearAbility(EquipmentAbilitieHandle);
	}

	Super::EndPlay(EndPlayReason);
}
