#include "CharacterAttributesComponent.h"

#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

#include "CharacterBase.h"
#include "ProxyProcessComponent.h"
#include "GravityMovementComponent.h"
#include "AssetRefMap.h"
#include "GameplayTagsLibrary.h"
#include "CharacterAbilitySystemComponent.h"
#include "GE_CharacterInitail.h"
#include "CharacterAttibutes.h"
#include "AS_Character.h"
#include "GroupManagger.h"
#include "LogWriter.h"
#include "TeamMatesHelperComponent.h"

UCharacterAttributesComponent::UCharacterAttributesComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 1.f;

	CharacterID = FGuid::NewGuid();

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
		// ProcessCharacterAttributes();
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

	auto GASPtr = CharacterPtr->GetCharacterAbilitySystemComponent();

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
		CharacterPtr->GetCharacterAbilitySystemComponent()->HasMatchingGameplayTag(UGameplayTagsLibrary::DeathingTag) ||
		CharacterPtr->GetCharacterAbilitySystemComponent()->HasMatchingGameplayTag(UGameplayTagsLibrary::Respawning)
	)
	{
	}
	else
	{
	}
}

float UCharacterAttributesComponent::GetRate() const
{
	const auto GAPerformSpeed = GetCharacterAttributes()->GetPerformSpeed();
	const float Rate = static_cast<float>(GAPerformSpeed) / 100;

	return Rate;
}

void UCharacterAttributesComponent::SetCharacterID(const FGuid& InCharacterID)
{
	CharacterID = InCharacterID;
}

FGuid UCharacterAttributesComponent::GetCharacterID() const
{
	return CharacterID;
}

FName UCharacterAttributesComponent::ComponentName = TEXT("CharacterAttributesComponent");

void UCharacterAttributesComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// DOREPLIFETIME_CONDITION(ThisClass, CharacterID, COND_AutonomousOnly);
	DOREPLIFETIME_CONDITION(ThisClass, CharacterID, COND_InitialOnly);

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

		auto GASPtr = CharacterPtr->GetCharacterAbilitySystemComponent();

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

			SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyType_BaseValue_Addtive);

			const auto GEHandle = GASPtr->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			if (!GEHandle.IsValid())
			{
				// checkNoEntry();
			}
		}
	}
#endif
}

ACharacterBase* UGameplayStatics_Character::GetCharacterByID(
	const UObject* WorldContextObject,
	TSubclassOf<ACharacterBase> ActorClass,
	const FGuid& CharacterID)
{
	TArray<AActor*> ActorAry;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, ActorClass, ActorAry);
	for (auto Iter : ActorAry)
	{
		if (auto CharacterPtr = Cast<ACharacterBase>(Iter))
		{
			if (CharacterPtr->GetCharacterAttributesComponent()->GetCharacterID() == CharacterID)
			{
				return CharacterPtr;
			}
		}
	}

	return nullptr;
}

void UCharacterAttributesComponent::OnRep_GetCharacterProxyID()
{
	PRINTINVOKEINFO();
	
	bCharacterIDIsReplicated = true;

	auto CharacterPtr = GetOwner<FOwnerType>();
	if (!CharacterPtr)
	{
		return;
	}

	CharacterPtr->OnRep_GroupSharedInfoChanged();
}

void UCharacterAttributesComponent::OnRep_CharacterID()
{
#if UE_EDITOR || UE_CLIENT
	if (GetNetMode() == NM_Client)
	{
		auto PlayerCharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
		if (PlayerCharacterPtr)
		{
			// 确认跟当前玩家的关系
			const auto bIsMember = PlayerCharacterPtr->GetGroupSharedInfo()->GetTeamMatesHelperComponent()->IsMember(
				CharacterID);

			auto CharacterPtr = GetOwner<FOwnerType>();
			CharacterPtr->SetCampType(
				bIsMember ? ECharacterCampType::kTeamMate : ECharacterCampType::kEnemy
			);
		}
	}
#endif
}
