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

	// 独立的CD
	FGuid SkillProxy_ID;

	// 类型的CD
	FGameplayTag SkillType;

	// 
	int32 CooldownTime = -1;

	// 
	float CooldownConsumeTime = 0.f;

private:

};

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

	// 公共的冷却，如：团队里面的复活技能
	TMap<FGameplayTag, TSharedPtr<FSkillCooldownHelper>>CommonCooldownMap;

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

	// 重置技能CD（包含公共CD）至满CD
	TMap<FGameplayTag, TWeakPtr<FSkillCooldownHelper>>ApplyCooldown(FActiveSkillProxy* ActiveSkillUnitPtr);

	// 重置技能CD 至满CD
	TWeakPtr<FSkillCooldownHelper> ApplyUniqueCooldown(FActiveSkillProxy* ActiveSkillUnitPtr);

	TMap<FGameplayTag, TWeakPtr<FSkillCooldownHelper>>GetCooldown(const FActiveSkillProxy* ActiveSkillUnitPtr);

	TMap<FGameplayTag, TWeakPtr<FSkillCooldownHelper>>ApplyCooldown(FConsumableProxy* UnitPtr);

	TMap<FGameplayTag, TWeakPtr<FSkillCooldownHelper>>GetCooldown(const FConsumableProxy* ConsumableUnitPtr);

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