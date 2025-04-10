#pragma once

#include <functional>
#include <set>

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Components/ActorComponent.h"

#include "ProxyProcessComponent.h"

#include "GroupManaggerInterface.h"
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
struct FReceivedEventModifyDataCallback;

TMap<ECharacterPropertyType, FBaseProperty> GetAllData();

struct FGAEventModify_key_compare;

class PLANET_API IGAEventModifyInterface
{
public:
	friend UCharacterAbilitySystemComponent;
	friend FGAEventModify_key_compare;

	IGAEventModifyInterface(int32 InPriority = 1);

	bool operator<(const IGAEventModifyInterface& RightValue) const;

protected:
	bool bIsOnceTime = false;

private:
	// 越小的越先算, 100~200 用于基础功能
	int32 Priority = -1;

	int32 ID = -1;
};

class PLANET_API IGAEventModifySendInterface : public IGAEventModifyInterface
{
public:
	IGAEventModifySendInterface(int32 InPriority = 1);

	// Return：本次修改完是否移除本【修正方式】
	virtual bool Modify(
		TMap<FGameplayTag, float>&	SetByCallerTagMagnitudes
		);
};

class PLANET_API IGAEventModifyReceivedInterface : public IGAEventModifyInterface
{
public:
	IGAEventModifyReceivedInterface(int32 InPriority = 1);

	virtual bool Modify(
		TMap<FGameplayTag, float>&	SetByCallerTagMagnitudes
	);
};

struct FGAEventModify_key_compare
{
	bool operator()(const TSharedPtr<IGAEventModifyInterface>& lhs,
					const TSharedPtr<IGAEventModifyInterface>& rhs) const
	{
		return lhs->Priority < rhs->Priority;
	}
};

UCLASS()
class PLANET_API UCharacterAbilitySystemComponent :
	public UPlanetAbilitySystemComponent,
	public IGroupManaggerInterface
{
	GENERATED_BODY()

public:
	friend UGAEvent_Received;

	using FOwnerPawnType = ACharacterBase;

	using FCharacterStateChanged = TCallbackHandleContainer<void(ECharacterStateType, UCS_Base*)>;

	using FMakedDamageDelegate = TCallbackHandleContainer<void(const FReceivedEventModifyDataCallback&)>;

	UCharacterAbilitySystemComponent(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay()override;

	bool IsInDeath() const;

	bool IsUnSelected() const;

	bool IsRunning() const;

	bool IsRootMotion() const;

	bool IsInFighting() const;

	void InitialBaseGAs();

#pragma region 输入和输出得修正
	
	void OnReceivedEventModifyData(
		const TMap<FGameplayTag, float>& CustomMagnitudes,
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput
	);

	void AddSendEventModify(const TSharedPtr<IGAEventModifySendInterface>& GAEventModifySPtr);

	void RemoveSendEventModify(const TSharedPtr<IGAEventModifySendInterface>& GAEventModifySPtr);

	void AddReceviedEventModify(const TSharedPtr<IGAEventModifyReceivedInterface>& GAEventModifySPtr);

	void RemoveReceviedEventModify(const TSharedPtr<IGAEventModifyReceivedInterface>& GAEventModifySPtr);
#pragma endregion

#pragma region 基础GA
	
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

	// FGameplayAbilitySpecHandle SendEventHandle;
	//
	// FGameplayAbilitySpecHandle ReceivedEventHandle;

	FGameplayAbilitySpecHandle MoveToAttaclAreaHandle;

	// 对“其他”角色造成的影响（伤害、控制）
	FMakedDamageDelegate MakedDamageDelegate;

#pragma endregion
	
protected:
	
	virtual void OnGroupManaggerReady(AGroupManagger* NewGroupSharedInfoPtr) override;

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
