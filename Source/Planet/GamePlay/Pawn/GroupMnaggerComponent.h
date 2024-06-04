#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"

#include "GroupsManaggerSubSystem.h"

#include "GroupMnaggerComponent.generated.h"

class AHumanCharacter;

UCLASS(BlueprintType, Blueprintable)
class UGroupMnaggerComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	using FPawnType = AHumanCharacter;

	using FTeamHelperChangedDelegateContainer = TCallbackHandleContainer<void()>;

	static FName ComponentName;

	void AddCharacterToGroup(FPawnType* PCPtr);

	void AddCharacterToTeam(FPawnType* PCPtr);

	void OnAddToNewGroup(FPawnType* OwnerPtr);

	void OnAddToNewTeam(FPawnType* OwnerPtr);

	const TSharedPtr<UGroupsManaggerSubSystem::FGroupMatesHelper>& GetGroupHelper();

	const TSharedPtr<UGroupsManaggerSubSystem::FTeamMatesHelper>& GetTeamHelper();

	FTeamHelperChangedDelegateContainer TeamHelperChangedDelegateContainer;

	FTeamHelperChangedDelegateContainer GroupHelperChangedDelegateContainer;

protected:

	virtual void BeginPlay()override;

private:

	TSharedPtr<UGroupsManaggerSubSystem::FGroupMatesHelper> GroupHelperSPtr;

	TSharedPtr<UGroupsManaggerSubSystem::FTeamMatesHelper> TeamHelperSPtr;

};