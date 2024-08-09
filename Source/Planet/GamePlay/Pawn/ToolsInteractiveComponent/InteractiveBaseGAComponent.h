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

struct FGameplayAbilityTargetData_Periodic_RootMotion;
struct FGameplayAbilityTargetData_PropertyModify;
struct FGameplayAbilityTargetData_AddTemporaryTag;
struct FGameplayAbilityTargetData_MoveToAttaclArea;
struct FGameplayAbilityTargetData;

UCLASS(BlueprintType, Blueprintable)
class PLANET_API UInteractiveBaseGAComponent : public UInteractiveComponent
{
	GENERATED_BODY()

public:

	friend UGAEvent_Received;

	using FOwnerPawnType = ACharacterBase;

	using FCallbackHandleContainer = TCallbackHandleContainer<void(ETagChangeType, const FGameplayTag&)>;

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

	void SendEvent2Other(
		const TMap<ACharacterBase*, TMap<ECharacterPropertyType, FBaseProperty>>& ModifyPropertyMap,
		bool bIsWeaponAttack = false
	);

	void SendEvent2Self(const TMap<ECharacterPropertyType, FBaseProperty>& ModifyPropertyMap);

	void SendEventImp(
		FGameplayAbilityTargetData_GASendEvent* GAEventDataPtr
	);
	
	void SendEventImp(
		FGameplayAbilityTargetData_Periodic_RootMotion* GameplayAbilityTargetDataPtr
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

	FGameplayAbilitySpecHandle SendEventHandle;

	FGameplayAbilitySpecHandle ReceivedEventHandle;

protected:

	FGameplayAbilitySpecHandle ExcuteEffects(
		FGameplayAbilityTargetData_Periodic_RootMotion* GameplayAbilityTargetDataPtr
	);

	FGameplayAbilitySpecHandle ExcuteEffects(
		FGameplayAbilityTargetData_PropertyModify* GameplayAbilityTargetDataPtr
	);

	FGameplayAbilitySpecHandle ExcuteEffects2Self(UConsumableUnit* UnitPtr);

	void AddSendGroupEffectModify();

	void AddSendWuXingModify();

	void AddReceivedWuXingModify();

	void AddReceivedModify();

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

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities Tag")
	FGameplayTagContainer CharacterTags;

	TMap<UConsumableUnit*, FGameplayAbilitySpecHandle>PeriodicPropertyModifyMap;

	TMap<FGameplayTag, FGameplayAbilitySpecHandle>PeriodicStateTagModifyMap;

};