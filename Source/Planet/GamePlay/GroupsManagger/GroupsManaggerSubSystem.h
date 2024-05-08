// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <atomic>

#include "CoreMinimal.h"

#include "GenerateType.h"

#include "GroupsManaggerSubSystem.generated.h"

class AHumanCharacter;
class UGourpMateUnit;

UCLASS()
class PLANET_API UGroupsManaggerSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	using FCallbackHandleContainerVoid = TCallbackHandleContainer<void(EGroupMateChangeType, ACharacterBase*)>;

	struct FGroupsHelper
	{
		void AddCharacter(AHumanCharacter* CharacterPtr);

		int32 ID = 1;

		FCallbackHandleContainerVoid GroupsChanged;

		AHumanCharacter* OwnerCharacterPtr = nullptr;

		TSet<AHumanCharacter*> CharactersSet;
	};

	static UGroupsManaggerSubSystem* GetInstance();

	TSharedPtr<FGroupsHelper> CreateGroup(AHumanCharacter* CharacterPtr);

private:

	TSet<TSharedPtr<FGroupsHelper>>GroupsMap;

};