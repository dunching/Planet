// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GenerateType.h"

#include "GameFramework/PlayerState.h"

#include "PlanetAbilitySystemComponent.h"
#include "ItemProxy.h"

#include "GroupMnaggerComponent.generated.h"

class AHumanCharacter;
class ACharacterBase;
class IPlanetControllerInterface;
class AGroupSharedInfo;
struct FSkillProxy;
struct FActiveSkillProxy;
struct FConsumableProxy;
struct FCharacterProxy;
class UPlanetAbilitySystemComponent;
class UProxySycHelperComponent;
struct FSceneUnitContainer;

/*
 *	“小队”信息
 */
UCLASS(BlueprintType, Blueprintable)
class UTeamMatesHelperComponent : public UActorComponent
{
	GENERATED_BODY()
public:

	friend AGroupSharedInfo;

	using FCharacterUnitType = FCharacterProxy;

	static FName ComponentName;

	using FPawnType = ACharacterBase;

	using FCharacterUnitType = FCharacterProxy;

	using FOwnerType = ACharacterBase;

	using FMemberChangedDelegateContainer =
		TCallbackHandleContainer<void(EGroupMateChangeType, const TSharedPtr<FCharacterUnitType>&)>;

	using FTeammateOptionChangedDelegateContainer = 
		TCallbackHandleContainer<void(ETeammateOption, const TSharedPtr<FCharacterUnitType>&)>;

	using FKnowCharaterChanged = 
		TCallbackHandleContainer<void(TWeakObjectPtr<ACharacterBase>, bool)>;

	using FTeamHelperChangedDelegateContainer = TCallbackHandleContainer<void()>;

	UTeamMatesHelperComponent(const FObjectInitializer& ObjectInitializer);

	void SwitchTeammateOption(ETeammateOption InTeammateOption);

	ETeammateOption GetTeammateOption()const;

	void AddKnowCharacter(ACharacterBase*CharacterPtr);

	void RemoveKnowCharacter(ACharacterBase* CharacterPtr);

	void SpwanTeammateCharacter();

	void AddCharacterToTeam(const TSharedPtr<FCharacterUnitType>& CharacterUnitPtr, int32 Index);

	bool IsMember(const TSharedPtr<FCharacterUnitType>& CharacterUnitPtr)const;

	TWeakObjectPtr<ACharacterBase> GetKnowCharacter()const;

	FTeammateOptionChangedDelegateContainer TeammateOptionChanged;

	FKnowCharaterChanged KnowCharaterChanged;

	FMemberChangedDelegateContainer MembersChanged;

	TSharedPtr<FCharacterUnitType> OwnerCharacterUnitPtr = nullptr;

	// 分配的小队
	TSet<TSharedPtr<FCharacterUnitType>> MembersSet;

	UPROPERTY(ReplicatedUsing = OnRep_GroupSharedInfoChanged)
	TArray<FGuid> CharactersAry;

	TWeakObjectPtr<ACharacterBase>ForceKnowCharater;

	TArray<TPair<TWeakObjectPtr<ACharacterBase>, int32>>KnowCharatersSet;

	ETeammateOption TeammateOption = ETeammateOption::kEnemy;

	FTeamHelperChangedDelegateContainer TeamHelperChangedDelegateContainer;

	TSet<AHumanCharacter*>TargetSet;

protected:

	virtual void InitializeComponent()override;

	virtual void BeginPlay()override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable)
	virtual void AddCharacterToTeam_Server(const FGuid&ProxtID, int32 Index);

	UFUNCTION(Server, Reliable)
	virtual void SpwanTeammateCharacter_Server();

private:
	
	UFUNCTION()
	void OnRep_GroupSharedInfoChanged();

	void OnAddToNewTeam(const TSharedPtr<FCharacterUnitType>& CharacterUnitPtr);

	void AddCharacter(FPawnType* PCPtr);

	void AddCharacter(const TSharedPtr<FCharacterUnitType>& CharacterUnitPtr);

};
