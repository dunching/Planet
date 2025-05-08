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

class ATornado;
class ATractionPoint;
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
struct FOnEffectedTawrgetCallback;

TMap<ECharacterPropertyType, FBaseProperty> GetAllData();

struct FDataModify_key_compare;

class PLANET_API IDataModifyInterface
{
public:
	friend UCharacterAbilitySystemComponent;
	friend FDataModify_key_compare;

	IDataModifyInterface(
		int32 InPriority = 1
	);

	bool operator<(
		const IDataModifyInterface& RightValue
	) const;

protected:
	bool bIsOnceTime = false;

private:
	// 越小的越先算, 100~200 用于基础功能
	int32 Priority = -1;

	int32 ID = -1;
};

class PLANET_API IOutputDataModifyInterface : public IDataModifyInterface
{
public:
	IOutputDataModifyInterface(
		int32 InPriority = 1
	);

	// Return：本次修改完是否移除本【修正方式】
	virtual bool Modify(
		TMap<FGameplayTag, float>& SetByCallerTagMagnitudes
	);
};

class PLANET_API IInputDataModifyInterface : public IDataModifyInterface
{
public:
	IInputDataModifyInterface(
		int32 InPriority = 1
	);

	virtual bool Modify(
		TMap<FGameplayTag, float>& SetByCallerTagMagnitudes
	);
};

struct FDataModify_key_compare
{
	bool operator()(
		const TSharedPtr<IDataModifyInterface>& lhs,
		const TSharedPtr<IDataModifyInterface>& rhs
	) const
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

	using FCharacterStateChanged = TCallbackHandleContainer<void(
		ECharacterStateType,
		UCS_Base*
	)>;

	using FMakedDamageDelegate = TCallbackHandleContainer<void(
		const FOnEffectedTawrgetCallback&
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

	TMap<FGameplayTag, float> ModifyOutputData(
		const TMap<FGameplayTag, float>& CustomMagnitudes,
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput
	);

	TMap<FGameplayTag, float> ModifyInputData(
		const TMap<FGameplayTag, float>& CustomMagnitudes,
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput
	);

	void ApplyInputData(
		const TMap<FGameplayTag, float>& CustomMagnitudes,
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput);
	
	void AddOutputModify(
		const TSharedPtr<IOutputDataModifyInterface>& GAEventModifySPtr
	);

	void RemoveOutputModify(
		const TSharedPtr<IOutputDataModifyInterface>& GAEventModifySPtr
	);

	void AddInputModify(
		const TSharedPtr<IInputDataModifyInterface>& GAEventModifySPtr
	);

	void RemoveInputModify(
		const TSharedPtr<IInputDataModifyInterface>& GAEventModifySPtr
	);
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
	 * Only Server
	 */
	void HasbeenTornodo(
			const TWeakObjectPtr<ATornado>&  TornadoPtr
	);

	/**
	 * 被牵引
	 * Only Server
	 */
	void HasbeenTraction(
		const TWeakObjectPtr<ATractionPoint>& TractionPointPtr
	);

	void SwitchCantBeSelect(
		bool bIsCanBeSelect
	);

	UFUNCTION(Server, Reliable)
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
	// 对“其他”角色造成的影响（伤害、控制）
	UFUNCTION(NetMulticast, Reliable)
	void OnEffectOhterCharacter(
		const FOnEffectedTawrgetCallback& ReceivedEventModifyDataCallback
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

	/**
	 * 
	 * @param Tag 仅为 DataSource
	 * @param Value 
	 * @param MinValue 
	 * @param MaxValue 
	 * @param Spec 
	 * @param GameplayAttributeDataPtr 
	 */
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

#pragma region 基础GA
	/**
	 * 基础GA
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UBasicFuturesBase>> CharacterAbilitiesAry;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Element Skills")
	TSubclassOf<USkill_Element_Gold> Skill_Element_GoldClass;
#pragma endregion GAs

	// 从小到大
	std::multiset<TSharedPtr<IOutputDataModifyInterface>, FDataModify_key_compare> OutputDataModifysMap;

	std::multiset<TSharedPtr<IInputDataModifyInterface>, FDataModify_key_compare> InputDataModifysMap;

	/**
	 * GameplayAttributeData的组成
	 * 如HP仅有 DataSource_Character 基础组成
	 * 而移速则会由 DataSource_Character 和减速时的减速buff 叠加负数
	 */
	TMap<const FGameplayAttributeData*, TMap<FGameplayTag, float>> ValueMap;
};
