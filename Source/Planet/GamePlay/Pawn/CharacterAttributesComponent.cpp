#include "CharacterAttributesComponent.h"

#include "Net/UnrealNetwork.h"

#include "CharacterBase.h"
#include "ProxyProcessComponent.h"
#include "GravityMovementComponent.h"
#include "AssetRefMap.h"
#include "GameplayTagsLibrary.h"
#include "BaseFeatureComponent.h"
#include "GE_CharacterInitail.h"
#include "CharacterAttibutes.h"
#include "AS_Character.h"

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

const UAS_Character* UCharacterAttributesComponent::GetCharacterAttributes() const
{
	auto CharacterPtr = GetOwner<FOwnerType>();
	if (!CharacterPtr)
	{
		return nullptr;
	}

	auto GASPtr = CharacterPtr->GetAbilitySystemComponent();

	return Cast<const UAS_Character>(GASPtr->GetAttributeSet(UAS_Character::StaticClass()));
}

void UCharacterAttributesComponent::ProcessCharacterAttributes()
{
	auto CharacterPtr = GetOwner<FOwnerType>();
	if (!CharacterPtr)
	{
		return;
	}

	if (
		CharacterPtr->GetAbilitySystemComponent()->HasMatchingGameplayTag(UGameplayTagsLibrary::DeathingTag) ||
		CharacterPtr->GetAbilitySystemComponent()->HasMatchingGameplayTag(UGameplayTagsLibrary::Respawning)
	)
	{
	}
	else
	{
		TMap<ECharacterPropertyType, FBaseProperty> ModifyPropertyMap;

		const auto DataSource = UGameplayTagsLibrary::DataSource_Regular;

		FGameplayAbilityTargetData_GASendEvent* GAEventDataPtr = new FGameplayAbilityTargetData_GASendEvent(
			CharacterPtr);

		GAEventDataPtr->TriggerCharacterPtr = CharacterPtr;

		FGAEventData GAEventData(CharacterPtr, CharacterPtr);

		GAEventData.DataSource = UGameplayTagsLibrary::DataSource_Character;

		// 基础回复
		{
			// GAEventData.DataModify.Add(ECharacterPropertyType::HP, CharacterAttributes.HP_Replay.GetCurrentValue());
			//
			// if (
			// 	CharacterPtr->GetCharacterMovement()->Velocity.Length() > 0.f
			// 	)
			// {
			// 	GAEventData.DataModify.Add(ECharacterPropertyType::PP, FMath::Max(1, CharacterAttributes.PP_Replay.GetCurrentValue() / 2));
			// }
			// else
			// {
			// 	GAEventData.DataModify.Add(ECharacterPropertyType::PP, CharacterAttributes.PP_Replay.GetCurrentValue());
			// }
			//
			// if (
			// 	CharacterPtr->GetBaseFeatureComponent()->IsInFighting()
			// 	)
			// {
			// }
			// else
			// {
			// 	GAEventData.DataModify.Add(ECharacterPropertyType::Mana, CharacterAttributes.Mana_Replay.GetCurrentValue());
			// }
		}

		GAEventDataPtr->DataAry.Add(GAEventData);

		auto ICPtr = CharacterPtr->GetBaseFeatureComponent();
		ICPtr->SendEventImp(GAEventDataPtr);
	}
}

FName UCharacterAttributesComponent::ComponentName = TEXT("CharacterAttributesComponent");

void UCharacterAttributesComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME_CONDITION(ThisClass, CharacterAttributes, COND_SimulatedOnly);
	// DOREPLIFETIME(ThisClass, CharacterAttributeSetPtr);
}

void UCharacterAttributesComponent::BeginPlay()
{
	Super::BeginPlay();

#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		auto CharacterPtr = GetOwner<FOwnerType>();
		if (!CharacterPtr)
		{
			return;
		}

		auto GASPtr = CharacterPtr->GetAbilitySystemComponent();

		// 初始化
		{
			auto Spec = GASPtr->MakeOutgoingSpec(GE_InitailCharacterClass, 1, GASPtr->MakeEffectContext());
			const auto GEHandle = GASPtr->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
			if (!GEHandle.IsValid())
			{
				// checkNoEntry();
			}
		}
		// 自动回复
		{
			auto SpecHandle = GASPtr->MakeOutgoingSpec(GE_CharacterReplyClass, 1, GASPtr->MakeEffectContext());

			SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyDuration_Immediate);
			SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyType_HP);

			const auto GEHandle = GASPtr->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			if (!GEHandle.IsValid())
			{
				// checkNoEntry();
			}
		}
	}
#endif
}
