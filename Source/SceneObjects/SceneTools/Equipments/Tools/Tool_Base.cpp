
#include "Tool_Base.h"

#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"

#include "PlanetGameplayAbility.h"
#include "ToolFuture_Base.h"

void ATool_Base::DoActionByCharacter(FOnwerType* CharacterPtr, EEquipmentActionType ActionType)
{
}

void ATool_Base::AttachToCharacter(FOnwerType* CharacterPtr)
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
		auto CharacterPtr = Cast<FOnwerType>(GetOwner());

		CharacterPtr->GetAbilitySystemComponent()->ClearAbility(EquipmentAbilitieHandle);
	}

	Super::EndPlay(EndPlayReason);
}
