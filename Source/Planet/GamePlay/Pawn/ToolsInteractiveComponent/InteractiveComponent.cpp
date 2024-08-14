
#include "InteractiveComponent.h"

#include <queue>
#include <map>

#include "GameplayAbilitySpec.h"

#include "GAEvent_Helper.h"
#include "CharacterBase.h"
#include "CharacterAttributesComponent.h"
#include "GenerateType.h"
#include "PlanetPlayerState.h"
#include "BasicFuturesBase.h"
#include "GAEvent_Send.h"
#include "GAEvent_Received.h"
#include "SceneElement.h"
#include "Weapon_Base.h"
#include "Skill_Base.h"
#include "Skill_Active_Base.h"
#include "Skill_WeaponActive_PickAxe.h"
#include "Skill_WeaponActive_HandProtection.h"
#include "Skill_WeaponActive_RangeTest.h"
#include "Weapon_HandProtection.h"
#include "Weapon_PickAxe.h"
#include "Weapon_RangeTest.h"
#include "AssetRefMap.h"
#include "Skill_Talent_NuQi.h"
#include "Skill_Talent_YinYang.h"
#include "Skill_Element_Gold.h"
#include "InputProcessorSubSystem.h"
#include "Tool_PickAxe.h"
#include "HumanRegularProcessor.h"
#include "HumanCharacter.h"

FName UInteractiveComponent::ComponentName = TEXT("InteractiveComponent");

UInteractiveComponent::UInteractiveComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 1.f / 10;
}

void UInteractiveComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInteractiveComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UInteractiveComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

TArray<TSharedPtr<FCanbeInteractionInfo>> UInteractiveComponent::GetCanbeActiveAction() const
{
	return CanbeInteractionAry;
}

bool UInteractiveComponent::ActiveAction(
	const TSharedPtr<FCanbeInteractionInfo>& CanbeActivedInfoSPtr, bool bIsAutomaticStop
)
{
	return false;
}

void UInteractiveComponent::CancelAction(const TSharedPtr<FCanbeInteractionInfo>& CanbeActivedInfoSPtr)
{

}

void UInteractiveComponent::GenerationCanbeActiveEvent()
{

}
