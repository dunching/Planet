#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"

#include "GroupsManaggerSubSystem.h"

#include "GroupMnaggerComponent.generated.h"

class IPlanetControllerInterface;

UCLASS(BlueprintType, Blueprintable)
class UGroupMnaggerComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	static FName ComponentName;

	void AddCharacterToGroup(IPlanetControllerInterface* PCPtr);

	void AddCharacterToTeam(IPlanetControllerInterface* PCPtr);

	void OnAddToNewGroup(IPlanetControllerInterface* OwnerPCPtr);

	const TSharedPtr<UGroupsManaggerSubSystem::FGroupMatesHelper>& GetGroupsHelper();

	const TSharedPtr<UGroupsManaggerSubSystem::FTeamMatesHelper>& GetTeamsHelper();

protected:

	virtual void BeginPlay()override;

private:

	TSharedPtr<UGroupsManaggerSubSystem::FGroupMatesHelper> GroupsHelperSPtr;

	TSharedPtr<UGroupsManaggerSubSystem::FTeamMatesHelper> TeamsHelperSPtr;

};