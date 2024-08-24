#pragma once

#include <functional>
#include <set>

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "GAEvent_Helper.h"
#include "InteractiveComponent.h"

#include "InteractiveSkillComponent.generated.h"

class UActiveSkillUnit;

struct FSkillSocketInfo
{
	FKey Key;

	FGameplayTag SkillSocket;

	USkillUnit* SkillUnitPtr = nullptr;
};

struct FWeaponSocketInfo
{
	FGameplayTag WeaponSocket;

	UWeaponUnit* WeaponUnitPtr = nullptr;

	FGameplayAbilitySpecHandle Handle;
};

UCLASS(BlueprintType, Blueprintable)
class UInteractiveSkillComponent : public UInteractiveComponent
{
	GENERATED_BODY()

public:

	static FName ComponentName;

	using FOnActivedWeaponChangedContainer = TCallbackHandleContainer<void(EWeaponSocket)>;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason)override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)override;

	virtual bool ActiveAction(
		const TSharedPtr<FCanbeInteractionInfo>& CanbeActivedInfoSPtr, bool bIsAutomaticStop = false
	)override;

	virtual void CancelAction(const TSharedPtr<FCanbeInteractionInfo>& CanbeActivedInfoSPtr)override;

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

	bool ActiveWeapon(EWeaponSocket WeaponSocket);

	bool SwitchWeapon();

	void RetractputWeapon();

	EWeaponSocket GetActivedWeaponType();

	TSharedPtr<FWeaponSocketInfo >GetActivedWeapon()const;
#pragma endregion Weapon

	void InitialBaseGAs();

	FOnActivedWeaponChangedContainer OnActivedWeaponChangedContainer;

protected:

	virtual void GenerationCanbeActiveEvent()override;

	void CancelSkill_WeaponActive(const TSharedPtr<FCanbeInteractionInfo>& CanbeActivedInfoSPtr);

	bool ActiveSkill_WeaponActive(
		const TSharedPtr<FCanbeInteractionInfo>& CanbeActivedInfoSPtr, bool bIsAutomaticStop = false
	);

	bool ActiveSkill_Active(
		const TSharedPtr<FCanbeInteractionInfo>& CanbeActivedInfoSPtr, bool bIsAutomaticStop = false
	);

	bool ActivedCorrespondingWeapon(UActiveSkillUnit* ActiveSkillUnitPtr);

	void RemoveSkill(const TMap<FGameplayTag, TSharedPtr<FSkillSocketInfo>>& InSkillsMap);

	void AddSkill(const TMap<FGameplayTag, TSharedPtr<FSkillSocketInfo>>& InSkillsMap);

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Element Skills")
	TSubclassOf<USkill_Element_Gold>Skill_Element_GoldClass;

	AWeapon_Base* ActivedWeaponPtr = nullptr;

	EWeaponSocket CurrentActivedWeaponSocket = EWeaponSocket::kNone;

	TSharedPtr<FWeaponSocketInfo>FirstWeaponUnit;

	TSharedPtr<FWeaponSocketInfo>SecondaryWeaponUnit;

	TMap<FGameplayTag, TSharedPtr<FSkillSocketInfo>>SkillsMap;

};