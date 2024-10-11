#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"
#include <GameplayTagContainer.h>
#include "GameplayAbilitySpec.h"

#include "Talent_FASI.h"
#include "GenerateType.h"
#include "CharacterStateInfo.h"

#include "StateProcessorComponent.generated.h"

class UGAEvent_Received;
struct FConsumableProxy;
class UCS_PeriodicPropertyModify;
class UCS_RootMotion;
class UCS_Base;
class UCS_RootMotion_KnockDown;
class ACharacterBase;

struct FGAEventData;
struct FGameplayEventData;
struct FGameplayAbilityTargetData_CS_Base;
struct FGameplayAbilityTargetData_RootMotion;
struct FGameplayAbilityTargetData_PropertyModify;
struct FGameplayAbilityTargetData_StateModify;
struct FGameplayAbilityTargetData_PeriodicPropertyTag;
struct FGameplayAbilityTargetData_MoveToAttaclArea;
struct FGameplayAbilityTargetData;
struct FCharacterStateInfo;

UCLASS(BlueprintType, Blueprintable)
class UStateProcessorComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	friend UGAEvent_Received;
	friend ACharacterBase;

	using FOwnerPawnType = ACharacterBase;

	using FCharacterStateChanged = TCallbackHandleContainer<void(ECharacterStateType, UCS_Base*)>;

	using FCharacterStateMapChanged = TCallbackHandleContainer<void(const TSharedPtr<FCharacterStateInfo>&, bool)>;

	using FMakedDamageDelegate = TCallbackHandleContainer<void(FOwnerPawnType*, const FGAEventData&)>;

	static FName ComponentName;

	UStateProcessorComponent(const FObjectInitializer& ObjectInitializer);

	TSharedPtr<FCharacterStateInfo> GetCharacterState(const FGameplayTag& CSTag)const;
	
	void AddStateDisplay(const TSharedPtr<FCharacterStateInfo>& StateDisplayInfo);

	void ChangeStateDisplay(const TSharedPtr<FCharacterStateInfo>& StateDisplayInfo);

	void RemoveStateDisplay(const TSharedPtr<FCharacterStateInfo>& StateDisplayInfo);

	FCharacterStateChanged CharacterStateChangedContainer;

	FCharacterStateMapChanged CharacterStateMapChanged;

	TMap<FGameplayTag, UCS_Base*>CharacterStateMap;

protected:

	virtual void BeginPlay()override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void BreakOhterState(
		const TSharedPtr<FGameplayAbilityTargetData_CS_Base>& GameplayAbilityTargetDataSPtr,
		const FGameplayTag& ThisTag,
		const TArray<FGameplayTag>& CancelTags);

	FGameplayEventData* MakeTargetData(
		const TSharedPtr<FGameplayAbilityTargetData_CS_Base>& GameplayAbilityTargetDataSPtr
	);
	
	FGameplayAbilitySpec MakeSpec(
		const TSharedPtr<FGameplayAbilityTargetData_CS_Base>& GameplayAbilityTargetDataSPtr,
		TSubclassOf<UGameplayAbility> InAbilityClass
	);

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

	void OnCharacterStateChanged(ECharacterStateType CharacterStateType, UCS_Base* CharacterStatePtr);

	void OnGameplayEffectTagCountChanged(const FGameplayTag Tag, int32 Count);
	
#pragma region GAs
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TSubclassOf<UCS_RootMotion_KnockDown> CS_RootMotion_KnockDownClass;
#pragma endregion GAs

	FDelegateHandle OnGameplayEffectTagCountChangedHandle;
	
	UPROPERTY(Replicated)
	FCharacterStateInfo_FASI_Container CharacterStateInfo_FASI_Container;

	TMap<FGuid, TSharedPtr<FCharacterStateInfo>>StateDisplayMap;

};