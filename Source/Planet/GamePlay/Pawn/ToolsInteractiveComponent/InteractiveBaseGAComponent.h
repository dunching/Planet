#pragma once

#include <functional>
#include <set>

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "InteractiveSkillComponent.h"
#include "GAEvent_Helper.h"

#include "InteractiveBaseGAComponent.generated.h"

class UGAEvent_Received;
class UConsumableUnit;
class UCS_PeriodicPropertyModify;
class UCS_RootMotion;
class UCS_Base;
class UCS_RootMotion_KnockDown;

struct FGameplayEventData;
struct FGameplayAbilityTargetData_CS_Base;
struct FGameplayAbilityTargetData_RootMotion;
struct FGameplayAbilityTargetData_PropertyModify;
struct FGameplayAbilityTargetData_StateModify;
struct FGameplayAbilityTargetData_PeriodicPropertyTag;
struct FGameplayAbilityTargetData_MoveToAttaclArea;
struct FGameplayAbilityTargetData;

TMap<ECharacterPropertyType, FBaseProperty> GetAllData();

UCLASS(BlueprintType, Blueprintable)
class PLANET_API UInteractiveBaseGAComponent : public UInteractiveComponent
{
	GENERATED_BODY()

public:

	friend UGAEvent_Received;

	using FOwnerPawnType = ACharacterBase;

	using FCharacterStateChanged = TCallbackHandleContainer<void(ECharacterStateType, UCS_Base*)>;

	using FMakedDamageDelegate = TCallbackHandleContainer<void(ACharacterBase*, const FGAEventData&)>;

	static FName ComponentName;

	void OnSendEventModifyData(FGameplayAbilityTargetData_GASendEvent& OutGAEventData);

	void OnReceivedEventModifyData(FGameplayAbilityTargetData_GAReceivedEvent& OutGAEventData);

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
	FGameplayAbilitySpecHandle AddPeriodicPropertyTag(
		FGameplayAbilityTargetData_PeriodicPropertyTag* GameplayAbilityTargetDataPtr
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

	bool SwitchWalkState(bool bIsRun);
	
	bool Dash(EDashDirection DashDirection);

	void MoveToAttackDistance(
		FGameplayAbilityTargetData_MoveToAttaclArea * MoveToAttaclAreaPtr
	);

	void BreakMoveToAttackDistance();

	void ExcuteAttackedEffect(EAffectedDirection AffectedDirection);

	UCS_Base* GetCharacterState(const FGameplayTag&CSTag)const;

	FGameplayAbilitySpecHandle SendEventHandle;

	FGameplayAbilitySpecHandle ReceivedEventHandle;

	FCharacterStateChanged CharacterStateChangedContainer;

	// 对“其他”角色造成的影响（伤害、控制）
	FMakedDamageDelegate MakedDamageDelegate;

protected:

	// 注意：如果是RootMotion类型的状态修改，则此类型的子状态只会为一种，比如：人物先被击飞2s，1s之后又被击飞2s，则刷新击飞时间为2s
	// 有些状态之间会互斥，如被击飞之后再被击落，则会立即取消击飞效果
	// 在这些状态，通常会激活对应的GA，并选择对应的montage
	// 
	// 此状态（RootMotion）通过GA控制人物的可操控状态
	void ExcuteEffects(
		const TSharedPtr<FGameplayAbilityTargetData_RootMotion>& GameplayAbilityTargetDataSPtr
	);

	void ExcuteEffects(
		const TSharedPtr<FGameplayAbilityTargetData_PropertyModify>& GameplayAbilityTargetDataSPtr
	);

	// 普通的状态修改（禁止移动的眩晕、禁锢，沉默，虚弱，禁疗）下，通过动画蓝图选择对应的动画（因为这些状态可以共存，比如人物
	// 先被眩晕2s，此时状态机选择被眩晕的动画，然后被虚弱1s，此时根据需求选择是否过度动画，虚弱结束之后回到眩晕的状态）
	// 
	// 此状态通过Tag控制人物的可操控状态
	void ExcuteEffects(
		const TSharedPtr<FGameplayAbilityTargetData_StateModify>& GameplayAbilityTargetDataSPtr
	);

	FGameplayEventData* MkeSpec(
		const TSharedPtr<FGameplayAbilityTargetData_CS_Base> &GameplayAbilityTargetDataSPtr
	);

	void BreakOhterState(
		const TSharedPtr<FGameplayAbilityTargetData_CS_Base>& GameplayAbilityTargetDataSPtr,
		const FGameplayTag& ThisTag, 
		const TArray<FGameplayTag>& CancelTags);

	void AddSendGroupEffectModify();

	void AddSendWuXingModify();

	void AddReceivedWuXingModify();

	void AddReceivedModify();

	void OnCharacterStateChanged(ECharacterStateType CharacterStateType, UCS_Base * CharacterStatePtr);

#pragma region GAs
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UBasicFuturesBase>> CharacterAbilities;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TSubclassOf<UCS_RootMotion_KnockDown> CS_RootMotion_KnockDownClass;
#pragma endregion GAs

	// 从小到大
	std::multiset<TSharedPtr<IGAEventModifySendInterface>, FGAEventModify_key_compare>SendEventModifysMap;

	std::multiset<TSharedPtr<IGAEventModifyReceivedInterface>, FGAEventModify_key_compare>ReceivedEventModifysMap;

	TMap<FGameplayTag, UCS_Base*>CharacterStateMap;

};