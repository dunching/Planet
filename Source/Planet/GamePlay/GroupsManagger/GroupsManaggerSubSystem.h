// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <atomic>

#include "CoreMinimal.h"

#include "GenerateType.h"

#include "GroupsManaggerSubSystem.generated.h"

class IPlanetControllerInterface;
class UGourpmateUnit;

UCLASS()
class PLANET_API UGroupsManaggerSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	using FCallbackHandleContainer = TCallbackHandleContainer<void(EGroupMateChangeType, IPlanetControllerInterface*)>;

	struct FGroupMatesHelper
	{
		void AddCharacter(IPlanetControllerInterface* PCPtr);

		int32 ID = 1;

		FCallbackHandleContainer MembersChanged;

		IPlanetControllerInterface* OwnerPCPtr = nullptr;

		TSet<IPlanetControllerInterface*> MembersSet;
	};

	struct FTeamMatesHelper
	{
		void AddCharacter(UGourpmateUnit* GourpMateUnitPtr, IPlanetControllerInterface* PCPtr);

		int32 ID = 1;

		FCallbackHandleContainer MembersChanged;

		IPlanetControllerInterface* OwnerPCPtr = nullptr;

		TMap<UGourpmateUnit*, IPlanetControllerInterface*> MembersMap;
	};

	static UGroupsManaggerSubSystem* GetInstance();

	TSharedPtr<FGroupMatesHelper> CreateGroup(IPlanetControllerInterface* PCPtr);

	TSharedPtr<FTeamMatesHelper> CreateTeam(IPlanetControllerInterface* PCPtr);

private:

	TSet<TSharedPtr<FGroupMatesHelper>>GroupMatesMap;

	TSet<TSharedPtr<FTeamMatesHelper>>TeamMatesMap;

};