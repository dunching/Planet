
#include "CharacterAttributesComponent.h"

#include "CharacterBase.h"
#include "EquipmentElementComponent.h"
#include "GravityMovementComponent.h"
#include "AssetRefMap.h"

UCharacterAttributesComponent::UCharacterAttributesComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 1.f;
}

void UCharacterAttributesComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CharacterAttributes.HP.AddCurrentValue(CharacterAttributes.HPReplay.GetCurrentValue(), CharacterAttributes.PropertuModify_GUID);
	
	auto CharacterPtr = GetOwner<ACharacterBase>();
	if (CharacterPtr)
	{
		if (
			(CharacterPtr->GetCharacterMovement()->Velocity.Length() > 0.f) &&
			(CharacterPtr->GetEquipmentItemsComponent()->GetCharacterTags().HasTag(UAssetRefMap::GetInstance()->RunningAbilityTag))
			)
		{
			if (!CharacterPtr->GetCharacterMovement()->HasRootMotionSources())
			{
				if (CharacterAttributes.PP.GetCurrentValue() <= 0)
				{
					FGameplayTagContainer GameplayTagContainer{ UAssetRefMap::GetInstance()->RunningAbilityTag };
					CharacterPtr->GetAbilitySystemComponent()->CancelAbilities(&GameplayTagContainer);
				}
				else
				{
					CharacterAttributes.PP.AddCurrentValue(-CharacterAttributes.RunningConsume.GetCurrentValue(), CharacterAttributes.PropertuModify_GUID);
				}
			}
		}
		else
		{
			CharacterAttributes.PP.AddCurrentValue(CharacterAttributes.PPReplay.GetCurrentValue(), CharacterAttributes.PropertuModify_GUID);
		}
	}
}

FName UCharacterAttributesComponent::ComponentName = TEXT("CharacterAttributesComponent");

void UCharacterAttributesComponent::BeginPlay()
{
	Super::BeginPlay();
}
