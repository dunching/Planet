// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <atomic>

#include "CoreMinimal.h"

#include "GenerateType.h"

#include "GroupsManaggerSubSystem.generated.h"

class IPlanetControllerInterface;
struct FCharacterProxy;
class AHumanCharacter;
class ACharacterBase;

UCLASS()
class PLANET_API UGroupsManaggerSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	using FPawnType = ACharacterBase;

	using FMemberChangedDelegateContainer = TCallbackHandleContainer<void(EGroupMateChangeType, FPawnType*)>;

	using FTeammateOptionChangedDelegateContainer = TCallbackHandleContainer<void(ETeammateOption, FPawnType*)>;

	static UGroupsManaggerSubSystem* GetInstance();

private:

};