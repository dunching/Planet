#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"

#include "GroupsManaggerSubSystem.h"

#include "GroupMnaggerComponent.generated.h"

class AHumanCharacter;
class IPlanetControllerInterface;

UCLASS(BlueprintType, Blueprintable)
class PLANET_API UGroupMnaggerComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	using FOwnerType = IPlanetControllerInterface;
	
	using FPawnType = ACharacterBase;

	using FTeamHelperChangedDelegateContainer = TCallbackHandleContainer<void()>;

	static FName ComponentName;

	void AddCharacterToGroup(FPawnType* TargetCharaterPtr);

	void AddCharacterToTeam(FPawnType* TargetCharaterPtr);

	void OnAddToNewGroup(FPawnType* TargetCharaterPtr);

	void OnAddToNewTeam(FPawnType* TargetCharaterPtr);

	const TSharedPtr<FGroupMatesHelper>& GetGroupHelper();

	const TSharedPtr<FTeamMatesHelper>& GetTeamHelper();

	FTeamHelperChangedDelegateContainer TeamHelperChangedDelegateContainer;

	FTeamHelperChangedDelegateContainer GroupHelperChangedDelegateContainer;

	TSet<AHumanCharacter*>TargetSet;

protected:

	virtual void BeginPlay()override;

private:

	TSharedPtr<FGroupMatesHelper> GroupHelperSPtr;

	TSharedPtr<FTeamMatesHelper> TeamHelperSPtr;

};