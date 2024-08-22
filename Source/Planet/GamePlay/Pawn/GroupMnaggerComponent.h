#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"

#include "GroupsManaggerSubSystem.h"

#include "GroupMnaggerComponent.generated.h"

class AHumanCharacter;
class IPlanetControllerInterface;
class USkillUnit;
class UActiveSkillUnit;
class UConsumableUnit;

struct FSkillCooldownHelper
{
	FSkillCooldownHelper();

	virtual bool CheckCooldown()const;

	void IncreaseCooldownTime(float DeltaTime);

	void AddCooldownConsumeTime(float NewTime);

	void FreshCooldownTime();

	void ResetCooldownTime();

	void OffsetCooldownTime();

	bool GetRemainingCooldown(
		float& RemainingCooldown, float& RemainingCooldownPercent
	)const;

	void SetCooldown(float CooldDown);

private:

	// 
	int32 CooldownTime = -1;

	// 
	float CooldownConsumeTime = 0.f;
};

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

	// 公共的冷却，如：团队里面的复活技能
	TMap<FGameplayTag, TSharedPtr<FSkillCooldownHelper>>CommonCooldownMap;

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

	using FOwnerType = ACharacterBase;

	using FPawnType = ACharacterBase;

	using FTeamHelperChangedDelegateContainer = TCallbackHandleContainer<void()>;

	static FName ComponentName;

	UGroupMnaggerComponent(const FObjectInitializer& ObjectInitializer);

	virtual void TickComponent(
		float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction
	)override;

	void AddCharacterToGroup(FCharacterUnitType* CharacterUnitPtr);

	void AddCharacterToTeam(FCharacterUnitType* CharacterUnitPtr);

	TSharedPtr<FGroupMatesHelper> CreateGroup();

	TSharedPtr<FTeamMatesHelper> CreateTeam();

	void OnAddToNewGroup(FCharacterUnitType* CharacterUnitPtr);

	void OnAddToNewTeam(FCharacterUnitType* CharacterUnitPtr);

	const TSharedPtr<FGroupMatesHelper>& GetGroupHelper();

	const TSharedPtr<FTeamMatesHelper>& GetTeamHelper();

	// 重置技能CD（包含公共CD）至满CD
	TMap<FGameplayTag, TWeakPtr<FSkillCooldownHelper>>ApplyCooldown(UActiveSkillUnit* ActiveSkillUnitPtr);

	// 重置技能CD 至满CD
	TWeakPtr<FSkillCooldownHelper> ApplyUniqueCooldown(UActiveSkillUnit* ActiveSkillUnitPtr);

	TMap<FGameplayTag, TWeakPtr<FSkillCooldownHelper>>GetCooldown(const UActiveSkillUnit* ActiveSkillUnitPtr);

	TMap<FGameplayTag, TWeakPtr<FSkillCooldownHelper>>ApplyCooldown(UConsumableUnit* UnitPtr);

	TMap<FGameplayTag, TWeakPtr<FSkillCooldownHelper>>GetCooldown(const UConsumableUnit* ConsumableUnitPtr);

	FTeamHelperChangedDelegateContainer TeamHelperChangedDelegateContainer;

	FTeamHelperChangedDelegateContainer GroupHelperChangedDelegateContainer;

	TSet<AHumanCharacter*>TargetSet;

protected:

	virtual void BeginPlay()override;

	TWeakPtr<FSkillCooldownHelper> ApplyCommonCooldownTime(
		const FGameplayTag& CommonCooldownTag
	);

private:

	TSharedPtr<FGroupMatesHelper> GroupHelperSPtr;

	TSharedPtr<FTeamMatesHelper> TeamHelperSPtr;

	// 角色的技能冷却
	TMap<FGameplayTag, TSharedPtr<FSkillCooldownHelper>>UniqueCooldownMap;

};