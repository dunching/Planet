#pragma once

#include <functional>
#include <set>

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Components/ActorComponent.h"

#include "ProxyProcessComponent.h"
#include "GAEvent_Helper.h"
#include "GroupSharedInterface.h"
#include "PlanetAbilitySystemComponent.h"

#include "CharacterAbilitySystemComponent.generated.h"

struct FGameplayAttributeData;

class UGAEvent_Received;
struct FConsumableProxy;
class UCS_PeriodicPropertyModify;
class UCS_RootMotion;
class UCS_Base;
class UCS_RootMotion_KnockDown;
class USkill_Element_Gold;
class UBasicFuturesBase;
class UAbilitySystemComponent;

struct FGameplayEventData;
struct FGameplayAbilityTargetData_CS_Base;
struct FGameplayAbilityTargetData_RootMotion;
struct FGameplayAbilityTargetData_PropertyModify;
struct FGameplayAbilityTargetData_StateModify;
struct FGameplayAbilityTargetData_MoveToAttaclArea;
struct FGameplayAbilityTargetData_TagModify;
struct FGameplayAbilityTargetData;
struct FGameplayEffectCustomExecutionParameters;
struct FGameplayEffectCustomExecutionOutput;

TMap<ECharacterPropertyType, FBaseProperty> GetAllData();

UCLASS()
class PLANET_API UCharacterAbilitySystemComponent :
	public UPlanetAbilitySystemComponent,
	public IGroupSharedInterface
{
	GENERATED_BODY()

public:
	friend UGAEvent_Received;

	using FOwnerPawnType = ACharacterBase;

	using FCharacterStateChanged = TCallbackHandleContainer<void(ECharacterStateType, UCS_Base*)>;

	using FMakedDamageDelegate_Deprecated = TCallbackHandleContainer<void(ACharacterBase*, const FGAEventData&)>;

	using FMakedDamageDelegate = TCallbackHandleContainer<void(ACharacterBase*, const TMap<FGameplayTag, float>&)>;

	virtual void BeginPlay()override;

	bool IsInDeath() const;

	bool IsUnSelected() const;

	bool IsRunning() const;

	bool IsRootMotion() const;

	bool IsInFighting() const;

	void OnSendEventModifyData(FGameplayAbilityTargetData_GASendEvent& OutGAEventData);

	void OnSendEventModifyData(
		UAbilitySystemComponent*AbilitySystemComponentPtr,
		FGameplayEffectSpecHandle GameplayEffectSpecHandle
	);

	void OnReceivedEventModifyData(
		const TMap<FGameplayTag, float>& CustomMagnitudes,
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput
	);

	void AddSendEventModify(const TSharedPtr<IGAEventModifySendInterface>& GAEventModifySPtr);

	void RemoveSendEventModify(const TSharedPtr<IGAEventModifySendInterface>& GAEventModifySPtr);

	void AddReceviedEventModify(const TSharedPtr<IGAEventModifyReceivedInterface>& GAEventModifySPtr);

	void RemoveReceviedEventModify(const TSharedPtr<IGAEventModifyReceivedInterface>& GAEventModifySPtr);

	void ClearData2Other(
		const TMap<ACharacterBase*, TMap<ECharacterPropertyType, FBaseProperty>>& ModifyPropertyMap,
		const FGameplayTag& DataSource
	);

	void ClearData2Self(
		const TMap<ECharacterPropertyType, FBaseProperty>& ModifyPropertyMap,
		const FGameplayTag& DataSource
	);

	void SendEvent2Other(
		const TMap<ACharacterBase*, TMap<ECharacterPropertyType, FBaseProperty>>& ModifyPropertyMap,
		const FGameplayTag& DataSource
	);

	// 增加
	void SendEvent2Self(
		const TMap<ECharacterPropertyType, FBaseProperty>& ModifyPropertyMap,
		const FGameplayTag& DataSource
	);

	/*
		一次性的伤害结算
	*/
	void SendEventImp(
		FGameplayAbilityTargetData_GASendEvent* GAEventDataPtr
	);

	/*
		周期类的标签
	*/
	void SendEventImp(
		FGameplayAbilityTargetData_TagModify* GameplayAbilityTargetDataPtr
	);

	/*
		周期类的跟运动修改
	*/
	void SendEventImp(
		FGameplayAbilityTargetData_RootMotion* GameplayAbilityTargetDataPtr
	);

	/*
		周期类的状态修改
	*/
	void SendEventImp(
		FGameplayAbilityTargetData_StateModify* GameplayAbilityTargetDataPtr
	);

	/*
		周期类的属性修改
	*/
	void SendEventImp(
		FGameplayAbilityTargetData_PropertyModify* GameplayAbilityTargetDataPtr
	);

	void InitialBaseGAs();

	UFUNCTION(Server, Reliable)
	void SwitchWalkState(bool bIsRun);

	// 冲刺/闪避
	UFUNCTION(Server, Reliable)
	void Dash(EDashDirection DashDirection);

	// 
	UFUNCTION(Server, Reliable)
	void Jump();

	void SwitchCantBeSelect(bool bIsCanBeSelect);

	void Respawn();

	// 移动至攻击范围内
	void MoveToAttackDistance(
		FGameplayAbilityTargetData_MoveToAttaclArea* MoveToAttaclAreaPtr
	);

	// 取消 移动至攻击范围内
	void BreakMoveToAttackDistance();

	// “受击”效果
	void ExcuteAttackedEffect(const FGameplayAbilityTargetData_GAReceivedEvent& GAEvent);

	FGameplayAbilitySpecHandle SendEventHandle;

	FGameplayAbilitySpecHandle ReceivedEventHandle;

	FGameplayAbilitySpecHandle MoveToAttaclAreaHandle;

	// 对“其他”角色造成的影响（伤害、控制）
	FMakedDamageDelegate_Deprecated MakedDamageDelegate_Deprecated;

	// 对“其他”角色造成的影响（伤害、控制）
	FMakedDamageDelegate MakedDamageDelegate;

protected:
	
	virtual void OnGroupSharedInfoReady(AGroupSharedInfo* NewGroupSharedInfoPtr) override;

	void OnGEAppliedDelegateToTarget(
		UAbilitySystemComponent*,
		const FGameplayEffectSpec&,
		FActiveGameplayEffectHandle
		);

	void OnActiveGEAddedDelegateToSelf(
		UAbilitySystemComponent*,
		const FGameplayEffectSpec&,
		FActiveGameplayEffectHandle
		);

	void AddSendBaseModify();

	void AddReceivedBaseModify();

	EAffectedDirection GetAffectedDirection(
		ACharacterBase* TargetCharacterPtr,
		ACharacterBase* TriggerCharacterPtr
	);

	void UpdateMap(
		const FGameplayTag& Tag,
		float Value,
		int32 MinValue,
		int32 MaxValue,
		const FGameplayEffectSpec& Spec,
		const FGameplayAttributeData* GameplayAttributeDataPtr
		);

	float GetMapValue(
		const FGameplayEffectSpec& Spec,
		const FGameplayAttributeData* GameplayAttributeDataPtr
	) const;

#pragma region GAs
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UBasicFuturesBase>> CharacterAbilitiesAry;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Element Skills")
	TSubclassOf<USkill_Element_Gold> Skill_Element_GoldClass;
#pragma endregion GAs

	// 从小到大
	std::multiset<TSharedPtr<IGAEventModifySendInterface>, FGAEventModify_key_compare> SendEventModifysMap;

	std::multiset<TSharedPtr<IGAEventModifyReceivedInterface>, FGAEventModify_key_compare> ReceivedEventModifysMap;

	// GameplayAttributeData的组成
	TMap<const FGameplayAttributeData*, TMap<FGameplayTag, float>> ValueMap;
	
};
