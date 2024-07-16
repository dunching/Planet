#pragma once

#include <functional>
#include <set>

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "InteractiveSkillComponent.h"
#include "GAEvent_Helper.h"

#include "InteractiveBaseGAComponent.generated.h"

class UConsumableUnit;
struct FGameplayAbilityTargetData_Periodic_StateTagModefy;
class UGA_Periodic_PropertyModefy;
struct FGameplayAbilityTargetData_Periodic_PropertyModefy;
class UGA_Periodic_StateTagModefy;

UCLASS(BlueprintType, Blueprintable)
class PLANET_API UInteractiveBaseGAComponent : public UInteractiveComponent
{
	GENERATED_BODY()

public:

	using FCallbackHandleContainer = TCallbackHandleContainer<void(ETagChangeType, const FGameplayTag&)>;

	static FName ComponentName;

	void OnSendEventModifyData(FGameplayAbilityTargetData_GASendEvent& OutGAEventData);

	void OnReceivedEventModifyData(FGameplayAbilityTargetData_GAReceivedEvent& OutGAEventData);

	void AddSendEventModify(const TSharedPtr<IGAEventModifySendInterface>& GAEventModifySPtr);

	void RemoveSendEventModify(const TSharedPtr<IGAEventModifySendInterface>& GAEventModifySPtr);

	void AddReceviedEventModify(const TSharedPtr<IGAEventModifyReceivedInterface>& GAEventModifySPtr);

	void RemoveReceviedEventModify(const TSharedPtr<IGAEventModifyReceivedInterface>& GAEventModifySPtr);

	FGameplayAbilitySpecHandle ExcuteEffects(FGameplayAbilityTargetData_Periodic_StateTagModefy* GameplayAbilityTargetDataPtr);
	
	FGameplayAbilitySpecHandle ExcuteEffects(FGameplayAbilityTargetData_Periodic_PropertyModefy* GameplayAbilityTargetDataPtr);
	
	FGameplayAbilitySpecHandle ExcuteEffects(UConsumableUnit* UnitPtr);

	void SendEvent(FGameplayAbilityTargetData_GASendEvent* GAEventDataPtr);

	void SendEvent2Other(
		const TMap<ACharacterBase*, TMap<ECharacterPropertyType, FBaseProperty>>& ModifyPropertyMap,
		bool bIsWeaponAttack = false
	);

	void SendEvent2Self(const TMap<ECharacterPropertyType, FBaseProperty>& ModifyPropertyMap);

	void InitialBaseGAs();

	FGameplayAbilitySpecHandle SendEventHandle;
	
	FGameplayAbilitySpecHandle ReceivedEventHandle;

protected:

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