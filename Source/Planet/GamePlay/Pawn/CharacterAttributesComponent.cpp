
#include "CharacterAttributesComponent.h"

#include "Net/UnrealNetwork.h"

#include "CharacterBase.h"
#include "UnitProxyProcessComponent.h"
#include "GravityMovementComponent.h"
#include "AssetRefMap.h"
#include "GameplayTagsSubSystem.h"
#include "BaseFeatureGAComponent.h"
#include "PlanetControllerInterface.h"
#include "CharacterAttibutes.h"

UCharacterAttributesComponent::UCharacterAttributesComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 1.f;

	SetIsReplicatedByDefault(true);
}

void UCharacterAttributesComponent::TickComponent(
	float DeltaTime,
	enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction
)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		ProcessCharacterAttributes();
	}
#endif
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
	auto CharacterPtr = GetOwner<FOwnerType>();
	if (CharacterPtr)
	{
		TMap<ECharacterPropertyType, FBaseProperty> ModifyPropertyMap;

		const auto DataSource = UGameplayTagsSubSystem::GetInstance()->DataSource_Regular;

		FGameplayAbilityTargetData_GASendEvent* GAEventDataPtr = new FGameplayAbilityTargetData_GASendEvent(CharacterPtr);

		GAEventDataPtr->TriggerCharacterPtr = CharacterPtr;

		FGAEventData GAEventData(CharacterPtr, CharacterPtr);

		GAEventData.DataSource = UGameplayTagsSubSystem::GetInstance()->DataSource_Character;

		// 基础回复
		{
			GAEventData.DataModify.Add(ECharacterPropertyType::HP, CharacterAttributes.HP_Replay.GetCurrentValue());

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
					GAEventData.DataModify.Add(ECharacterPropertyType::PP, CharacterAttributes.RunningConsume.GetCurrentValue());
				}
			}
			else
			{
				GAEventData.DataModify.Add(ECharacterPropertyType::PP, CharacterAttributes.PP_Replay.GetCurrentValue());
			}
		}

		GAEventDataPtr->DataAry.Add(GAEventData);

		auto ICPtr = CharacterPtr->GetInteractiveBaseGAComponent();
		ICPtr->SendEventImp(GAEventDataPtr);
	}
}

FName UCharacterAttributesComponent::ComponentName = TEXT("CharacterAttributesComponent");

void UCharacterAttributesComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;

	Params.Condition = COND_OwnerOnly;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, CharacterAttributes, Params);
}

void UCharacterAttributesComponent::BeginPlay()
{
	Super::BeginPlay();
}
