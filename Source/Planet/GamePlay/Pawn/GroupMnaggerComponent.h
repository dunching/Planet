#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"

#include "GroupsManaggerSubSystem.h"

#include "GroupMnaggerComponent.generated.h"

class AHumanCharacter;

UCLASS(BlueprintType, Blueprintable)
class PLANET_API UGroupMnaggerComponent : public UActorComponent
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