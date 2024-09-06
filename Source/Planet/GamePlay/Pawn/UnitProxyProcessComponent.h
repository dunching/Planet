#pragma once

#include <functional>
#include <set>

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "GAEvent_Helper.h"

#include "AllocationSkills.h"

#include "UnitProxyProcessComponent.generated.h"

struct FActiveSkillProxy;
struct FSkillSocket;
struct FWeaponSocket;
class ACharacterBase;

UCLASS(BlueprintType, Blueprintable)
class UUnitProxyProcessComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	static FName ComponentName;

	using FOwnerPawnType = ACharacterBase;

	using FOnActivedWeaponChangedContainer =
		TCallbackHandleContainer<void(const TSharedPtr<FWeaponSocket>&)>;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason)override;

	virtual void TickComponent(
		float DeltaTime,
		enum ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	)override;

	virtual bool ActiveAction(
		const TSharedPtr<FSocketBase>& CanbeActivedInfoSPtr,
		bool bIsAutomaticStop = false
	);

	virtual void CancelAction(
		const TSharedPtr<FSocketBase>& CanbeActivedInfoSPtr
	);

	TArray<TSharedPtr<FSocketBase>> GetCanbeActiveAction()const;

#pragma region Skills
	void RegisterMultiGAs(
		const TSharedPtr<FSkillSocket>& SkillSocket
	);

	TSharedPtr<FSkillSocket> FindSkill(const FGameplayTag& Tag);

	TMap<FGameplayTag, TSharedPtr<FSkillSocket>> GetSkills()const;
#pragma endregion 

#pragma region Weapon
	void RegisterWeapon(
		const TSharedPtr<FWeaponSocket>& WeaponSocket
	);

	// 激活可用的武器
	void ActiveWeapon();

	bool SwitchWeapon();

	void RetractputWeapon();

	int32 GetCurrentWeaponAttackDistance()const;

	void GetWeapon(
		TSharedPtr<FWeaponSocket>& FirstWeaponSocketInfoSPtr,
		TSharedPtr<FWeaponSocket>& SecondWeaponSocketInfoSPtr
	);

	TSharedPtr<FWeaponProxy>GetActivedWeapon()const;
#pragma endregion 

#pragma region Consumables
#pragma endregion 

	TSharedPtr<FAllocationSkills> GetAllocationSkills()const;

	FOnActivedWeaponChangedContainer OnActivedWeaponChangedContainer;

protected:

	bool ActivedCorrespondingWeapon(const TSharedPtr<FActiveSkillProxy>& ActiveSkillUnitPtr);

	void SwitchWeaponImp(const TSharedPtr<FWeaponSocket>& NewWeaponSocketSPtr);

	TSharedPtr<FWeaponSocket>PreviousWeaponSocketSPtr = nullptr;

};