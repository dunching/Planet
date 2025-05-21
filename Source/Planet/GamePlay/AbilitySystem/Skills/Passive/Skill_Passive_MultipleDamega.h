#pragma once

#include "CoreMinimal.h"
#include "ItemDecription.h"

#include "PlanetGameplayAbility.h"
#include "SceneProxyTable.h"
#include "Skill_Passive_Base.h"

#include "Skill_Passive_MultipleDamega.generated.h"

struct FStreamableHandle;

class UAbilityTask_TimerHelper;
class UEffectItem;
class ACharacterBase;
class UGE_ZMJZ;
class UGE_ZMJZImp;
class IOutputDataModifyInterface;

struct FGAEventData;
struct FCharacterStateInfo;
struct FOnEffectedTawrgetCallback;

UCLASS()
class PLANET_API UItemProxy_Description_PassiveSkill_MultipleDamega : public UItemProxy_Description_PassiveSkill
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 Count = 2;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	float Multiple = 2.f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FPerLevelValue_Float Duration = {3, 4, 5, 6, 7};

};

UCLASS()
class PLANET_API UItemDecription_Skill_PassiveSkill_MultipleDamega : public UItemDecription
{
	GENERATED_BODY()

public:
	using FItemProxy_DescriptionType = UItemProxy_Description_PassiveSkill_MultipleDamega;
private:
	virtual void SetUIStyle() override;
};

/**
 * 致命节奏 增加平通攻击得速度
 */
UCLASS()
class PLANET_API USkill_Passive_MultipleDamega : public USkill_Passive_Base
{
	GENERATED_BODY()

	using FItemProxy_DescriptionType = UItemProxy_Description_PassiveSkill_MultipleDamega;
	
public:
	using FMakedDamageHandle =
	TCallbackHandleContainer<void(
		const FOnEffectedTawrgetCallback&
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
		const FOnEffectedTawrgetCallback& ReceivedEventModifyDataCallback
	);

	void DurationDelegate(
		UAbilityTask_TimerHelper* TaskPtr,
		float CurrentInterval,
		float Duration
	);
	
	bool OnTimerFinished(
		UAbilityTask_TimerHelper* TaskPtr
	);
	
	TSharedPtr<IOutputDataModifyInterface>OutputDataModifySPtr = nullptr;
	
	FMakedDamageHandle AbilityActivatedCallbacksHandle;
private:
	
	TObjectPtr<FItemProxy_DescriptionType> ItemProxy_DescriptionPtr = nullptr;

	bool bIsRead =true;
};
