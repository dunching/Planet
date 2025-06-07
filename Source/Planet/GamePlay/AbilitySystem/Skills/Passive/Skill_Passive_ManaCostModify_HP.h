#pragma once

#include "CoreMinimal.h"
#include "ItemDecription.h"

#include "PlanetGameplayAbility.h"
#include "SceneProxyTable.h"
#include "Skill_Passive_Base.h"

#include "Skill_Passive_ManaCostModify_HP.generated.h"

struct FStreamableHandle;

class UAbilityTask_TimerHelper;
class UEffectItem;
class ACharacterBase;
class UGE_ZMJZ;
class UGE_ZMJZImp;
class IGostModifyInterface;

struct FGAEventData;
struct FCharacterStateInfo;
struct FOnEffectedTargetCallback;

UCLASS()
class PLANET_API UItemProxy_Description_PassiveSkill_ManaCostModify_HP : public UItemProxy_Description_PassiveSkill
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	FGameplayTag InCostAttributeTag;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 InManaPercent = 30;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 InNewResourcePercent = 100;
};

UCLASS()
class PLANET_API UItemDecription_Skill_PassiveSkill_ManaCostModify_HP : public UItemDecription
{
	GENERATED_BODY()

public:
	using FItemProxy_DescriptionType = UItemProxy_Description_PassiveSkill_ManaCostModify_HP;

private:
	virtual void SetUIStyle() override;
};

/**
 * 致命节奏 增加平通攻击得速度
 */
UCLASS()
class PLANET_API USkill_Passive_ManaCostModify_HP : public USkill_Passive_Base
{
	GENERATED_BODY()

	using FItemProxy_DescriptionType = UItemProxy_Description_PassiveSkill_ManaCostModify_HP;

public:
	virtual void OnAvatarSet(
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilitySpec& Spec
		) override;

	virtual void OnRemoveAbility(
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilitySpec& Spec
		) override;

protected:
private:
	TSharedPtr<IGostModifyInterface> ModifySPtr = nullptr;

	TObjectPtr<FItemProxy_DescriptionType> ItemProxy_DescriptionPtr = nullptr;
};
