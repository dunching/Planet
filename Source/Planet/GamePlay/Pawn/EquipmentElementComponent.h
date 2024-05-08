#pragma once

#include <functional>
#include <set>

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "GenerateType.h"
#include <SceneElement.h>
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpecHandle.h"

#include "EquipmentElementComponent.generated.h"

class UBasicFuturesBase;
class UPlanetGameplayAbility_HumanSkillBase;
class IGAEventModifyInterface;
class AEquipmentBase;
class USkill_Base;
class AWeapon_Base;
class UGAEvent_Send;
class UGAEvent_Received;

struct FGameplayAbilityTargetData_GAEvent;
class UWeaponUnit;

struct FSkillsSocketInfo
{
	FKey Key;

	FGameplayTag SkillSocket;

	USkillUnit* SkillUnit = nullptr;

	FGameplayAbilitySpecHandle Handle;
};

struct FToolsSocketInfo
{
	FKey Key;

	FGameplayTag SkillSocket;

	UToolUnit* ToolUnitPtr = nullptr;
};

enum class EWeaponSocket
{
	kNone,
	kMain,
	kSecondary,
};

UCLASS(BlueprintType, Blueprintable)
class UEquipmentElementComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	using FCallbackHandleContainer = TCallbackHandleContainer<void(ETagChangeType, const FGameplayTag&)>;

	UEquipmentElementComponent(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay()override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)override;

	static FName ComponentName;

	void InitialBaseGAs();

	void OnSendEventModifyData(FGameplayAbilityTargetData_GAEvent& GAEventData);

	void OnReceivedEventModifyData(const FGameplayAbilityTargetData_GAEvent& GAEventData);

	UWeaponUnit* GetMainWeaponUnit()const;

	void SetMainWeapon(UWeaponUnit* InMainWeaponSPtr);

	UWeaponUnit* GetSecondaryWeaponUnit()const;

	void SetSecondaryWeapon(UWeaponUnit* InMainWeaponSPtr);

	AWeapon_Base* ActiveWeapon(EWeaponSocket WeaponSocket);

	AWeapon_Base* GetActivedWeapon();

	void RegisterMultiGAs(const TMap<FGameplayTag, FSkillsSocketInfo>&InSkillsMap);

	void RegisterTool(const FToolsSocketInfo& InToolInfo);

	FSkillsSocketInfo FindSkill(const FGameplayTag& Tag);

	FToolsSocketInfo FindTool(const FGameplayTag& Tag);

	const TMap<FGameplayTag, FSkillsSocketInfo>& GetSkills()const;

	const TMap<FGameplayTag, FToolsSocketInfo>& GetTools()const;

	void AddGAEventModify(const TSharedPtr<IGAEventModifyInterface>& GAEventModifySPtr);

	void RemoveGAEventModify(const TSharedPtr<IGAEventModifyInterface>& GAEventModifySPtr);

	FGameplayAbilitySpecHandle SendEventHandle;

	FGameplayAbilitySpecHandle ReceivedEventHandle;

	const FGameplayTagContainer& GetCharacterTags()const;

	void AddTag(const FGameplayTag&Tag);

	void RemoveTag(const FGameplayTag& Tag);

	FCallbackHandleContainer TagsModifyHandleContainer;

protected:

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities Tag")
	FGameplayTagContainer CharacterTags;

	std::multiset<TSharedPtr<IGAEventModifyInterface>, FGAEventModify_key_compare>SendEventModifysMap;

#pragma region GAs
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UBasicFuturesBase>> CharacterAbilities;
#pragma endregion GAs

	AWeapon_Base* ActivedWeaponPtr = nullptr;

	UWeaponUnit* MainWeaponUnitPtr = nullptr;

	UWeaponUnit* SecondaryWeaponUnitPtr = nullptr;

	TMap<FGameplayTag, FSkillsSocketInfo>SkillsMap;

	TMap<FGameplayTag, FToolsSocketInfo>ToolsMap;

};