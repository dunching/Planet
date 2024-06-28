#pragma once

#include <functional>
#include <set>

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "GenerateType.h"
#include <SceneElement.h>
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpecHandle.h"
#include "GAEvent_Helper.h"

#include "EquipmentElementComponent.generated.h"

class UGameplayAbility;

class UBasicFuturesBase;
class UPlanetGameplayAbility_HumanSkillBase;
class IGAEventModifyInterface;
class AEquipmentBase;
class USkill_Base;
class USkill_Active_Base;
class AWeapon_Base;
class UGAEvent_Send;
class UGAEvent_Received;
class USkill_Element_Gold;

struct FGameplayAbilityTargetData_GASendEvent;
class UWeaponUnit;

struct FSkillSocketInfo
{
	FKey Key;

	FGameplayTag SkillSocket;

	USkillUnit* SkillUnit = nullptr;

	// Active类型只会有一个技能
	TArray<FGameplayAbilitySpecHandle>HandleAry;
};

struct FWeaponSocketInfo
{
	FGameplayTag WeaponSocket;

	UWeaponUnit* WeaponUnitPtr = nullptr;

	FGameplayAbilitySpecHandle Handle;
};

struct FCanbeActivedInfo
{
	enum class EType
	{
		kActiveSkill,
		kWeaponActiveSkill,
	};

	EType Type = EType::kActiveSkill;

	FKey Key;

	FGameplayTag SkillSocket;
};

struct FToolsSocketInfo
{
	FKey Key;

	FGameplayTag SkillSocket;

	UToolUnit* ToolUnitPtr = nullptr;
};

UCLASS(BlueprintType, Blueprintable)
class UEquipmentElementComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	using FCallbackHandleContainer = TCallbackHandleContainer<void(ETagChangeType, const FGameplayTag&)>;

	using FOnActivedWeaponChangedContainer = TCallbackHandleContainer<void(EWeaponSocket)>;

	UEquipmentElementComponent(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay()override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason)override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)override;

	static FName ComponentName;

	void InitialBaseGAs();

	void OnSendEventModifyData(FGameplayAbilityTargetData_GASendEvent& OutGAEventData);

	void OnReceivedEventModifyData(FGameplayAbilityTargetData_GAReceivedEvent& OutGAEventData);

	const TArray<TSharedPtr<FCanbeActivedInfo>>& GetCanbeActivedInfo()const;

	void GenerationCanbeActivedInfo();

#pragma region Tools
	void RegisterTool(const TSharedPtr < FToolsSocketInfo>& InToolInfo);

	TSharedPtr < FToolsSocketInfo> FindTool(const FGameplayTag& Tag);

	const TMap<FGameplayTag, TSharedPtr<FToolsSocketInfo>>& GetTools()const;
#pragma endregion Tools

#pragma region Skills
	void RegisterMultiGAs(const TMap<FGameplayTag, TSharedPtr<FSkillSocketInfo>>& InSkillsMap);

	TSharedPtr < FSkillSocketInfo> FindSkill(const FGameplayTag& Tag);

	const TMap<FGameplayTag, TSharedPtr<FSkillSocketInfo>>& GetSkills()const;
#pragma endregion Skills

#pragma region Weapon
	void RegisterWeapon(
		const TSharedPtr < FWeaponSocketInfo>& FirstWeaponSocketInfo,
		const TSharedPtr < FWeaponSocketInfo>& SecondWeaponSocketInfo
	);

	void GetWeapon(
		TSharedPtr < FWeaponSocketInfo>& FirstWeaponSocketInfo,
		TSharedPtr < FWeaponSocketInfo>& SecondWeaponSocketInfo
	)const;

	bool SwitchWeapon();

	void RetractputWeapon();

	EWeaponSocket GetActivedWeaponType();

	TSharedPtr < FWeaponSocketInfo >GetActivedWeapon()const;
#pragma endregion Weapon

	void AddSendEventModify(const TSharedPtr<IGAEventModifySendInterface>& GAEventModifySPtr);

	void RemoveSendEventModify(const TSharedPtr<IGAEventModifySendInterface>& GAEventModifySPtr);

	void AddReceviedEventModify(const TSharedPtr<IGAEventModifyReceivedInterface>& GAEventModifySPtr);

	void RemoveReceviedEventModify(const TSharedPtr<IGAEventModifyReceivedInterface>& GAEventModifySPtr);

	FGameplayAbilitySpecHandle SendEventHandle;

	FGameplayAbilitySpecHandle ReceivedEventHandle;

	const FGameplayTagContainer& GetCharacterTags()const;

	void AddTag(const FGameplayTag& Tag);

	void RemoveTag(const FGameplayTag& Tag);

	bool ActiveSkill(const TSharedPtr <FCanbeActivedInfo>& CanbeActivedInfoSPtr, bool bIsAutomaticStop = false);

	void CancelSkill(const TSharedPtr < FCanbeActivedInfo>& CanbeActivedInfoSPtr);

	FCallbackHandleContainer TagsModifyHandleContainer;

	FOnActivedWeaponChangedContainer OnActivedWeaponChangedContainer;

protected:

	bool ActiveWeapon(EWeaponSocket WeaponSocket);

	bool ActivedCorrespondingWeapon(USkill_Active_Base* GAPtr);

	void AddSendGroupEffectModify();

	void AddSendWuXingModify();

	void AddReceivedWuXingModify();

	void AddReceivedModify();

	FDelegateHandle AbilityActivatedCallbacksHandle;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities Tag")
	FGameplayTagContainer CharacterTags;

	std::multiset<TSharedPtr<IGAEventModifySendInterface>, FGAEventModify_key_compare>SendEventModifysMap;

	std::multiset<TSharedPtr<IGAEventModifyReceivedInterface>, FGAEventModify_key_compare>ReceivedEventModifysMap;

#pragma region GAs
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UBasicFuturesBase>> CharacterAbilities;
#pragma endregion GAs

	AWeapon_Base* ActivedWeaponPtr = nullptr;

	EWeaponSocket CurrentActivedWeaponSocket = EWeaponSocket::kNone;

	TSharedPtr<FWeaponSocketInfo>FirstWeaponUnit;

	TSharedPtr<FWeaponSocketInfo>SecondaryWeaponUnit;

	TMap<FGameplayTag, TSharedPtr<FSkillSocketInfo>>SkillsMap;

	TMap<FGameplayTag, TSharedPtr<FToolsSocketInfo>>ToolsMap;

	TArray<TSharedPtr<FCanbeActivedInfo>>CanbeActivedInfoAry;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Element Skills")
	TSubclassOf<USkill_Element_Gold>Skill_Element_GoldClass;

};