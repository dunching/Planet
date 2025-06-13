#pragma once

#include "CoreMinimal.h"
#include "ItemDecription.h"

#include "PlanetGameplayAbility.h"
#include "SceneProxyTable.h"
#include "Skill_Passive_Base.h"

#include "Skill_Passive_XR.generated.h"

struct FStreamableHandle;

class UAbilityTask_TimerHelper;
class UEffectItem;
class ACharacterBase;
class UGE_ZMJZ;
class UGE_ZMJZImp;

struct FGAEventData;
struct FCharacterStateInfo;
struct FOnEffectedTargetCallback;

UCLASS()
class PLANET_API UItemProxy_Description_PassiveSkill_XR : public UItemProxy_Description_PassiveSkill
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FPerLevelValue_Float CD = {1, 2, 3, 4, 5};

};

UCLASS()
class PLANET_API UItemDecription_Skill_PassiveSkill_XR : public UItemDecription_Skill_PassiveSkill
{
	GENERATED_BODY()

public:
	using FItemProxy_DescriptionType = UItemProxy_Description_PassiveSkill_XR;
private:
	virtual void SetUIStyle() override;
};

/**
 * 讯刃的效果，找一个在CD的主动技能减少冷却
 */
UCLASS()
class PLANET_API USkill_Passive_XR : public USkill_Passive_Base
{
	GENERATED_BODY()

	using FItemProxy_DescriptionType = UItemProxy_Description_PassiveSkill_XR;
	
public:
	using FMakedDamageHandle =
	TCallbackHandleContainer<void(
		const FOnEffectedTargetCallback&
	)>::FCallbackHandleSPtr;

	virtual void OnAvatarSet(
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilitySpec& Spec
	) override;

	virtual void OnRemoveAbility(
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilitySpec& Spec
	) override;

protected:

	void MakedDamageDelegate(
		const FOnEffectedTargetCallback& ReceivedEventModifyDataCallback
	);
	
	FMakedDamageHandle AbilityActivatedCallbacksHandle;

private:
	
	TObjectPtr<FItemProxy_DescriptionType> ItemProxy_DescriptionPtr = nullptr;

};
