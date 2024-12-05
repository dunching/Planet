#include "HumanCharacter_AI.h"

#include <GameFramework/CharacterMovementComponent.h>
#include "Components/StateTreeComponent.h"
#include "Components/StateTreeAIComponent.h"
#include <Perception/AIPerceptionComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Perception/AISenseConfig_Sight.h>

#include "GeneratorNPCs_Patrol.h"
#include "AIComponent.h"
#include "CharacterTitle.h"
#include "CharacterBase.h"
#include "AssetRefMap.h"
#include "Planet.h"
#include "GroupMnaggerComponent.h"
#include "ItemProxy.h"
#include "HumanCharacter.h"
#include "HoldingItemsComponent.h"
#include "PlanetPlayerController.h"
#include "TestCommand.h"
#include "GameplayTagsLibrary.h"
#include "SceneUnitTable.h"
#include "GeneratorNPCs_Patrol.h"
#include "GeneratorColony.h"
#include "SceneUnitExtendInfo.h"
#include "CharactersInfo.h"
#include "HumanAIController.h"

AHumanCharacter_AI::AHumanCharacter_AI(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	AIComponentPtr = CreateDefaultSubobject<UAIComponent>(TEXT("AIComponent"));
}

void AHumanCharacter_AI::BeginPlay()
{
	Super::BeginPlay();

	InitialAllocations();
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

void AHumanCharacter_AI::InitialAllocations()
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
				auto HICPtr = GetHoldingItemsComponent();
				// 武器
				{
					auto EICPtr = GetProxyProcessComponent();
					{
						if (TableRowUnit_CharacterInfoPtr->FirstWeaponSocketInfo.IsValid())
						{
							auto WeaponProxyPtr = HICPtr->AddUnit_Weapon(
								TableRowUnit_CharacterInfoPtr->FirstWeaponSocketInfo);
							if (WeaponProxyPtr)
							{
								FMySocket_FASI SkillsSocketInfo;
								SkillsSocketInfo.Socket = UGameplayTagsLibrary::ActiveSocket_1;
								SkillsSocketInfo.UpdateProxy(
									HoldingItemsComponentPtr->AddUnit_Weapon(
										TableRowUnit_CharacterInfoPtr->FirstWeaponSocketInfo));;

								HoldingItemsComponentPtr->UpdateSocket(GetCharacterProxy(), SkillsSocketInfo);
							}
						}
					}
					EICPtr->ActiveWeapon();
				}

				// 技能
				{
					auto EICPtr = GetProxyProcessComponent();

					if (TableRowUnit_CharacterInfoPtr->ActiveSkillSet_1.IsValid())
					{
						auto SkillUnitPtr = HICPtr->AddUnit_Skill(TableRowUnit_CharacterInfoPtr->ActiveSkillSet_1);
						if (SkillUnitPtr)
						{
							FMySocket_FASI SkillsSocketInfo;

							SkillsSocketInfo.Socket = UGameplayTagsLibrary::ActiveSocket_1;

							HoldingItemsComponentPtr->UpdateSocket(GetCharacterProxy(), SkillsSocketInfo);
						}
					}

					EICPtr->UpdateCanbeActiveSkills();
				}
			}
		}
	}
#endif
}
