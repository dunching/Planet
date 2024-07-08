#pragma once

#include <functional>
#include <set>

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "GAEvent_Helper.h"
#include "InteractiveComponent.h"

#include "InteractiveSkillComponent.generated.h"

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

UCLASS(BlueprintType, Blueprintable)
class ABILITYSYSTEM_API UInteractiveSkillComponent : public UInteractiveComponent
{
	GENERATED_BODY()

public:

	static FName ComponentName;

	using FOnActivedWeaponChangedContainer = TCallbackHandleContainer<void(EWeaponSocket)>;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason)override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)override;

	virtual TArray<TSharedPtr<FCanbeActivedInfo>> GetCanbeActiveAction()const override;

	virtual bool ActiveAction(
		const TSharedPtr<FCanbeActivedInfo>& CanbeActivedInfoSPtr, bool bIsAutomaticStop = false
	)override;

	virtual void CancelAction(const TSharedPtr<FCanbeActivedInfo>& CanbeActivedInfoSPtr)override;

#pragma region Skills
	void RegisterMultiGAs(
		const TMap<FGameplayTag, TSharedPtr<FSkillSocketInfo>>& InSkillsMap, bool bIsGenerationEvent = true
	);

	TSharedPtr<FSkillSocketInfo> FindSkill(const FGameplayTag& Tag);

	const TMap<FGameplayTag, TSharedPtr<FSkillSocketInfo>>& GetSkills()const;
#pragma endregion Skills

#pragma region Weapon
	void RegisterWeapon(
		const TSharedPtr<FWeaponSocketInfo>& FirstWeaponSocketInfo,
		const TSharedPtr<FWeaponSocketInfo>& SecondWeaponSocketInfo
	);

	void GetWeapon(
		TSharedPtr<FWeaponSocketInfo>& FirstWeaponSocketInfo,
		TSharedPtr<FWeaponSocketInfo>& SecondWeaponSocketInfo
	)const;

	bool SwitchWeapon();

	void RetractputWeapon();

	EWeaponSocket GetActivedWeaponType();

	TSharedPtr<FWeaponSocketInfo >GetActivedWeapon()const;
#pragma endregion Weapon

	void InitialBaseGAs();

	FOnActivedWeaponChangedContainer OnActivedWeaponChangedContainer;

protected:

	virtual void GenerationCanbeActiveEvent()override;

	void CancelSkill_WeaponActive(const TSharedPtr<FCanbeActivedInfo>& CanbeActivedInfoSPtr);

	bool ActiveSkill_WeaponActive(
		const TSharedPtr<FCanbeActivedInfo>& CanbeActivedInfoSPtr, bool bIsAutomaticStop = false
	);

	bool ActiveSkill_Active(
		const TSharedPtr<FCanbeActivedInfo>& CanbeActivedInfoSPtr, bool bIsAutomaticStop = false
	);

	bool ActiveWeapon(EWeaponSocket WeaponSocket);

	bool ActivedCorrespondingWeapon(USkill_Active_Base* GAPtr);

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Element Skills")
	TSubclassOf<USkill_Element_Gold>Skill_Element_GoldClass;

	AWeapon_Base* ActivedWeaponPtr = nullptr;

	EWeaponSocket CurrentActivedWeaponSocket = EWeaponSocket::kNone;

	TSharedPtr<FWeaponSocketInfo>FirstWeaponUnit;

	TSharedPtr<FWeaponSocketInfo>SecondaryWeaponUnit;

	TMap<FGameplayTag, TSharedPtr<FSkillSocketInfo>>SkillsMap;

	TArray<TSharedPtr<FCanbeActivedInfo>>CanbeActiveSkillsAry;

};