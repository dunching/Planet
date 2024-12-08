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
#include "ItemProxy_Container.h"
#include "GameOptions.h"
#include "SceneProxyTable.h"
#include "ItemProxy_Minimal.h"
#include "HumanCharacter_AI.h"
#include "LogWriter.h"
#include "ProxySycHelperComponent.h"
#include "TeamConfigure.h"
#include "GroupSharedInfo.h"
#include "ItemProxy_Character.h"

#ifdef WITH_EDITOR
static TAutoConsoleVariable<int32> GroupMnaggerComponent_KnowCharaterChanged(
	TEXT("GroupMnaggerComponent.KnowCharaterChanged"),
	1,
	TEXT("")
	TEXT(" default: 0"));
#endif

UTeamMatesHelperComponent::UTeamMatesHelperComponent(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UTeamMatesHelperComponent::AddCharacterToTeam(const TSharedPtr<FCharacterProxyType>& CharacterProxyPtr, int32 Index)
{
#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		AddCharacter(CharacterProxyPtr);
	}
#endif

#if UE_EDITOR || UE_CLIENT
	if (GetNetMode() == NM_Client)
	{
		AddCharacter(CharacterProxyPtr);
		if (CharacterProxyPtr)
		{
			AddCharacterToTeam_Server(CharacterProxyPtr->GetID(), Index);
		}
		else
		{
			AddCharacterToTeam_Server(FGuid(), Index);
		}
	}
#endif
}

void UTeamMatesHelperComponent::OnAddToNewTeam(const TSharedPtr<FCharacterProxyType>& CharacterProxyPtr)
{
	TeamHelperChangedDelegateContainer.ExcuteCallback();
}

void UTeamMatesHelperComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UTeamMatesHelperComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTeamMatesHelperComponent::AddCharacterToTeam_Server_Implementation(const FGuid& ProxtID, int32 Index)
{
	const auto CharacterProxySPtr =
		GetOwner<FOwnerType>()->GetHoldingItemsComponent()->FindProxy_Character(ProxtID);

	AddCharacterToTeam(CharacterProxySPtr, Index);
}

void UTeamMatesHelperComponent::SpwanTeammateCharacter_Server_Implementation()
{
	auto PlayerCharacterPtr = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (!PlayerCharacterPtr)
	{
		return;
	}

	const auto PlayerCharacterLocation = PlayerCharacterPtr->GetActorLocation();
	const auto PlayerCharacterRotation = PlayerCharacterPtr->GetActorRotation();

	FTransform Transform;

	const auto OwnerPtr = GetOwner<FOwnerType>();
	for (int32 Index = 0; Index < TeamConfigure.CharactersAry.Num(); Index++)
	{
		const auto CharacterProxySPtr =
			GetOwner<FOwnerType>()->GetHoldingItemsComponent()->FindProxy_Character(TeamConfigure.CharactersAry[Index]);
		if (CharacterProxySPtr)
		{
			Transform.SetLocation(PlayerCharacterLocation + (PlayerCharacterRotation.Vector() * 100));
			auto AICharacterPtr = CharacterProxySPtr->SpwanCharacter(Transform);
			if (AICharacterPtr)
			{
				AICharacterPtr->SetGroupSharedInfo(OwnerPtr);
				AICharacterPtr->SetCharacterID(CharacterProxySPtr->GetID());
			}
		}
	}
}

void UTeamMatesHelperComponent::SpwanTeammateCharacter()
{
	SpwanTeammateCharacter_Server();
}

void UTeamMatesHelperComponent::AddCharacter(FPawnType* PCPtr)
{
	auto CharacterProxyPtr = PCPtr->GetCharacterProxy();
	AddCharacter(CharacterProxyPtr);
}

void UTeamMatesHelperComponent::AddCharacter(const TSharedPtr<FCharacterProxyType>& CharacterProxyPtr)
{
	MembersSet.Add(CharacterProxyPtr);
	TeamConfigure.CharactersAry.Add(CharacterProxyPtr->GetID());

	MembersChanged.ExcuteCallback(EGroupMateChangeType::kAdd, CharacterProxyPtr);
}

bool UTeamMatesHelperComponent::IsMember(const TSharedPtr<FCharacterProxyType>& CharacterProxyPtr) const
{
	for (auto Iter : MembersSet)
	{
		if (Iter == CharacterProxyPtr)
		{
			return true;
		}
	}

	return false;
}

bool UTeamMatesHelperComponent::IsMember(const FGuid& CharacterID) const
{
	for (auto Iter : MembersSet)
	{
		if (Iter->GetID() == CharacterID)
		{
			return true;
		}
	}

	return false;
}

void UTeamMatesHelperComponent::SwitchTeammateOption(ETeammateOption InTeammateOption)
{
	TeammateOption = InTeammateOption;

	TeammateOptionChanged.ExcuteCallback(InTeammateOption, OwnerCharacterProxyPtr);
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

	KnowCharatersSet.Add({CharacterPtr, 1});
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

	DOREPLIFETIME_CONDITION(ThisClass, TeamConfigure, COND_None);
}

void UTeamMatesHelperComponent::OnRep_GroupSharedInfoChanged()
{
	TeamHelperChangedDelegateContainer();
	PRINTINVOKEWITHSTR(FString(TEXT("")));
}
