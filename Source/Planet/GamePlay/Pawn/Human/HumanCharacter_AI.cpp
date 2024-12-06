#include "HumanCharacter_AI.h"

#include <Kismet/GameplayStatics.h>
#include "Net/UnrealNetwork.h"

#include "AIComponent.h"
#include "CharacterTitle.h"
#include "CharacterBase.h"
#include "ItemProxy_Minimal.h"
#include "HumanCharacter.h"
#include "HoldingItemsComponent.h"
#include "TestCommand.h"
#include "GameplayTagsLibrary.h"
#include "SceneUnitExtendInfo.h"
#include "CharactersInfo.h"
#include "HumanAIController.h"
#include "GroupSharedInfo.h"

AHumanCharacter_AI::AHumanCharacter_AI(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	AIComponentPtr = CreateDefaultSubobject<UAIComponent>(TEXT("AIComponent"));
}

void AHumanCharacter_AI::BeginPlay()
{
	Super::BeginPlay();
}

void AHumanCharacter_AI::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

void AHumanCharacter_AI::SetGroupSharedInfo(AGroupSharedInfo* InGroupSharedInfoPtr)
{
	GroupSharedInfoPtr = InGroupSharedInfoPtr;

	if (auto ControllerPtr = GetController<AHumanAIController>())
	{
		ControllerPtr->SetGroupSharedInfo(InGroupSharedInfoPtr);
	}
}

void AHumanCharacter_AI::SetCharacterID(const FGuid& InCharacterID)
{
	CharacterID = InCharacterID;

#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		InitialAllocationsByProxy();
	}
#endif
}

void AHumanCharacter_AI::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, CharacterID, COND_None);
}

void AHumanCharacter_AI::OnRep_GroupSharedInfoChanged()
{
	Super::OnRep_GroupSharedInfoChanged();
}

TSharedPtr<FCharacterProxy> AHumanCharacter_AI::GetCharacterProxy() const
{
	return GetGroupSharedInfo()->GetHoldingItemsComponent()->FindProxy_Character(CharacterID);
}

void AHumanCharacter_AI::OnRep_CharacterID()
{
#if UE_EDITOR || UE_CLIENT
	if (GetNetMode() == NM_Client)
	{
		auto PlayerCharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
		if (PlayerCharacterPtr)
		{
			const auto bIsMember = PlayerCharacterPtr->GetGroupSharedInfo()->GetTeamMatesHelperComponent()->IsMember(
				CharacterID);
			SetCampType(
				bIsMember ? ECharacterCampType::kTeamMate : ECharacterCampType::kEnemy
			);
		}
	}
#endif
}

void AHumanCharacter_AI::InitialAllocationsRowName()
{
#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
#if TESTAICHARACTERHOLDDATA
		TestCommand::AddAICharacterTestDataImp(this);
#endif
		{
			auto TableRowUnit_CharacterInfoPtr =
				USceneUnitExtendInfoMap::GetInstance()->GetTableRowUnit_AICharacter_Allocation(AI_Allocation_RowName);
			auto HoldingItemsComponentPtr = GetHoldingItemsComponent();
			if (TableRowUnit_CharacterInfoPtr)
			{
				// 武器
				{
					{
						if (TableRowUnit_CharacterInfoPtr->FirstWeaponSocketInfo.IsValid())
						{
							auto WeaponProxyPtr = HoldingItemsComponentPtr->AddUnit_Weapon(
								TableRowUnit_CharacterInfoPtr->FirstWeaponSocketInfo);
							if (WeaponProxyPtr)
							{
								FCharacterSocket SkillsSocketInfo;
								SkillsSocketInfo.Socket = UGameplayTagsLibrary::ActiveSocket_1;
								SkillsSocketInfo.UpdateProxy(
									HoldingItemsComponentPtr->AddUnit_Weapon(
										TableRowUnit_CharacterInfoPtr->FirstWeaponSocketInfo));;

								HoldingItemsComponentPtr->UpdateSocket(GetCharacterProxy(), SkillsSocketInfo);
							}
						}
					}
					GetProxyProcessComponent()->ActiveWeapon();
				}

				// 技能
				{
					if (TableRowUnit_CharacterInfoPtr->ActiveSkillSet_1.IsValid())
					{
						auto SkillUnitPtr = HoldingItemsComponentPtr->AddUnit_Skill(
							TableRowUnit_CharacterInfoPtr->ActiveSkillSet_1);
						if (SkillUnitPtr)
						{
							FCharacterSocket SkillsSocketInfo;

							SkillsSocketInfo.Socket = UGameplayTagsLibrary::ActiveSocket_1;

							HoldingItemsComponentPtr->UpdateSocket(GetCharacterProxy(), SkillsSocketInfo);
						}
					}

					GetProxyProcessComponent()->UpdateCanbeActiveSkills();
				}
			}
		}
	}
#endif
}

void AHumanCharacter_AI::InitialAllocationsByProxy()
{
	GetProxyProcessComponent()->ActiveWeapon();
}
