
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
#include "SceneUnitContainer.h"
#include "GameOptions.h"
#include "SceneUnitTable.h"
#include "SceneElement.h"
#include "Planet.h"
#include "LogWriter.h"

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

void UGroupMnaggerComponent::AddCharacterToGroup(const TSharedPtr<FCharacterUnitType>& CharacterUnitPtr)
{
	//
	GetGroupHelper()->AddCharacter(CharacterUnitPtr);
}

void UGroupMnaggerComponent::AddCharacterToTeam(const TSharedPtr<FCharacterUnitType>& CharacterUnitPtr)
{
	GetGroupHelper()->AddCharacter(CharacterUnitPtr);
}

void UGroupMnaggerComponent::OnAddToNewGroup(const TSharedPtr<FCharacterUnitType>& CharacterUnitPtr)
{
	GroupHelperSPtr = CharacterUnitPtr->ProxyCharacterPtr->GetGroupMnaggerComponent()->GetGroupHelper();

	if (CharacterUnitPtr->ProxyCharacterPtr->IsPlayerControlled())
	{
		TeamHelperSPtr->SwitchTeammateOption(ETeammateOption::kFree);
	}

	GroupHelperChangedDelegateContainer.ExcuteCallback();
}

void UGroupMnaggerComponent::OnAddToNewTeam(const TSharedPtr<FCharacterUnitType>& CharacterUnitPtr)
{
	TeamHelperSPtr = CharacterUnitPtr->ProxyCharacterPtr->GetGroupMnaggerComponent()->GetTeamHelper();

	TeamHelperChangedDelegateContainer.ExcuteCallback();
}

const TSharedPtr<FGroupMatesHelper>& UGroupMnaggerComponent::GetGroupHelper()
{
	if (!GroupHelperSPtr)
	{
		CreateGroup();
	}
	return GroupHelperSPtr;
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

TSharedPtr<FGroupMatesHelper> UGroupMnaggerComponent::CreateGroup()
{
	GroupHelperSPtr = MakeShared<FGroupMatesHelper>();
	GroupHelperSPtr->OwnerCharacterUnitPtr = GetOwner<FOwnerType>()->GetCharacterUnit();
	for (;;)
	{
		GroupHelperSPtr->ID = FMath::RandRange(1, std::numeric_limits<int32>::max());
		break;
	}

	GroupHelperChangedDelegateContainer.ExcuteCallback();

	return GroupHelperSPtr;
}

TSharedPtr<FTeamMatesHelper> UGroupMnaggerComponent::CreateTeam()
{
	TeamHelperSPtr = MakeShared<FTeamMatesHelper>();
	TeamHelperSPtr->OwnerCharacterUnitPtr = GetOwner<FOwnerType>()->GetCharacterUnit();
	for (;;)
	{
		TeamHelperSPtr->ID = FMath::RandRange(1, std::numeric_limits<int32>::max());
		break;
	}

	TeamHelperChangedDelegateContainer.ExcuteCallback();

	return TeamHelperSPtr;
}

void FGroupMatesHelper::AddCharacter(FPawnType* PCPtr)
{
	auto CharacterUnitPtr = PCPtr->GetCharacterUnit();
	AddCharacter(CharacterUnitPtr);
}

void FGroupMatesHelper::AddCharacter(const TSharedPtr<FCharacterUnitType>& CharacterUnitPtr)
{
	MembersSet.Add(CharacterUnitPtr);

	CharacterUnitPtr->ProxyCharacterPtr->GetGroupMnaggerComponent()->OnAddToNewGroup(OwnerCharacterUnitPtr);

	MembersChanged.ExcuteCallback(EGroupMateChangeType::kAdd, CharacterUnitPtr);
}

bool FGroupMatesHelper::IsMember(const TSharedPtr<FCharacterUnitType>& CharacterUnitPtr) const
{
	for (auto Iter : MembersSet)
	{
		if (Iter == CharacterUnitPtr)
		{
			return true;
		}
	}

	if (CharacterUnitPtr == OwnerCharacterUnitPtr)
	{
		return true;
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
