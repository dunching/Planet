#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"

#include "GenerateType.h"

#include "GroupMnaggerComponent.generated.h"

class AHumanCharacter;
class ACharacterBase;
class IPlanetControllerInterface;
struct FSkillProxy;
struct FActiveSkillProxy;
struct FConsumableProxy;
struct FCharacterProxy;

class PLANET_API FGroupMatesHelper
{
public:

	using FCharacterUnitType = FCharacterProxy;

	using FPawnType = ACharacterBase;

	using FMemberChangedDelegateContainer = TCallbackHandleContainer<void(EGroupMateChangeType, const TSharedPtr<FCharacterUnitType>&)>;

	void AddCharacter(FPawnType* PCPtr);

	void AddCharacter(const TSharedPtr<FCharacterUnitType>& CharacterUnitPtr);

	bool IsMember(const TSharedPtr<FCharacterUnitType>& CharacterUnitPtr)const;

	int32 ID = 1;

	FMemberChangedDelegateContainer MembersChanged;

	TSharedPtr<FCharacterUnitType> OwnerCharacterUnitPtr = nullptr;

	TSet<TSharedPtr<FCharacterUnitType>> MembersSet;
};

class PLANET_API FTeamMatesHelper : public FGroupMatesHelper
{
public:

	using FTeammateOptionChangedDelegateContainer = TCallbackHandleContainer<void(ETeammateOption, const TSharedPtr<FCharacterUnitType>&)>;

	using FKnowCharaterChanged = TCallbackHandleContainer<void(TWeakObjectPtr<ACharacterBase>, bool)>;

	void SwitchTeammateOption(ETeammateOption InTeammateOption);

	ETeammateOption GetTeammateOption()const;

	void AddKnowCharacter(ACharacterBase*CharacterPtr);

	void RemoveKnowCharacter(ACharacterBase* CharacterPtr);

	TWeakObjectPtr<ACharacterBase> GetKnowCharacter()const;

	FTeammateOptionChangedDelegateContainer TeammateOptionChanged;

	FKnowCharaterChanged KnowCharaterChanged;

private:

	TWeakObjectPtr<ACharacterBase>ForceKnowCharater;

	TArray<TPair<TWeakObjectPtr<ACharacterBase>, int32>>KnowCharatersSet;

	ETeammateOption TeammateOption = ETeammateOption::kEnemy;

};

UCLASS(BlueprintType, Blueprintable)
class PLANET_API UGroupMnaggerComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	using FCharacterUnitType = FCharacterProxy;

	using FOwnerType = ACharacterBase;

	using FPawnType = ACharacterBase;

	using FTeamHelperChangedDelegateContainer = TCallbackHandleContainer<void()>;

	static FName ComponentName;

	UGroupMnaggerComponent(const FObjectInitializer& ObjectInitializer);

	virtual void TickComponent(
		float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction
	)override;

	void AddCharacterToGroup(const TSharedPtr<FCharacterUnitType>& CharacterUnitPtr);

	void AddCharacterToTeam(const TSharedPtr<FCharacterUnitType>& CharacterUnitPtr);

	TSharedPtr<FGroupMatesHelper> CreateGroup();

	TSharedPtr<FTeamMatesHelper> CreateTeam();

	void OnAddToNewGroup(const TSharedPtr<FCharacterUnitType>& CharacterUnitPtr);

	void OnAddToNewTeam(const TSharedPtr<FCharacterUnitType>& CharacterUnitPtr);

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