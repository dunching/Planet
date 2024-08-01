// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <atomic>

#include "CoreMinimal.h"

#include "GenerateType.h"

#include "GroupsManaggerSubSystem.generated.h"

class IPlanetControllerInterface;
class UCharacterUnit;
class AHumanCharacter;
class ACharacterBase;

PLANET_API class FGroupMatesHelper
{
public:
	using FPawnType = ACharacterBase;

	using FMemberChangedDelegateContainer = TCallbackHandleContainer<void(EGroupMateChangeType, FPawnType*)>;

	using FTeammateOptionChangedDelegateContainer = TCallbackHandleContainer<void(ETeammateOption, FPawnType*)>;

	void AddCharacter(FPawnType* PCPtr);

	int32 ID = 1;

	FMemberChangedDelegateContainer MembersChanged;

	FPawnType* OwnerPtr = nullptr;

	TSet<FPawnType*> MembersSet;
};

PLANET_API class FTeamMatesHelper
{
public:
	using FPawnType = ACharacterBase;

	using FMemberChangedDelegateContainer = TCallbackHandleContainer<void(EGroupMateChangeType, FPawnType*)>;

	using FTeammateOptionChangedDelegateContainer = TCallbackHandleContainer<void(ETeammateOption, FPawnType*)>;

	void AddCharacter(UCharacterUnit* GourpMateUnitPtr, FPawnType* PCPtr);

	PLANET_API void SwitchTeammateOption(ETeammateOption InTeammateOption);

	ETeammateOption GetTeammateOption()const;

	int32 ID = 1;

	FMemberChangedDelegateContainer MembersChanged;

	FTeammateOptionChangedDelegateContainer TeammateOptionChanged;

	FPawnType* OwnerPtr = nullptr;

	TMap<UCharacterUnit*, FPawnType*> MembersMap;

private:

	ETeammateOption TeammateOption = ETeammateOption::kEnemy;

};

UCLASS()
class PLANET_API UGroupsManaggerSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	using FPawnType = ACharacterBase;

	using FMemberChangedDelegateContainer = TCallbackHandleContainer<void(EGroupMateChangeType, FPawnType*)>;

	using FTeammateOptionChangedDelegateContainer = TCallbackHandleContainer<void(ETeammateOption, FPawnType*)>;

	static UGroupsManaggerSubSystem* GetInstance();

	TSharedPtr<FGroupMatesHelper> CreateGroup(FPawnType* PCPtr);

	TSharedPtr<FTeamMatesHelper> CreateTeam(FPawnType* PCPtr);

private:

	TSet<TSharedPtr<FGroupMatesHelper>>GroupMatesMap;

	TSet<TSharedPtr<FTeamMatesHelper>>TeamMatesMap;

};