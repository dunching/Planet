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

	static FName ComponentName;

	void AddCharacterToGroup(AHumanCharacter*CharacterPtr);

	void OnAddToNewGroup(AHumanCharacter* GroupOwnerCharacterPtr);

	const TSharedPtr<UGroupsManaggerSubSystem::FGroupsHelper>&GetGroupsHelper();

protected:

	virtual void BeginPlay()override;

private:

	TSharedPtr<UGroupsManaggerSubSystem::FGroupsHelper> GroupsHelperSPtr;

};