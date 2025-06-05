#pragma once

#include "CoreMinimal.h"
#include "ItemDecription.h"

#include "PlanetGameplayAbility.h"
#include "SceneProxyTable.h"
#include "Skill_Passive_Base.h"

#include "Skill_Passive_ZMJZ.generated.h"

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
class PLANET_API UItemProxy_Description_PassiveSkill_ZMJZ : public UItemProxy_Description_PassiveSkill
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	float DecreamTime = 5.f;

};

UCLASS()
class PLANET_API UItemDecription_Skill_PassiveSkill_ZMJZ : public UItemDecription
{
	GENERATED_BODY()

public:
	using FItemProxy_DescriptionType = UItemProxy_Description_PassiveSkill_ZMJZ;
private:
	virtual void SetUIStyle() override;
};

/**
 * 致命节奏 增加平通攻击得速度
 */
UCLASS()
class PLANET_API USkill_Passive_ZMJZ : public USkill_Passive_Base
{
	GENERATED_BODY()

	using FItemProxy_DescriptionType = UItemProxy_Description_PassiveSkill_ZMJZ;
	
public:
	using FMakedDamageHandle =
	TCallbackHandleContainer<void(
		const FOnEffectedTargetCallback&
	)>::FCallbackHandleSPtr;

	virtual void OnAvatarSet(
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilitySpec& Spec
	) override;

	virtual void PreActivate(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
		const FGameplayEventData* TriggerEventData = nullptr
	) override;

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	virtual void OnRemoveAbility(
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilitySpec& Spec
	) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	) override;

protected:
	void ModifyGASpeed(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	);

	void MakedDamageDelegate(
		const FOnEffectedTargetCallback& ReceivedEventModifyDataCallback
	);

	void DurationDelegate(
		UAbilityTask_TimerHelper* TaskPtr,
		float CurrentInterval,
		float Duration
	);

	bool OnTimerTaskFinished(
		UAbilityTask_TimerHelper* TaskPtr
	);

	void ModifyCharacterData(
		const FGameplayTag& DataSource,
		int32 Value,
		bool bIsClear = false
	);

	void OnActiveGameplayEffectStackChange(
		FActiveGameplayEffectHandle,
		int32 NewStackCount,
		int32 PreviousStackCount
	);

	void OnGameplayEffectRemoved_InfoDelegate(
		const FGameplayEffectRemovalInfo&
		
	);

	float SecondaryDecreamTime = 1.f;

	FMakedDamageHandle EffectOhterCharacterCallbackDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GE")
	TSubclassOf<UGameplayEffect> GE_ZMJZClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GE")
	TSubclassOf<UGameplayEffect> GE_ZMJZImpClass;

private:
	
	TObjectPtr<FItemProxy_DescriptionType> ItemProxy_DescriptionPtr = nullptr;
	
};
