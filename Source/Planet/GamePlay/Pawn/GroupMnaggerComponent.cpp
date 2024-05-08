
#include "GroupMnaggerComponent.h"

#include "AIController.h"
#include "Kismet/GameplayStatics.h"

#include "GroupsManaggerSubSystem.h"
#include "CharacterBase.h"
#include "HumanCharacter.h"
#include "AIHumanInfo.h"
#include "PlanetAIController.h"
#include "PlanetControllerInterface.h"

FName UGroupMnaggerComponent::ComponentName = TEXT("GroupMnaggerComponent");

void UGroupMnaggerComponent::AddCharacterToGroup(AHumanCharacter* CharacterPtr)
{
	GetGroupsHelper()->AddCharacter(CharacterPtr);
}

void UGroupMnaggerComponent::OnAddToNewGroup(AHumanCharacter* GroupOwnerCharacterPtr)
{
	GroupsHelperSPtr = GroupOwnerCharacterPtr->GetController<IPlanetControllerInterface>()->GetGroupMnaggerComponent()->GetGroupsHelper();
}

const TSharedPtr<UGroupsManaggerSubSystem::FGroupsHelper>& UGroupMnaggerComponent::GetGroupsHelper() 
{
	if (!GroupsHelperSPtr)
	{
		auto CharacterPtr = Cast<AHumanCharacter>(GetOwner<AController>()->GetPawn());
		GroupsHelperSPtr = UGroupsManaggerSubSystem::GetInstance()->CreateGroup(CharacterPtr);
	}
	return GroupsHelperSPtr;
}

void UGroupMnaggerComponent::BeginPlay()
{
	Super::BeginPlay();
}
