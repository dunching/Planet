#include "STE_Assistance.h"

#include <NavigationSystem.h>
#include <Perception/AIPerceptionComponent.h>
#include <Components/SphereComponent.h>
#include <Components/SplineComponent.h>

#include "AIComponent.h"
#include "HumanCharacter.h"
#include "TeamMatesHelperComponentBase.h"
#include "HumanAIController.h"
#include "PlanetPlayerController.h"
#include "LogWriter.h"
#include "BuildingArea.h"
#include "CharacterAbilitySystemComponent.h"
#include "GameOptions.h"
#include "GameplayTagsLibrary.h"
#include "GeneratorColony_ByInvoke.h"
#include "GeneratorColony_Patrol.h"
#include "GroupManagger.h"
#include "GroupManagger_NPC.h"
#include "GroupManagger_Player.h"
#include "HumanCharacter_AI.h"
#include "HumanCharacter_Player.h"
#include "ItemProxy_Character.h"
#include "STE_CharacterBase.h"
#include "TeamMatesHelperComponent.h"

void USTE_Assistance::TreeStart(
	FStateTreeExecutionContext& Context
	)
{
	Super::TreeStart(Context);

	auto GroupManagger = HumanCharacterPtr->GetGroupManagger();
	if (GroupManagger)
	{
		// 如果是由Playher生成的NPC群体 则是这个类型
		if (GroupManagger->IsA(AGroupManagger_Player::StaticClass()))
		{
		}
		// 如果是NPC群体 则是这个类型
		else if (GroupManagger->IsA(AGroupManagger_NPC::StaticClass()))
		{
			GeneratorNPCs_PatrolPtr = HumanCharacterPtr->GetGroupManagger_NPC()->GeneratorNPCs_PatrolPtr;
			
			BuildingAreaPtr = HumanCharacterPtr->GetGroupManagger_NPC()->BuildingAreaPtr;
			GloabVariable_Character->OriginalLocation = HumanCharacterPtr->GetActorLocation();
		}

		if (HumanAIControllerPtr)
		{
			HumanCharacterPtr = HumanAIControllerPtr->GetPawn<AHumanCharacter_AI>();
			if (HumanCharacterPtr)
			{
				TeammateChangedDelegate = HumanCharacterPtr->GetGroupManagger()->GetTeamMatesHelperComponent()->
				                                             TeamHelperChangedDelegateContainer.AddCallback(
					                                              std::bind(&ThisClass::OnTeamChanged, this)
					                                             );
				OnTeamChanged();
			}
		}
	}
}

void USTE_Assistance::TreeStop(
	FStateTreeExecutionContext& Context
	)
{
	if (TeammateOptionChangedDelegate)
	{
		TeammateOptionChangedDelegate.Reset();
	}

	if (TeammateChangedDelegate)
	{
		TeammateChangedDelegate.Reset();
	}

	Super::TreeStop(Context);
}

void USTE_Assistance::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime
	)
{
	Super::Tick(Context, DeltaTime);
}

void USTE_Assistance::OnTeamOptionChanged(
	ETeammateOption NewTeammateOption
	)
{
#if WITH_EDITOR
	// TODO. 满足测试
	auto DefaultTeammateOption = HumanCharacterPtr->GetAIComponent()->DefaultTeammateOption;
	if (DefaultTeammateOption == ETeammateOption::kTest)
	{
		NewTeammateOption = DefaultTeammateOption;
	}
#endif

	TeammateOption = NewTeammateOption;

	switch (TeammateOption)
	{
	case ETeammateOption::kEnemy:
		{
		}
		break;
	case ETeammateOption::kFollow:
		{
		}
		break;
	case ETeammateOption::kAssistance:
		{
		}
		break;
	default: ;
	}
}

void USTE_Assistance::OnTeamChanged()
{
	auto TeamHelperSPtr = HumanCharacterPtr->GetGroupManagger()->GetTeamMatesHelperComponent();
	if (TeamHelperSPtr)
	{
		if (auto OwnerCharacterProxySPtr = TeamHelperSPtr->GetOwnerCharacterProxy())
		{
			LeaderCharacterPtr = OwnerCharacterProxySPtr->GetCharacterActor().Get();
		}

		TeammateOptionChangedDelegate = TeamHelperSPtr->TeammateOptionChanged.AddCallback(
			 std::bind(&ThisClass::OnTeamOptionChanged, this, std::placeholders::_1)
			);
		OnTeamOptionChanged(TeamHelperSPtr->GetTeammateOption());
	}
}

UGloabVariable_Character* USTE_Assistance::CreateGloabVarianble()
{
	auto GloabVariablePtr = NewObject<FGloabVariable>();

	GloabVariablePtr->UpdateTargetCharacterFunc = std::bind(&ThisClass::UpdateTargetCharacter, this);

	return GloabVariablePtr;
}

TWeakObjectPtr<ACharacterBase> USTE_Assistance::UpdateTargetCharacter()
{
	auto TeamMatesHelperComponent = HumanAIControllerPtr->GetGroupManagger()->GetTeamMatesHelperComponent();
	auto OwnerCharacterProxyPtr = TeamMatesHelperComponent->GetOwnerCharacterProxy();

	// 是否远离了设定位置
	auto CharacterActorPtr = OwnerCharacterProxyPtr->GetCharacterActor();
	if (CharacterActorPtr.IsValid())
	{
		if (CharacterActorPtr->IsA(AHumanCharacter_AI::StaticClass()))
		{
			auto KnowCharatersSet = TeamMatesHelperComponent->GetSensingChractersSet();

			if (GeneratorNPCs_PatrolPtr)
			{
				const auto Location = GeneratorNPCs_PatrolPtr->GetActorLocation();
				const auto MaxDistance = GeneratorNPCs_PatrolPtr->MaxDistance;

				return GetTargetCharacter(Location, MaxDistance,KnowCharatersSet);
			}
			
			if (BuildingAreaPtr)
			{
				const auto Location = BuildingAreaPtr->GetActorLocation();
				const auto MaxDistance = BuildingAreaPtr->MaxDistance;

				return GetTargetCharacter(Location, MaxDistance,KnowCharatersSet);
			}
		}
		else if (CharacterActorPtr->IsA(AHumanCharacter_Player::StaticClass()))
		{
			TSet<TWeakObjectPtr<ACharacterBase>> KnowCharatersSet;

			switch (TeammateOption)
			{
			case ETeammateOption::kFollow:
				break;
			case ETeammateOption::kAssistance:
				{
					KnowCharatersSet = TeamMatesHelperComponent->GetSensingChractersSet();
				}
				break;
			case ETeammateOption::kFireTarget:
				{
					KnowCharatersSet.Add(TeamMatesHelperComponent->GetForceKnowCharater());
				}
				break;
			}

			const auto Location = HumanCharacterPtr->GetActorLocation();
			const auto MaxDistance = UGameOptions::GetInstance()->NPCTeammateMaxActtackDistance;
			
			return GetTargetCharacter(Location, MaxDistance,KnowCharatersSet);
		}
	}

	return nullptr;
}

TWeakObjectPtr<ACharacterBase> USTE_Assistance::GetTargetCharacter(
	const FVector& Location,
	const int32& MaxDistance,
	TSet<TWeakObjectPtr<ACharacterBase>>& KnowCharatersSet
	) const
{
	TSet<ACharacterBase*> NeedRemoveAry;
	for (const auto& Iter : KnowCharatersSet)
	{
		if (Iter.IsValid())
		{
			// 超出距离
			if (FVector::Distance(Iter->GetActorLocation(), Location) > MaxDistance)
			{
				NeedRemoveAry.Add(Iter.Get());
				continue;
			}

			// 角色死亡了
			if (Iter->GetCharacterAbilitySystemComponent()->IsInDeath())
			{
				NeedRemoveAry.Add(Iter.Get());
				continue;
			}

			// 角色是功能性的NPC
			if (Iter->GetCharacterProxy()->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Character_NPC_Functional))
			{
				NeedRemoveAry.Add(Iter.Get());
				continue;
			}
		}
		else
		{
			// 无效了，比如被Destroy
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
			
	return GetNewTargetCharacter(KnowCharatersSet);
}
