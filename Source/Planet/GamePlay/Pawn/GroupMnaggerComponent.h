#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"

#include "GroupsManaggerSubSystem.h"

#include "GroupMnaggerComponent.generated.h"

class AHumanCharacter;
class IPlanetControllerInterface;

class PLANET_API FGroupMatesHelper
{
public:

	using FCharacterUnitType = UCharacterUnit;

	using FPawnType = ACharacterBase;

	using FMemberChangedDelegateContainer = TCallbackHandleContainer<void(EGroupMateChangeType, FCharacterUnitType*)>;

	void AddCharacter(FPawnType* PCPtr);

	void AddCharacter(FCharacterUnitType* CharacterUnitPtr);

	bool IsMember(FCharacterUnitType* CharacterUnitPtr)const;

	int32 ID = 1;

	FMemberChangedDelegateContainer MembersChanged;

	FCharacterUnitType* OwnerCharacterUnitPtr = nullptr;

	TSet<FCharacterUnitType*> MembersSet;

};

class PLANET_API FTeamMatesHelper : public FGroupMatesHelper
{
public:

	using FTeammateOptionChangedDelegateContainer = TCallbackHandleContainer<void(ETeammateOption, FCharacterUnitType*)>;

	void SwitchTeammateOption(ETeammateOption InTeammateOption);

	ETeammateOption GetTeammateOption()const;

	FTeammateOptionChangedDelegateContainer TeammateOptionChanged;

private:

	ETeammateOption TeammateOption = ETeammateOption::kEnemy;

};

UCLASS(BlueprintType, Blueprintable)
class PLANET_API UGroupMnaggerComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	using FCharacterUnitType = UCharacterUnit;

	using FOwnerType = IPlanetControllerInterface;

	using FPawnType = ACharacterBase;

	using FTeamHelperChangedDelegateContainer = TCallbackHandleContainer<void()>;

	static FName ComponentName;

	void AddCharacterToGroup(FCharacterUnitType* CharacterUnitPtr);

	void AddCharacterToTeam(FCharacterUnitType* CharacterUnitPtr);

	TSharedPtr<FGroupMatesHelper> CreateGroup();

	TSharedPtr<FTeamMatesHelper> CreateTeam();

	void OnAddToNewGroup(FCharacterUnitType* CharacterUnitPtr);

	void OnAddToNewTeam(FCharacterUnitType* CharacterUnitPtr);

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