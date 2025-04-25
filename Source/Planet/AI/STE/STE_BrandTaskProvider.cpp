
#include "STE_BrandTaskProvider.h"

#include <NavigationSystem.h>
#include <Perception/AIPerceptionComponent.h>
#include <Components/SphereComponent.h>
#include <Components/SplineComponent.h>

#include "AIComponent.h"
#include "HumanCharacter.h"
#include "TeamMatesHelperComponent.h"
#include "HumanAIController.h"
#include "PlanetPlayerController.h"
#include "LogWriter.h"
#include "BuildingArea.h"
#include "CharacterAbilitySystemComponent.h"
#include "GeneratorColony_ByInvoke.h"
#include "GroupManagger.h"
#include "HumanCharacter_AI.h"
#include "ItemProxy_Character.h"
#include "STE_CharacterBase.h"

void USTE_BrandTaskProvider::TreeStart(FStateTreeExecutionContext& Context)
{
	Super::TreeStart(Context);

	GetWorld()->GetTimerManager().SetTimer(
		CheckKnowCharacterTimerHandle,
		this,
		&ThisClass::CheckKnowCharacterImp,
		1.f,
		true
	);
}

void USTE_BrandTaskProvider::CheckKnowCharacterImp()
{
	auto TeamMatesHelperComponent= HumanAIControllerPtr->GetGroupSharedInfo()->GetTeamMatesHelperComponent();
	auto KnowCharatersSet = TeamMatesHelperComponent->GetSensingChractersSet();
	auto OwnerCharacterProxyPtr = TeamMatesHelperComponent->GetOwnerCharacterProxyPtr();
}
