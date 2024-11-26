
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
#include "GameplayTagsSubSystem.h"
#include "SceneUnitTable.h"
#include "GeneratorNPCs_Patrol.h"
#include "GeneratorColony.h"
#include "SceneUnitExtendInfo.h"
#include "CharactersInfo.h"

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
			if (TableRowUnit_CharacterInfoPtr)
			{
				auto HICPtr = GetHoldingItemsComponent();
				// 武器
				{
					auto EICPtr = GetProxyProcessComponent();
					{
						if (TableRowUnit_CharacterInfoPtr->FirstWeaponSocketInfo.IsValid())
						{
							auto WeaponProxyPtr = HICPtr->AddUnit_Weapon(TableRowUnit_CharacterInfoPtr->FirstWeaponSocketInfo);
							if (WeaponProxyPtr)
							{
								auto WeaponSocketSPtr = MakeShared<FSocket_FASI>();
								WeaponSocketSPtr->Socket = UGameplayTagsSubSystem::WeaponSocket_1;
								WeaponSocketSPtr->ProxySPtr = WeaponProxyPtr;
								WeaponSocketSPtr->ProxySPtr->SetAllocationCharacterUnit(GetCharacterUnit());
								EICPtr->UpdateSocket(WeaponSocketSPtr);

								auto WeaponSkillSocketSPtr = MakeShared<FSocket_FASI>();
								WeaponSkillSocketSPtr->Socket = UGameplayTagsSubSystem::WeaponActiveSocket_1;
								WeaponSkillSocketSPtr->ProxySPtr = WeaponProxyPtr->GetWeaponSkill();
								WeaponSkillSocketSPtr->ProxySPtr->SetAllocationCharacterUnit(GetCharacterUnit());
								EICPtr->UpdateSocket(WeaponSkillSocketSPtr);
							}
						}
					}
					{
						if (TableRowUnit_CharacterInfoPtr->SecondWeaponSocketInfo.IsValid())
						{
							auto WeaponProxyPtr = HICPtr->AddUnit_Weapon(TableRowUnit_CharacterInfoPtr->SecondWeaponSocketInfo);
							if (WeaponProxyPtr)
							{
								auto WeaponSocketSPtr = MakeShared<FSocket_FASI>();
								WeaponSocketSPtr->Socket = UGameplayTagsSubSystem::WeaponSocket_2;
								WeaponSocketSPtr->ProxySPtr = WeaponProxyPtr;
								WeaponSocketSPtr->ProxySPtr->SetAllocationCharacterUnit(GetCharacterUnit());
								EICPtr->UpdateSocket(WeaponSocketSPtr);

								auto WeaponSkillSocketSPtr = MakeShared<FSocket_FASI>();
								WeaponSkillSocketSPtr->Socket = UGameplayTagsSubSystem::WeaponActiveSocket_2;
								WeaponSkillSocketSPtr->ProxySPtr = WeaponProxyPtr->GetWeaponSkill();
								WeaponSkillSocketSPtr->ProxySPtr->SetAllocationCharacterUnit(GetCharacterUnit());
								EICPtr->UpdateSocket(WeaponSkillSocketSPtr);
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
							auto SkillsSocketInfo = MakeShared<FSocket_FASI>();

							SkillsSocketInfo->Socket = UGameplayTagsSubSystem::ActiveSocket_1;
							SkillsSocketInfo->ProxySPtr = SkillUnitPtr;
							SkillsSocketInfo->ProxySPtr->SetAllocationCharacterUnit(GetCharacterUnit());

							EICPtr->UpdateSocket(SkillsSocketInfo);
						}
					}
					if (TableRowUnit_CharacterInfoPtr->ActiveSkillSet_2.IsValid())
					{
						auto SkillUnitPtr = HICPtr->AddUnit_Skill(TableRowUnit_CharacterInfoPtr->ActiveSkillSet_2);
						if (SkillUnitPtr)
						{
							auto SkillsSocketInfo = MakeShared<FSocket_FASI>();

							SkillsSocketInfo->Socket = UGameplayTagsSubSystem::ActiveSocket_2;
							SkillsSocketInfo->ProxySPtr = SkillUnitPtr;
							SkillsSocketInfo->ProxySPtr->SetAllocationCharacterUnit(GetCharacterUnit());

							EICPtr->UpdateSocket(SkillsSocketInfo);
						}
					}

					EICPtr->UpdateCanbeActiveSkills();
				}
			}
		}
	}
#endif
}

