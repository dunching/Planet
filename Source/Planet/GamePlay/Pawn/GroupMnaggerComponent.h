#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"

#include "GenerateType.h"

#include "GroupMnaggerComponent.generated.h"

class AHumanCharacter;
class ACharacterBase;
class IPlanetControllerInterface;
class UGroupMnaggerComponent;
struct FSkillProxy;
struct FActiveSkillProxy;
struct FConsumableProxy;
struct FCharacterProxy;

class PLANET_API FTeamMatesHelper
{
public:

	friend UGroupMnaggerComponent;

	using FCharacterUnitType = FCharacterProxy;

	using FPawnType = ACharacterBase;

	using FMemberChangedDelegateContainer =
		TCallbackHandleContainer<void(EGroupMateChangeType, const TSharedPtr<FCharacterUnitType>&)>;

	using FTeammateOptionChangedDelegateContainer = 
		TCallbackHandleContainer<void(ETeammateOption, const TSharedPtr<FCharacterUnitType>&)>;

	using FKnowCharaterChanged = 
		TCallbackHandleContainer<void(TWeakObjectPtr<ACharacterBase>, bool)>;

	void AddCharacter(FPawnType* PCPtr);

	void AddCharacter(const TSharedPtr<FCharacterUnitType>& CharacterUnitPtr);

	void SwitchTeammateOption(ETeammateOption InTeammateOption);

	ETeammateOption GetTeammateOption()const;

	void AddKnowCharacter(ACharacterBase*CharacterPtr);

	void RemoveKnowCharacter(ACharacterBase* CharacterPtr);

	TWeakObjectPtr<ACharacterBase> GetKnowCharacter()const;

	FTeammateOptionChangedDelegateContainer TeammateOptionChanged;

	FKnowCharaterChanged KnowCharaterChanged;

	FMemberChangedDelegateContainer MembersChanged;

	TSharedPtr<FCharacterUnitType> OwnerCharacterUnitPtr = nullptr;

	// 分配的小队
	TSet<TSharedPtr<FCharacterUnitType>> MembersSet;

private:

	FGuid Guid = FGuid();

	TWeakObjectPtr<ACharacterBase>ForceKnowCharater;

	TArray<TPair<TWeakObjectPtr<ACharacterBase>, int32>>KnowCharatersSet;

	ETeammateOption TeammateOption = ETeammateOption::kEnemy;

};

// Character之间共享的信息
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

	void SpwanTeammateCharacter();

	void AddCharacterToTeam(const TSharedPtr<FCharacterUnitType>& CharacterUnitPtr, int32 Index);

	TSharedPtr<FTeamMatesHelper> CreateTeam();

	void OnAddToNewTeam(const TSharedPtr<FCharacterUnitType>& CharacterUnitPtr);

	const TSharedPtr<FTeamMatesHelper>& GetTeamHelper();

	bool IsMember(const TSharedPtr<FCharacterUnitType>& CharacterUnitPtr)const;

	FTeamHelperChangedDelegateContainer TeamHelperChangedDelegateContainer;

	FTeamHelperChangedDelegateContainer GroupHelperChangedDelegateContainer;

	TSet<AHumanCharacter*>TargetSet;

protected:

	virtual void BeginPlay()override;
	
	UFUNCTION(Server, Reliable)
	virtual void AddCharacterToTeam_Server(const FGuid&ProxtID, int32 Index);
	
private:

	TSharedPtr<FTeamMatesHelper> TeamHelperSPtr;

};