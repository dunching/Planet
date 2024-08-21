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

struct FGameplayAbilityTargetData_RootMotion;
struct FGameplayAbilityTargetData_PropertyModify;
struct FGameplayAbilityTargetData_StateModify;
struct FGameplayAbilityTargetData_AddTemporaryTag;
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

	using FMakedDamage = TCallbackHandleContainer<void(ACharacterBase*, const FGAEventData&)>;

	static FName ComponentName;

	void OnSendEventModifyData(FGameplayAbilityTargetData_GASendEvent& OutGAEventData);

	void OnReceivedEventModifyData(FGameplayAbilityTargetData_GAReceivedEvent& OutGAEventData);

	void AddSendEventModify(const TSharedPtr<IGAEventModifySendInterface>& GAEventModifySPtr);

	void RemoveSendEventModify(const TSharedPtr<IGAEventModifySendInterface>& GAEventModifySPtr);

	void AddReceviedEventModify(const TSharedPtr<IGAEventModifyReceivedInterface>& GAEventModifySPtr);

	void RemoveReceviedEventModify(const TSharedPtr<IGAEventModifyReceivedInterface>& GAEventModifySPtr);

	FGameplayAbilitySpecHandle AddTemporaryTag(
		ACharacterBase* TargetCharacterPtr, 
		FGameplayAbilityTargetData_AddTemporaryTag* GameplayAbilityTargetDataPtr
	);

	void ClearData2Self(
		const FGameplayTag &DataSource
	);

	void SendEvent2Other(
		const TMap<ACharacterBase*, TMap<ECharacterPropertyType, FBaseProperty>>& ModifyPropertyMap,
		const FGameplayTag& DataSource
	);

	void SendEvent2Self(
		const TMap<ECharacterPropertyType, FBaseProperty>& ModifyPropertyMap,
		const FGameplayTag& DataSource
	);

	void SendEventImp(
		FGameplayAbilityTargetData_GASendEvent* GAEventDataPtr
	);
	
	void SendEventImp(
		FGameplayAbilityTargetData_RootMotion* GameplayAbilityTargetDataPtr
	);

	void SendEventImp(
		FGameplayAbilityTargetData_StateModify* GameplayAbilityTargetDataPtr
	);

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

	FMakedDamage MakedDamage;

protected:

	// 注意：如果是RootMotion类型的状态修改，则此类型的子状态只会为一种，比如：人物先被击飞2s，1s之后又被击飞2s，则刷新击飞时间为2s
	// 有些状态之间会互斥，如被击飞之后再被击落，则会立即取消击飞效果
	// 在这些状态，通常会激活对应的GA，并选择对应的montage
	// 
	// 此状态（RootMotion）通过GA控制人物的可操控状态
	FGameplayAbilitySpecHandle ExcuteEffects(
		FGameplayAbilityTargetData_RootMotion* GameplayAbilityTargetDataPtr
	);

	void ExcuteEffects(
		TSharedPtr<FGameplayAbilityTargetData_PropertyModify> GameplayAbilityTargetDataSPtr
	);

	// 普通的状态修改（禁止移动的眩晕、禁锢，沉默，虚弱，禁疗）下，通过动画蓝图选择对应的动画（因为这些状态可以共存，比如人物
	// 先被眩晕2s，此时状态机选择被眩晕的动画，然后被虚弱1s，此时根据需求选择是否过度动画，虚弱结束之后回到眩晕的状态）
	// 
	// 此状态通过Tag控制人物的可操控状态
	FGameplayAbilitySpecHandle ExcuteEffects(
		FGameplayAbilityTargetData_StateModify* GameplayAbilityTargetDataPtr
	);

	void AddSendGroupEffectModify();

	void AddSendWuXingModify();

	void AddReceivedWuXingModify();

	void AddReceivedModify();

	void OnCharacterStateChanged(ECharacterStateType CharacterStateType, UCS_Base * CharacterStatePtr);

#pragma region GAs
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UBasicFuturesBase>> CharacterAbilities;

	// 	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	// 	TSubclassOf<UGA_Periodic_PropertyModefy> GA_Periodic_PropertyModefyClass;
	// 
	// 	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	// 	TSubclassOf<UGA_Periodic_StateTagModefy> GA_Periodic_StateTagModefyClass;
#pragma endregion GAs

	std::multiset<TSharedPtr<IGAEventModifySendInterface>, FGAEventModify_key_compare>SendEventModifysMap;

	std::multiset<TSharedPtr<IGAEventModifyReceivedInterface>, FGAEventModify_key_compare>ReceivedEventModifysMap;

	TMap<FGameplayTag, UCS_Base*>CharacterStateMap;

	TMap<FGameplayTag, FGameplayAbilitySpecHandle>PeriodicStateTagModifyMap;
	
	TPair<FGameplayTag, FGameplayAbilitySpecHandle>PreviousRootMotionModify;

};