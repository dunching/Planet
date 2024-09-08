
#include "CharacterAttributesComponent.h"

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

	CharacterAttributesSPtr = MakeShared<FCharacterAttributes>();
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

TSharedPtr<FCharacterAttributes> UCharacterAttributesComponent::GetCharacterAttributes() const
{
	return CharacterAttributesSPtr;
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
			GAEventData.HP = CharacterAttributesSPtr->HPReplay.GetCurrentValue();

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
					GAEventData.PP = CharacterAttributesSPtr->RunningConsume.GetCurrentValue();
				}
			}
			else
			{
				GAEventData.PP = CharacterAttributesSPtr->PPReplay.GetCurrentValue();
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

#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		auto SPtr = GetCharacterAttributes();
		auto HPChangedHandle = SPtr->HP.AddOnValueChanged(std::bind(&ThisClass::OnPropertyChanged, this, ECharacterPropertyType::HP, std::placeholders::_2));
		HPChangedHandle->bIsAutoUnregister = false;
	}
#endif
}

void UCharacterAttributesComponent::OnPropertyChanged_Implementation(
	ECharacterPropertyType CharacterPropertyType,
	int32 CurrentValue
)
{
	const auto DataSource = UGameplayTagsSubSystem::GetInstance()->DataSource_Character;

	switch (CharacterPropertyType)
	{
	case ECharacterPropertyType::LiDao:
		break;
	case ECharacterPropertyType::GenGu:
		break;
	case ECharacterPropertyType::ShenFa:
		break;
	case ECharacterPropertyType::DongCha:
		break;
	case ECharacterPropertyType::TianZi:
		break;
	case ECharacterPropertyType::GoldElement:
		break;
	case ECharacterPropertyType::WoodElement:
		break;
	case ECharacterPropertyType::WaterElement:
		break;
	case ECharacterPropertyType::FireElement:
		break;
	case ECharacterPropertyType::SoilElement:
		break;
	case ECharacterPropertyType::HP:
	{
		CharacterAttributesSPtr->HP.SetCurrentValue(CurrentValue, DataSource);
	}
	break;
	case ECharacterPropertyType::BaseAttackPower:
		break;
	case ECharacterPropertyType::Penetration:
		break;
	case ECharacterPropertyType::PercentPenetration:
		break;
	case ECharacterPropertyType::Resistance:
		break;
	case ECharacterPropertyType::GAPerformSpeed:
		break;
	case ECharacterPropertyType::Evade:
		break;
	case ECharacterPropertyType::HitRate:
		break;
	case ECharacterPropertyType::Toughness:
		break;
	case ECharacterPropertyType::CriticalHitRate:
		break;
	case ECharacterPropertyType::CriticalDamage:
		break;
	case ECharacterPropertyType::MoveSpeed:
		break;
	default:
		break;
	}
}
