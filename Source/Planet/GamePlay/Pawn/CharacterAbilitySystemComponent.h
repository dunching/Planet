#pragma once

#include <functional>
#include <set>

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Components/ActorComponent.h"

#include "GroupManaggerInterface.h"
#include "PlanetAbilitySystemComponent.h"
#include "DataModifyInterface.h"
#include "BaseData.h"
#include "GameplayEffectDataModifyInterface.h"
#include "GenerateTypes.h"

#include "CharacterAbilitySystemComponent.generated.h"

struct FGameplayAttributeData;

class ATornado;
class ATractionPoint;
class UGAEvent_Received;
struct FConsumableProxy;
class UCS_PeriodicPropertyModify;
class UCS_RootMotion;
class UCS_Base;
class UCS_RootMotion_KnockDown;
class USkill_Element_Metal;
class UBasicFuturesBase;
class ACharacterBase;
class UAbilitySystemComponent;
class IOutputDataModifyInterface;
class IInputDataModifyInterface;
class IOutputDataModifyInterface;

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
struct FOnEffectedTargetCallback;

TMap<ECharacterPropertyType, FBaseProperty> GetAllData();

UCLASS()
class PLANET_API UCharacterAbilitySystemComponent :
	public UPlanetAbilitySystemComponent,
	public IGroupManaggerInterface,
	public IGameplayEffectDataModifyInterface
{
	GENERATED_BODY()

public:
	friend UGAEvent_Received;

	using FOwnerPawnType = ACharacterBase;

	using FCharacterStateChanged = TCallbackHandleContainer<void(
		ECharacterStateType,
		UCS_Base*

		
		)>;

	using FMakedDamageDelegate = TCallbackHandleContainer<void(
		const FOnEffectedTargetCallback&

		
		)>;

	UCharacterAbilitySystemComponent(
		const FObjectInitializer& ObjectInitializer
		);

	virtual void BeginPlay() override;

	bool IsCantBeDamage() const;

	bool IsInDeath() const;

	bool IsUnSelected() const;

	bool IsRunning() const;

	bool IsRootMotion() const;

	bool IsInFighting() const;

	void InitialBaseGAs();

#pragma region 输入和输出得修正

	virtual void ApplyInputData(
		const FGameplayTagContainer& AllAssetTags,
		TSet<FGameplayTag>& NeedModifySet,
		const TMap<FGameplayTag, float>& CustomMagnitudes,
		const TSet<EAdditionalModify>& AdditionalModifyAry,
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput
		) override;

	virtual bool CheckCost(
		const TMap<FGameplayTag, int32>& CostMap
		) const override;

	int32 GetCooldown(
		int32 Cooldown
		) const;

#pragma endregion

#pragma region 基础GA

	UFUNCTION(Server, Reliable)
	void SwitchWalkState(
		bool bIsRun
		);

	// 冲刺/闪避
	UFUNCTION(Server, Reliable)
	void Dash(
		EDashDirection DashDirection
		);

	// 
	UFUNCTION(Server, Reliable)
	void Jump();

	/**
	 * 切换武器
	 */
	UFUNCTION(Server, Reliable)
	void SwitchWeapon();

	/**
	 * 被击飞
	 * @param Height 
	 * Only Server
	 */
	UFUNCTION(Server, Reliable)
	void HasBeenFlayAway(
		int32 Height
		);

	/**
	 * 被击飞
	 * Only Server Invoke
	 */
	void HasbeenTornodo(
		const TWeakObjectPtr<ATornado>& TornadoPtr
		);

	/**
	 * 被牵引
	 * Only Server Invoke
	 */
	void HasbeenTraction(
		const TWeakObjectPtr<ATractionPoint>& TractionPointPtr
		);

	/**
	 * 被压制
	 * Only Server Invoke
	 */
	void HasbeenSuppress(
		const TWeakObjectPtr<ACharacterBase>& InstigatorPtr,
		const TSoftObjectPtr<UAnimMontage>& Montage,
		float Duration
		);

	/**
	 * 受击
	 * Only Server Invoke
	 */
	void HasbeenAttacked(
		const TWeakObjectPtr<ACharacterBase>& InstigatorPtr,
		FVector RepelDirection
		);

	/**
	 * 被击退
	 * Only Server Invoke
	 */
	void HasBeenRepel(
		const TWeakObjectPtr<ACharacterBase>& InstigatorPtr,
		float Duration,
		FVector RepelDirection,
		int32 RepelDistance
		);

	/**
	 * 是否进入【不可选中】状态
	 * @param bIsCanBeSelect 
	 */
	void SwitchCantBeSelect(
		bool bIsCanBeSelect
		);

	/**
	 * 是否进入【隐身】状态
	 * @param bIsInvisible 
	 */
	void SwitchInvisible(
		bool bIsInvisible
		);

	UFUNCTION(Server, Reliable)
	void Respawn();

	UFUNCTION(Server, Reliable)
	void StunTarget(
		ACharacterBase* TargetCharacterPtr,
		float Duration
		);

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

	// 收到“其他”角色造成的影响（伤害、控制）
	FMakedDamageDelegate ReceivedDamageDelegate;
#pragma endregion

public:
	// 对“其他”角色造成的影响（伤害、控制）
	UFUNCTION(NetMulticast, Reliable)
	void OnEffectOhterCharacter(
		const FOnEffectedTargetCallback& ReceivedEventModifyDataCallback
		);

	// 收到“其他”角色造成的影响（伤害、控制）
	UFUNCTION(NetMulticast, Reliable)
	void OnReceivedOhterCharacter(
		const FOnEffectedTargetCallback& ReceivedEventModifyDataCallback
		);

	virtual void OnGroupManaggerReady(
		AGroupManagger* NewGroupSharedInfoPtr
		) override;

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

	void ModifyType_Permanent(
		FOnEffectedTargetCallback& ReceivedEventModifyDataCallback,
		EUpdateValueType UpdateValueType,
		const FGameplayTagContainer& AllAssetTags,
		TSet<FGameplayTag>& NeedModifySet,
		const TMap<FGameplayTag, float>& CustomMagnitudes,
		const TSet<EAdditionalModify>& AdditionalModifyAry,
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput
		);

	void ModifyType_Temporary(
		FOnEffectedTargetCallback& ReceivedEventModifyDataCallback,
		EUpdateValueType UpdateValueType,
		const FGameplayTagContainer& AllAssetTags,
		TSet<FGameplayTag>& NeedModifySet,
		const TMap<FGameplayTag, float>& CustomMagnitudes,
		const TSet<EAdditionalModify>& AdditionalModifyAry,
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput
		);

#pragma region 基础GA
	/**
	 * 基础GA
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<UBasicFuturesBase>> CharacterAbilitiesAry;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Element Skills", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<USkill_Element_Metal> Skill_Element_GoldClass;
#pragma endregion GAs
};
