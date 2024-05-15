// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <atomic>

#include "CoreMinimal.h"

#include "GenerateType.h"

#include "GroupsManaggerSubSystem.generated.h"

class IPlanetControllerInterface;
class UGourpmateUnit;
class AHumanCharacter;

UCLASS()
class PLANET_API UGroupsManaggerSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	using FPawnType = AHumanCharacter;

	using FMemberChangedDelegateContainer = TCallbackHandleContainer<void(EGroupMateChangeType, FPawnType*)>;

	using FTeammateOptionChangedDelegateContainer = TCallbackHandleContainer<void(ETeammateOption, FPawnType*)>;

	struct FGroupMatesHelper
	{
		void AddCharacter(FPawnType* PCPtr);

		int32 ID = 1;

		FMemberChangedDelegateContainer MembersChanged;

		FPawnType* OwnerPCPtr = nullptr;

		TSet<FPawnType*> MembersSet;
	};

	struct FTeamMatesHelper
	{
		void AddCharacter(UGourpmateUnit* GourpMateUnitPtr, FPawnType* PCPtr);

		void SwitchTeammateOption(ETeammateOption InTeammateOption);

		ETeammateOption GetTeammateOption()const;

		int32 ID = 1;

		FMemberChangedDelegateContainer MembersChanged;

		FTeammateOptionChangedDelegateContainer TeammateOptionChanged;

		FPawnType* OwnerPCPtr = nullptr;

		TMap<UGourpmateUnit*, FPawnType*> MembersMap;

	private:

		ETeammateOption TeammateOption = ETeammateOption::kFollow;

	};

	static UGroupsManaggerSubSystem* GetInstance();

	TSharedPtr<FGroupMatesHelper> CreateGroup(FPawnType* PCPtr);

	TSharedPtr<FTeamMatesHelper> CreateTeam(FPawnType* PCPtr);

private:

	TSet<TSharedPtr<FGroupMatesHelper>>GroupMatesMap;

	TSet<TSharedPtr<FTeamMatesHelper>>TeamMatesMap;

};