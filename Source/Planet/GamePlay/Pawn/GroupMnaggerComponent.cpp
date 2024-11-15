
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

#ifdef WITH_EDITOR
static TAutoConsoleVariable<int32> GroupMnaggerComponent_KnowCharaterChanged(
	TEXT("GroupMnaggerComponent.KnowCharaterChanged"),
	1,
	TEXT("")
	TEXT(" default: 0"));
#endif

FName UGroupMnaggerComponent::ComponentName = TEXT("GroupMnaggerComponent");

UGroupMnaggerComponent::UGroupMnaggerComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 1.f / 10;
}

void UGroupMnaggerComponent::TickComponent(
	float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction
)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UGroupMnaggerComponent::AddCharacterToTeam(const TSharedPtr<FCharacterUnitType>& CharacterUnitPtr, int32 Index)
{
#if UE_EDITOR || UE_SERVER
	if (GetOwnerRole() == ROLE_Authority)
	{
		GetTeamHelper()->AddCharacter(CharacterUnitPtr);
	}
#endif

#if UE_EDITOR || UE_CLIENT
	if (GetOwnerRole() == ROLE_AutonomousProxy)
	{
		GetTeamHelper()->AddCharacter(CharacterUnitPtr);
		AddCharacterToTeam_Server(CharacterUnitPtr->GetID(), Index);
	}
#endif

	FTeammate_FASI Teammate_FASI;

	Teammate_FASI.Index = Index;
	Teammate_FASI.CharacterProxyID = CharacterUnitPtr->GetID();

	auto PSPtr = GetOwner<FOwnerType>()->GetPlayerState<APlanetPlayerState>();
	PSPtr->GetTeamConfigureomponent()->UpdateConfigure(Teammate_FASI);
}

void UGroupMnaggerComponent::OnAddToNewTeam(const TSharedPtr<FCharacterUnitType>& CharacterUnitPtr)
{
	TeamHelperSPtr = CharacterUnitPtr->ProxyCharacterPtr->GetGroupMnaggerComponent()->GetTeamHelper();

	TeamHelperChangedDelegateContainer.ExcuteCallback();
}

const TSharedPtr<FTeamMatesHelper>& UGroupMnaggerComponent::GetTeamHelper()
{
	if (!TeamHelperSPtr)
	{
		CreateTeam();
	}
	return TeamHelperSPtr;
}

void UGroupMnaggerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UGroupMnaggerComponent::AddCharacterToTeam_Server_Implementation(const FGuid& ProxtID, int32 Index)
{
	const auto CharacterProxySPtr = GetOwner<FOwnerType>()->GetHoldingItemsComponent()->FindUnit_Character(ProxtID);

	AddCharacterToTeam(CharacterProxySPtr, Index);
}

TSharedPtr<FTeamMatesHelper> UGroupMnaggerComponent::CreateTeam()
{
	TeamHelperSPtr = MakeShared<FTeamMatesHelper>();
	TeamHelperSPtr->OwnerCharacterUnitPtr = GetOwner<FOwnerType>()->GetCharacterUnit();
	
	TeamHelperSPtr->Guid = FGuid::NewGuid();

	TeamHelperChangedDelegateContainer.ExcuteCallback();

	return TeamHelperSPtr;
}

void UGroupMnaggerComponent::SpwanTeammateCharacter()
{

}

void FTeamMatesHelper::AddCharacter(FPawnType* PCPtr)
{
	auto CharacterUnitPtr = PCPtr->GetCharacterUnit();
	AddCharacter(CharacterUnitPtr);
}

void FTeamMatesHelper::AddCharacter(const TSharedPtr<FCharacterUnitType>& CharacterUnitPtr)
{
	MembersSet.Add(CharacterUnitPtr);

	MembersChanged.ExcuteCallback(EGroupMateChangeType::kAdd, CharacterUnitPtr);
}

bool UGroupMnaggerComponent::IsMember(const TSharedPtr<FCharacterUnitType>& CharacterUnitPtr) const
{
	const auto MembersSet = GetOwner<FOwnerType>()->GetHoldingItemsComponent()->GetCharacterProxyAry();

	for (auto Iter : MembersSet)
	{
		if (Iter == CharacterUnitPtr)
		{
			return true;
		}
	}

	return false;
}

void FTeamMatesHelper::SwitchTeammateOption(ETeammateOption InTeammateOption)
{
	TeammateOption = InTeammateOption;

	TeammateOptionChanged.ExcuteCallback(InTeammateOption, OwnerCharacterUnitPtr);
}

ETeammateOption FTeamMatesHelper::GetTeammateOption() const
{
	return TeammateOption;
}

void FTeamMatesHelper::AddKnowCharacter(ACharacterBase* CharacterPtr)
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

void FTeamMatesHelper::RemoveKnowCharacter(ACharacterBase* CharacterPtr)
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

TWeakObjectPtr<ACharacterBase> FTeamMatesHelper::GetKnowCharacter() const
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
