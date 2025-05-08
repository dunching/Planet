#include "TeamMatesHelperComponent.h"

#include "Net/UnrealNetwork.h"

#include "TeamMatesHelperComponent.h"

#include "AIComponent.h"
#include "AIController.h"
#include "CharacterAbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "CharacterBase.h"
#include "HumanCharacter.h"
#include "CharacterTitle.h"
#include "HumanAIController.h"
#include "PlanetControllerInterface.h"
#include "PlanetPlayerState.h"
#include "InventoryComponent.h"
#include "ItemProxy_Container.h"
#include "GameOptions.h"
#include "GeneratorColony_ByInvoke.h"
#include "SceneProxyTable.h"
#include "ItemProxy_Minimal.h"
#include "HumanCharacter_AI.h"
#include "LogWriter.h"
#include "TeamConfigure.h"
#include "GroupManagger.h"
#include "ItemProxy_Character.h"

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

UTeamMatesHelperComponent::UTeamMatesHelperComponent(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UTeamMatesHelperComponent::UpdateTeammateConfig(
	const TSharedPtr<FCharacterProxyType>& CharacterProxyPtr,
	int32 Index
	)
{
#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		UpdateTeammateConfigImp(CharacterProxyPtr, Index);
	}
#endif

#if UE_EDITOR || UE_CLIENT
	if (GetNetMode() == NM_Client)
	{
		UpdateTeammateConfigImp(CharacterProxyPtr, Index);
		if (CharacterProxyPtr)
		{
			UpdateTeammateConfig_Server(CharacterProxyPtr->GetID(), Index);
		}
		else
		{
			UpdateTeammateConfig_Server(FGuid(), Index);
		}
	}
#endif
}

void UTeamMatesHelperComponent::OnAddToNewTeam(
	const TSharedPtr<FCharacterProxyType>& CharacterProxyPtr
	)
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

#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		GetWorld()->GetTimerManager().SetTimer(
		                                       CheckKnowCharacterTimerHandle,
		                                       this,
		                                       &ThisClass::UpdateSensingCharacters,
		                                       1.f,
		                                       true
		                                      );
	}
#endif
}

void UTeamMatesHelperComponent::UpdateTeammateConfig_Server_Implementation(
	const FGuid& ProxtID,
	int32 Index
	)
{
	const auto CharacterProxySPtr =
		GetOwner<FOwnerType>()->GetInventoryComponent()->FindProxy_Character(ProxtID);

	UpdateTeammateConfig(CharacterProxySPtr, Index);
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
	const auto CharactersAry = TeamConfigure.GetCharactersAry();
	for (int32 Index = 0; Index < CharactersAry.Num(); Index++)
	{
		for (int32 SecondIndex = 0; SecondIndex < CharactersAry[Index].Num(); SecondIndex++)
		{
			const auto CharacterProxySPtr =
				GetOwner<FOwnerType>()->GetInventoryComponent()->FindProxy_Character(
					 CharactersAry[Index][SecondIndex]
					);
			if (CharacterProxySPtr)
			{
				Transform.SetLocation(PlayerCharacterLocation + (PlayerCharacterRotation.Vector() * 100));
				auto AICharacterPtr = CharacterProxySPtr->SpwanCharacter(Transform);
				if (AICharacterPtr)
				{
					// AICharacterPtr->SetGroupSharedInfo(OwnerPtr);
					// AICharacterPtr->SetCharacterID(CharacterProxySPtr->GetID());

					AICharacterPtr->GetProxyProcessComponent()->ActiveWeapon();
				}
			}
		}
	}
}

void UTeamMatesHelperComponent::SpwanTeammateCharacter()
{
	SpwanTeammateCharacter_Server();
}

void UTeamMatesHelperComponent::UpdateTeammateConfigImp(
	FPawnType* PCPtr,
	int32 Index
	)
{
	auto CharacterProxyPtr = PCPtr->GetCharacterProxy();
	UpdateTeammateConfigImp(CharacterProxyPtr, Index);
}

void UTeamMatesHelperComponent::UpdateTeammateConfigImp(
	const TSharedPtr<FCharacterProxyType>& CharacterProxyPtr,
	int32 Index
	)
{
	FTeammate Teammate;

	if (CharacterProxyPtr)
	{
		Teammate.CharacterProxyID = CharacterProxyPtr->GetID();

		MembersSet.Add(CharacterProxyPtr);
	}
	else
	{
	}

	Teammate.IndexInTheTeam = Index;
	TeamConfigure.UpdateTeammateConfig(Teammate);
	MembersChanged.ExcuteCallback(Teammate, CharacterProxyPtr);
}

bool UTeamMatesHelperComponent::IsMember(
	const TSharedPtr<FCharacterProxyType>& CharacterProxyPtr
	) const
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

bool UTeamMatesHelperComponent::IsMember(
	const FGuid& CharacterID
	) const
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

bool UTeamMatesHelperComponent::TeleportTo(
	const FVector& DestLocation,
	const FRotator& DestRotation,
	bool bIsATest,
	bool bNoCheck
	)
{
	for (auto Iter : MembersSet)
	{
		if (Iter == OwnerCharacterProxyPtr)
		{
			continue;
		}
		auto TargetActorPtr = Iter->GetCharacterActor();
		if (TargetActorPtr.IsValid())
		{
			TargetActorPtr->TeleportTo(DestLocation, DestRotation, bIsATest, bNoCheck);
		}
	}

	return true;
}

void UTeamMatesHelperComponent::SwitchTeammateOption(
	ETeammateOption InTeammateOption
	)
{
	TeammateOption = InTeammateOption;

	TeammateOptionChanged.ExcuteCallback(InTeammateOption, OwnerCharacterProxyPtr);

#if UE_EDITOR || UE_CLIENT
	if (GetNetMode() == NM_Client)
	{
		SwitchTeammateOption_Server(InTeammateOption);
	}
#endif
}

ETeammateOption UTeamMatesHelperComponent::GetTeammateOption() const
{
	return TeammateOption;
}

void UTeamMatesHelperComponent::AddKnowCharacter(
	ACharacterBase* CharacterPtr
	)
{
	if (CheckCharacterIsValid(CharacterPtr))
	{
		SensingChractersSet.Add(CharacterPtr);
	}
}

void UTeamMatesHelperComponent::RemoveKnowCharacter(
	ACharacterBase* CharacterPtr
	)
{
	// 不要通过离开视野移除目标，因为我们要通过其他判断
	// SensingChractersSet.Remove(CharacterPtr);
}

void UTeamMatesHelperComponent::SetFocusCharactersAry(
	ACharacterBase* TargetCharacterPtr
	)
{
	ForceKnowCharater = TargetCharacterPtr;
	OnFocusCharacterDelegate(TargetCharacterPtr);

	SensingChractersSet.Add(TargetCharacterPtr);
}

void UTeamMatesHelperComponent::ClearFocusCharactersAry()
{
	ForceKnowCharater = nullptr;
	OnFocusCharacterDelegate(nullptr);
}

TWeakObjectPtr<ACharacterBase> UTeamMatesHelperComponent::GetForceKnowCharater() const
{
	if (ForceKnowCharater.IsValid())
	{
		return ForceKnowCharater;
	}

	return nullptr;
}

TSet<TWeakObjectPtr<ACharacterBase>> UTeamMatesHelperComponent::GetSensingChractersSet() const
{
	return SensingChractersSet;
}

void UTeamMatesHelperComponent::SetSensingChractersSet(
	const TSet<TWeakObjectPtr<ACharacterBase>>& KnowCharater
	)
{
	SensingChractersSet = KnowCharater;
}

TSharedPtr<UTeamMatesHelperComponent::FCharacterProxyType> UTeamMatesHelperComponent::GetOwnerCharacterProxyPtr() const
{
	return OwnerCharacterProxyPtr;
}

void UTeamMatesHelperComponent::SetOwnerCharacterProxy(
	const TSharedPtr<FCharacterProxyType>& CharacterProxySPtr
	)
{
	OwnerCharacterProxyPtr = CharacterProxySPtr;
	MembersSet.Add(OwnerCharacterProxyPtr);
}

FName UTeamMatesHelperComponent::ComponentName = TEXT("TeamMatesHelperComponent");

void UTeamMatesHelperComponent::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
	) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, TeamConfigure, COND_None);
	DOREPLIFETIME_CONDITION(ThisClass, TeammateOption, COND_None);

	DOREPLIFETIME_CONDITION(ThisClass, ForceKnowCharater, COND_None);
}

void UTeamMatesHelperComponent::TeammateCharacter_ActiveWeapon_Server_Implementation()
{
	const auto CharactersAry = TeamConfigure.GetCharactersAry();
	for (int32 Index = 0; Index < CharactersAry.Num(); Index++)
	{
		for (int32 SecondIndex = 0; SecondIndex < CharactersAry[Index].Num(); SecondIndex++)
		{
			const auto CharacterProxySPtr =
				GetOwner<FOwnerType>()->GetInventoryComponent()->FindProxy_Character(
					 CharactersAry[Index][SecondIndex]
					);
			if (CharacterProxySPtr)
			{
				auto CharacterActorPtr = CharacterProxySPtr->GetCharacterActor();
				if (CharacterActorPtr.IsValid())
				{
					CharacterActorPtr->GetProxyProcessComponent()->ActiveWeapon();
				}
			}
		}
	}
}

void UTeamMatesHelperComponent::SwitchTeammateOption_Server_Implementation(
	ETeammateOption InTeammateOption
	)
{
	SwitchTeammateOption(InTeammateOption);
}

void UTeamMatesHelperComponent::OnRep_GroupSharedInfoChanged()
{
	TeamHelperChangedDelegateContainer();
	PRINTINVOKEWITHSTR(FString(TEXT("")));
}

void UTeamMatesHelperComponent::OnRep_TeammateOptionChanged()
{
	TeammateOptionChanged.ExcuteCallback(TeammateOption, OwnerCharacterProxyPtr);
}

void UTeamMatesHelperComponent::UpdateSensingCharacters()
{
	auto KnowCharatersSet = GetSensingChractersSet();

	decltype(KnowCharatersSet) NewSensingChractersSet;
	decltype(KnowCharatersSet) NewValidCharater;

	// 是否有效
	{
		TSet<ACharacterBase*> NeedRemoveAry;
		for (const auto& Iter : KnowCharatersSet)
		{
			if (CheckCharacterIsValid(Iter))
			{
			}
			else
			{
				NeedRemoveAry.Add(Iter.Get());
			}
		}
		for (const auto& Iter : NeedRemoveAry)
		{
			if (KnowCharatersSet.Contains(Iter))
			{
				KnowCharatersSet.Remove(Iter);
			}
		}
	}

	NewSensingChractersSet = KnowCharatersSet;
	SetSensingChractersSet(NewSensingChractersSet);
}

bool UTeamMatesHelperComponent::CheckCharacterIsValid(
	const TWeakObjectPtr<ACharacterBase>& CharacterPtr
	)
{
	if (CharacterPtr.IsValid())
	{
		if (CharacterPtr->GetCharacterAbilitySystemComponent()->IsInDeath())
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	return false;
}
