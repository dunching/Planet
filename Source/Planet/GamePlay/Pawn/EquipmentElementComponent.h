#pragma once

#include <functional>
#include <set>

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "GenerateType.h"
#include <SceneElement.h>
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpecHandle.h"
#include "GAEvent.h"

#include "EquipmentElementComponent.generated.h"

class UGameplayAbility;

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

struct FSkillSocketInfo
{
	FGameplayTag SkillSocket;

	USkillUnit* SkillUnit = nullptr;

	FGameplayAbilitySpecHandle Handle;
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

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)override;

	static FName ComponentName;

	void InitialBaseGAs();

	void OnSendEventModifyData(FGameplayAbilityTargetData_GAEvent& OutGAEventData);

	void OnReceivedEventModifyData(FGameplayAbilityTargetData_GAEvent& OutGAEventData);

	void RegisterWeapon(
		const TSharedPtr < FWeaponSocketInfo>& FirstWeaponSocketInfo,
		const TSharedPtr < FWeaponSocketInfo>& SecondWeaponSocketInfo
	);

	void GetWeapon(
		TSharedPtr < FWeaponSocketInfo>& FirstWeaponSocketInfo,
		TSharedPtr < FWeaponSocketInfo>& SecondWeaponSocketInfo
	)const;

	AWeapon_Base* SwitchWeapon();

	void RetractputWeapon();

	EWeaponSocket GetActivedWeapon();

	void RegisterMultiGAs(const TMap<FGameplayTag, TSharedPtr<FSkillSocketInfo>>& InSkillsMap);

	void RegisterTool(const TSharedPtr < FToolsSocketInfo>& InToolInfo);

	void RegisterCanbeActivedInfo(const TArray< TSharedPtr<FCanbeActivedInfo>>& InCanbeActivedInfoAry);

	TSharedPtr < FSkillSocketInfo> FindSkill(const FGameplayTag& Tag);

	TSharedPtr < FToolsSocketInfo> FindTool(const FGameplayTag& Tag);

	const TMap<FGameplayTag, TSharedPtr<FSkillSocketInfo>>& GetSkills()const;

	const TMap<FGameplayTag, TSharedPtr<FToolsSocketInfo>>& GetTools()const;

	const TArray<TSharedPtr<FCanbeActivedInfo>>& GetCanbeActivedInfo()const;

	void AddGAEventModify(const TSharedPtr<IGAEventModifyInterface>& GAEventModifySPtr);

	void RemoveGAEventModify(const TSharedPtr<IGAEventModifyInterface>& GAEventModifySPtr);

	FGameplayAbilitySpecHandle SendEventHandle;

	FGameplayAbilitySpecHandle ReceivedEventHandle;

	const FGameplayTagContainer& GetCharacterTags()const;

	void AddTag(const FGameplayTag& Tag);

	void RemoveTag(const FGameplayTag& Tag);

	void ActiveSkill(const TSharedPtr < FCanbeActivedInfo>& CanbeActivedInfoSPtr);

	void CancelSkill(const TSharedPtr < FCanbeActivedInfo>& CanbeActivedInfoSPtr);

	FCallbackHandleContainer TagsModifyHandleContainer;

	FOnActivedWeaponChangedContainer OnActivedWeaponChangedContainer;

protected:

	AWeapon_Base* ActiveWeapon(EWeaponSocket WeaponSocket);

	bool ActivedCorrespondingWeapon(USkill_Base* GAPtr);

	FDelegateHandle AbilityActivatedCallbacksHandle;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities Tag")
	FGameplayTagContainer CharacterTags;

	std::multiset<TSharedPtr<IGAEventModifyInterface>, FGAEventModify_key_compare>SendEventModifysMap;

	std::multiset<TSharedPtr<IGAEventModifyInterface>, FGAEventModify_key_compare>ReceivedEventModifysMap;

#pragma region GAs
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UBasicFuturesBase>> CharacterAbilities;
#pragma endregion GAs

	AWeapon_Base* ActivedWeaponPtr = nullptr;

	EWeaponSocket CurrentActivedWeaponSocket = EWeaponSocket::kNone;

	TSharedPtr < FWeaponSocketInfo >FirstWeaponUnit;

	TSharedPtr < FWeaponSocketInfo >SecondaryWeaponUnit;

	TMap<FGameplayTag, TSharedPtr<FSkillSocketInfo>>SkillsMap;

	TMap<FGameplayTag, TSharedPtr<FToolsSocketInfo>>ToolsMap;

	TArray<TSharedPtr<FCanbeActivedInfo>>CanbeActivedInfoAry;

};