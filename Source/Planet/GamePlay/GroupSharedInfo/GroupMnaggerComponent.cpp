
#include "GroupMnaggerComponent.h"

#include "Net/UnrealNetwork.h"

#include "GroupMnaggerComponent.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "CharacterBase.h"
#include "HumanCharacter.h"
#include "CharacterTitle.h"
#include "HumanAIController.h"
#include "PlanetControllerInterface.h"
#include "PlanetPlayerState.h"
#include "HoldingItemsComponent.h"
#include "ItemProxyContainer.h"
#include "GameOptions.h"
#include "SceneUnitTable.h"
#include "ItemProxy.h"
#include "Planet.h"
#include "LogWriter.h"
#include "TeamConfigureomponent.h"
#include "TeamConfigure.h"
#include "HumanCharacter_AI.h"

#ifdef WITH_EDITOR
static TAutoConsoleVariable<int32> GroupMnaggerComponent_KnowCharaterChanged(
	TEXT("GroupMnaggerComponent.KnowCharaterChanged"),
	1,
	TEXT("")
	TEXT(" default: 0"));
#endif

void UTeamMatesHelperComponent::AddCharacterToTeam(const TSharedPtr<FCharacterUnitType>& CharacterUnitPtr, int32 Index)
{
#if UE_EDITOR || UE_SERVER
	if (GetOwnerRole() == ROLE_Authority)
	{
		AddCharacter(CharacterUnitPtr);
	}
#endif

#if UE_EDITOR || UE_CLIENT
	if (GetOwnerRole() == ROLE_AutonomousProxy)
	{
		AddCharacter(CharacterUnitPtr);
		if (CharacterUnitPtr)
		{
			AddCharacterToTeam_Server(CharacterUnitPtr->GetID(), Index);
		}
		else
		{
			AddCharacterToTeam_Server(FGuid(), Index);
		}
	}
#endif

	FTeammate_FASI Teammate_FASI;

	Teammate_FASI.Index = Index;
	if (CharacterUnitPtr)
	{
		Teammate_FASI.CharacterProxyID = CharacterUnitPtr->GetID();
	}
	else
	{
		Teammate_FASI.CharacterProxyID = FGuid();
	}

	auto PSPtr = GetOwner<FOwnerType>()->GetPlayerState<APlanetPlayerState>();
	PSPtr->GetTeamConfigureomponent()->UpdateConfigure(Teammate_FASI);
}

void UTeamMatesHelperComponent::OnAddToNewTeam(const TSharedPtr<FCharacterUnitType>& CharacterUnitPtr)
{
	TeamHelperChangedDelegateContainer.ExcuteCallback();
}

void UTeamMatesHelperComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTeamMatesHelperComponent::AddCharacterToTeam_Server_Implementation(const FGuid& ProxtID, int32 Index)
{
	const auto CharacterProxySPtr =
		GetOwner<FOwnerType>()->GetHoldingItemsComponent()->FindUnit_Character(ProxtID);

	AddCharacterToTeam(CharacterProxySPtr, Index);
}

void UTeamMatesHelperComponent::SpwanTeammateCharacter_Server_Implementation()
{
	const auto OwnerPtr = GetOwner<FOwnerType>();
	auto PSPtr = GetOwner<FOwnerType>()->GetPlayerState<APlanetPlayerState>();
	if (PSPtr)
	{
		auto& Ref = PSPtr->TeamConfigureompConentPtr->Teammate_FASI_Container.Items;
		for (auto& Iter : Ref)
		{
			auto CharacterProxySPtr =
				OwnerPtr->GetHoldingItemsComponent()->FindUnit_Character(Iter.CharacterProxyID);

			if (CharacterProxySPtr)
			{
				const auto Pt = OwnerPtr->GetActorLocation();
				const auto Rot = OwnerPtr->GetActorRotation();
				FTransform Transform(Pt + (Rot.Vector() * 100));

				CharacterProxySPtr->SpwanCharacter(Transform);
			}

			auto PreviousCharacterProxySPtr =
				OwnerPtr->GetHoldingItemsComponent()->FindUnit_Character(Iter.PreviousCharacterProxyID);

			if (CharacterProxySPtr != PreviousCharacterProxySPtr)
			{
				if (PreviousCharacterProxySPtr)
				{
					PreviousCharacterProxySPtr->DestroyCharacter();
				}
			}
		}
	}
}

void UTeamMatesHelperComponent::SpwanTeammateCharacter()
{
	SpwanTeammateCharacter_Server();
}

UTeamMatesHelperComponent::UTeamMatesHelperComponent(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UTeamMatesHelperComponent::AddCharacter(FPawnType* PCPtr)
{
	auto CharacterUnitPtr = PCPtr->GetCharacterUnit();
	AddCharacter(CharacterUnitPtr);
}

void UTeamMatesHelperComponent::AddCharacter(const TSharedPtr<FCharacterUnitType>& CharacterUnitPtr)
{
	MembersSet.Add(CharacterUnitPtr);
	CharactersAry.Add(CharacterUnitPtr->GetID());

	MembersChanged.ExcuteCallback(EGroupMateChangeType::kAdd, CharacterUnitPtr);
}

bool UTeamMatesHelperComponent::IsMember(const TSharedPtr<FCharacterUnitType>& CharacterUnitPtr) const
{
	for (auto Iter : MembersSet)
	{
		if (Iter == CharacterUnitPtr)
		{
			return true;
		}
	}

	return false;
}

void UTeamMatesHelperComponent::SwitchTeammateOption(ETeammateOption InTeammateOption)
{
	TeammateOption = InTeammateOption;

	TeammateOptionChanged.ExcuteCallback(InTeammateOption, OwnerCharacterUnitPtr);
}

ETeammateOption UTeamMatesHelperComponent::GetTeammateOption() const
{
	return TeammateOption;
}

void UTeamMatesHelperComponent::AddKnowCharacter(ACharacterBase* CharacterPtr)
{
	for (auto& Iter : KnowCharatersSet)
	{
		if (Iter.Key == CharacterPtr)
		{
			Iter.Value++;

			return;
		}
	}

	KnowCharatersSet.Add({ CharacterPtr, 1 });
	KnowCharaterChanged(CharacterPtr, true);
#ifdef WITH_EDITOR
	if (GroupMnaggerComponent_KnowCharaterChanged.GetValueOnGameThread())
	{
		PRINTINVOKEWITHSTR(FString(TEXT("")));
	}
#endif
}

void UTeamMatesHelperComponent::RemoveKnowCharacter(ACharacterBase* CharacterPtr)
{
	for (int32 Index = 0; Index < KnowCharatersSet.Num(); Index++)
	{
		if (KnowCharatersSet[Index].Key == CharacterPtr)
		{
			KnowCharatersSet[Index].Value--;

			if (KnowCharatersSet[Index].Value <= 0)
			{
				KnowCharatersSet.RemoveAt(Index);
			}
			KnowCharaterChanged(CharacterPtr, false);
#ifdef WITH_EDITOR
			if (GroupMnaggerComponent_KnowCharaterChanged.GetValueOnGameThread())
			{
				PRINTINVOKEWITHSTR(FString(TEXT("")));
			}
#endif
			return;
		}
	}
}

TWeakObjectPtr<ACharacterBase> UTeamMatesHelperComponent::GetKnowCharacter() const
{
	if (ForceKnowCharater.IsValid())
	{
		return ForceKnowCharater;
	}

	for (auto Iter : KnowCharatersSet)
	{
		return Iter.Key;
	}

	return nullptr;
}

FName UTeamMatesHelperComponent::ComponentName = TEXT("TeamMatesHelperComponent");

void UTeamMatesHelperComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, CharactersAry, COND_None);
}

void UTeamMatesHelperComponent::OnRep_GroupSharedInfoChanged()
{
	TeamHelperChangedDelegateContainer();
}
