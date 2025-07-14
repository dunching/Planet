#include "TeamMatesHelperComponentBase.h"

#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

#include "LogWriter.h"
#include "TeamConfigure.h"

#ifdef WITH_EDITOR
static TAutoConsoleVariable<int32> GroupMnaggerComponent_KnowCharaterChanged(
                                                                             TEXT(
	                                                                              "GroupMnaggerComponent.KnowCharaterChanged"
	                                                                             ),
                                                                             1,
                                                                             TEXT("")
                                                                             TEXT(" default: 0")
                                                                            );
#endif

UTeamMatesHelperComponentBase::UTeamMatesHelperComponentBase(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UTeamMatesHelperComponentBase::InitializeComponent()
{
	Super::InitializeComponent();
}

void UTeamMatesHelperComponentBase::BeginPlay()
{
	Super::BeginPlay();
}

void UTeamMatesHelperComponentBase::SpwanTeammateCharacter()
{
}

bool UTeamMatesHelperComponentBase::IsMember(
	const FGuid& CharacterID
	) const
{
	for (auto Iter : MembersIDSet)
	{
		if (Iter == CharacterID)
		{
			return true;
		}
	}

	return false;
}

bool UTeamMatesHelperComponentBase::TeleportTo(
	const FVector& DestLocation,
	const FRotator& DestRotation,
	bool bIsATest,
	bool bNoCheck
	)
{
	return true;
}

void UTeamMatesHelperComponentBase::SwitchTeammateOption(
	ETeammateOption InTeammateOption
	)
{
	TeammateOption = InTeammateOption;
}

ETeammateOption UTeamMatesHelperComponentBase::GetTeammateOption() const
{
	return TeammateOption;
}

FName UTeamMatesHelperComponentBase::ComponentName = TEXT("TeamMatesHelperComponent");

void UTeamMatesHelperComponentBase::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
	) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, TeamConfigure, COND_None);
	DOREPLIFETIME_CONDITION(ThisClass, TeammateOption, COND_None);

	DOREPLIFETIME_CONDITION(ThisClass, MembersIDSet, COND_None);
}

void UTeamMatesHelperComponentBase::OnRep_MembersIDSet()
{
}

void UTeamMatesHelperComponentBase::OnRep_TeamConfigure()
{
	TeamHelperChangedDelegateContainer();
	PRINTINVOKEWITHSTR(FString(TEXT("")));
}

void UTeamMatesHelperComponentBase::OnRep_TeammateOptionChanged()
{
}
