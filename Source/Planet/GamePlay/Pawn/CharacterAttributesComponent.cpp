
#include "CharacterAttributesComponent.h"

#include "CharacterBase.h"
#include "InteractiveSkillComponent.h"
#include "GravityMovementComponent.h"
#include "AssetRefMap.h"
#include "GameplayTagsSubSystem.h"
#include "InteractiveBaseGAComponent.h"
#include "PlanetControllerInterface.h"

UCharacterAttributesComponent::UCharacterAttributesComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 1.f;
}

void UCharacterAttributesComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ProcessCharacterAttributes();
}

const FCharacterAttributes& UCharacterAttributesComponent::GetCharacterAttributes() const
{
	return CharacterAttributes;
}

FCharacterAttributes& UCharacterAttributesComponent::GetCharacterAttributes()
{
	return CharacterAttributes;
}

void UCharacterAttributesComponent::ProcessCharacterAttributes()
{
	auto OwnerPtr = GetOwner<FOwnerType>();
	auto CharacterPtr = OwnerPtr->GetRealCharacter();
	if (CharacterPtr)
	{
		TMap<ECharacterPropertyType, FBaseProperty> ModifyPropertyMap;

		const auto DataSource = UGameplayTagsSubSystem::GetInstance()->DataSource_Regular;

		FGameplayAbilityTargetData_GASendEvent* GAEventDataPtr = new FGameplayAbilityTargetData_GASendEvent(CharacterPtr);

		GAEventDataPtr->TriggerCharacterPtr = CharacterPtr;

		FGAEventData GAEventData(CharacterPtr, CharacterPtr);

		GAEventData.DataSource = UGameplayTagsSubSystem::GetInstance()->DataSource_Character;

		// »ù´¡»Ø¸´
		{
			GAEventData.HP = CharacterAttributes.HPReplay.GetCurrentValue();

			if (
				CharacterPtr->GetCharacterMovement()->Velocity.Length() > 0.f
				)
			{
				if (!CharacterPtr->GetCharacterMovement()->HasRootMotionSources())
				{
				}
			}
			else if (
				CharacterPtr->GetAbilitySystemComponent()->K2_HasMatchingGameplayTag(UGameplayTagsSubSystem::GetInstance()->Running)
				)
			{
				if (!CharacterPtr->GetCharacterMovement()->HasRootMotionSources())
				{
					GAEventData.PP = CharacterAttributes.RunningConsume.GetCurrentValue();
				}
			}
			else
			{
				GAEventData.PP = CharacterAttributes.PPReplay.GetCurrentValue();
			}
		}

		GAEventDataPtr->DataAry.Add(GAEventData);

		auto ICPtr = CharacterPtr->GetInteractiveBaseGAComponent();
		ICPtr->SendEventImp(GAEventDataPtr);
	}
}

FName UCharacterAttributesComponent::ComponentName = TEXT("CharacterAttributesComponent");

void UCharacterAttributesComponent::BeginPlay()
{
	Super::BeginPlay();

	CharacterAttributes.InitialData();
}
