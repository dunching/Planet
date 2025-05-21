#pragma once

#include <functional>
#include <set>

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Components/ActorComponent.h"

#include "GroupManaggerInterface.h"
#include "PlanetAbilitySystemComponent.h"
#include "DataModifyInterface.h"

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
struct FOnEffectedTawrgetCallback;

TMap<ECharacterPropertyType, FBaseProperty> GetAllData();

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

	void ModifyOutputData(
		const FGameplayTagContainer & AllAssetTags,
		TSet<FGameplayTag>& NeedModifySet,
		TMap<FGameplayTag, float>& NewDatas,
		TSet<EAdditionalModify>& AdditionalModifyAry,
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput
		);

	void ModifyInputData(
		const FGameplayTagContainer & AllAssetTags,
		TSet<FGameplayTag>& NeedModifySet,
		TMap<FGameplayTag, float>& NewDatas,
		TSet<EAdditionalModify>& AdditionalModifyAry,
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput
		);

	void ApplyInputData(
		const FGameplayTagContainer & AllAssetTags,
		TSet<FGameplayTag>& NeedModifySet,
		const TMap<FGameplayTag, float>& CustomMagnitudes,
		const TSet<EAdditionalModify>& AdditionalModifyAry,
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput
		);

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
		FVector RepelDirection,
		int32 RepelDistance
		);

	void SwitchCantBeSelect(
		bool bIsCanBeSelect
		);

	/**
	 * 是否进入隐身状态
	 * @param bIsInvisible 
	 */
	void SwitchInvisible(
		bool bIsInvisible
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

public:
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
	 * 更新数据组成
	 * @param Tag 仅为 DataSource_Character
	 * @param Value 
	 * @param MinValue 仅GEData_ModifyType_BaseValue_Addtive、GEData_ModifyType_Immediate_Override 生效
	 * @param MaxValue 仅GEData_ModifyType_BaseValue_Addtive、GEData_ModifyType_Immediate_Override 生效
	 * @param Spec 
	 * @param GameplayAttributeDataPtr 
	 */
	void UpdateMapBaseValue(
		const FGameplayTag& Tag,
		float Value,
		int32 MinValue,
		int32 MaxValue,
		const FGameplayEffectSpec& Spec,
		const FGameplayAttributeData* GameplayAttributeDataPtr
		);

	void UpdateMapTemporary(
		const FGameplayTag& Tag,
		float Value,
		const FGameplayEffectSpec& Spec,
		const FGameplayAttributeData* GameplayAttributeDataPtr
		);

	/**
	 * 更新数据组成
	 * @param Value 该属性之后的值
	 * @param AllAssetTags 
	 * @param GameplayAttributeDataPtr 
	 */
	void UpdateMapTemporary(
		const FGameplayTag& ModifyTypeTag,
		float Value,
		const FGameplayAttributeData* GameplayAttributeDataPtr
		);

	/**
	 * 获取基础数据
	 * @param Spec 
	 * @param GameplayAttributeDataPtr 
	 * @return 
	 */
	float GetBaseValueMaps(
		const FGameplayEffectSpec& Spec,
		const FGameplayAttributeData* GameplayAttributeDataPtr
		) const;

private:

	void UpdateValueMap();
	
#pragma region 基础GA
	/**
	 * 基础GA
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<UBasicFuturesBase>> CharacterAbilitiesAry;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Element Skills", meta = (AllowPrivateAccess = "true"))
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
