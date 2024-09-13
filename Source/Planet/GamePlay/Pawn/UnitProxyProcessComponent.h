#pragma once

#include <functional>
#include <set>

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "GAEvent_Helper.h"

#include "AllocationSkills.h"

#include "UnitProxyProcessComponent.generated.h"

struct FActiveSkillProxy;
struct FWeaponSocket;
struct FSocket_FASI;
class ACharacterBase;

/*
	释放技能
	技能配置相关
*/
UCLASS(BlueprintType, Blueprintable)
class UUnitProxyProcessComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	static FName ComponentName;

	using FOwnerType = ACharacterBase;

	using FOnAllocationChanged =
		TCallbackHandleContainer<void()>;

	UUnitProxyProcessComponent(const FObjectInitializer& ObjectInitializer);

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason)override;

	virtual void TickComponent(
		float DeltaTime,
		enum ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	)override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	bool ActiveAction(
		const TSharedPtr<FSocket_FASI>& CanbeActivedInfoSPtr,
		bool bIsAutomaticStop = false
	);

	virtual void CancelAction(
		const TSharedPtr<FSocket_FASI>& CanbeActivedInfoSPtr
	);

	TArray<TSharedPtr<FSocket_FASI>> GetCanbeActiveAction()const;

#pragma region Skills
	TSharedPtr<FActiveSkillProxy> FindActiveSkillSocket(const FGameplayTag& Tag)const;

	void RegisterMultiGAs(
		const TSharedPtr<FSocket_FASI>& SkillSocket
	);
#pragma endregion 

#pragma region Weapon
	void RegisterWeapon(
		const TSharedPtr<FSocket_FASI>& WeaponSocket
	);

	// 激活可用的武器
	void ActiveWeapon();

	// 切換主副武器
	void SwitchWeapon();

	void RetractputWeapon();

	int32 GetCurrentWeaponAttackDistance()const;

	void GetWeapon(
		TSharedPtr<FSocket_FASI>& FirstWeaponSocketInfoSPtr,
		TSharedPtr<FSocket_FASI>& SecondWeaponSocketInfoSPtr
	);

	TSharedPtr<FWeaponProxy> GetActivedWeapon()const;

	TSharedPtr<FWeaponProxy> FindWeaponSocket(const FGameplayTag& Tag)const;
#pragma endregion 

#pragma region Consumables
#pragma endregion 

	TMap<FGameplayTag, TSharedPtr<FSocket_FASI>> GetSkills()const;

	TSharedPtr<FSocket_FASI> FindSocket(const FGameplayTag& Tag)const;

	void Update(const TSharedPtr<FSocket_FASI>& Socket);

	bool Active(const TSharedPtr<FSocket_FASI>& Socket);

	bool Active(const FGameplayTag& Socket);

	void Cancel(const TSharedPtr<FSocket_FASI>& Socket);

	void Cancel(const FGameplayTag& Socket);

	FOnAllocationChanged OnAllocationChanged;

	UPROPERTY(ReplicatedUsing = OnRep_AllocationChanged)
	FAllocation_FASI_Container AllocationSkills_Container;
	
	UPROPERTY(ReplicatedUsing = OnRep_CurrentActivedSocketChanged)
	FGameplayTag CurrentWeaponSocket;

protected:
	
	UFUNCTION(Server, Reliable)
	void RegisterMultiGAs_Server(
		const FSocket_FASI& SkillSocket
	);
	
	UFUNCTION(Server, Reliable)
	void RegisterWeapon_Server(
		const FSocket_FASI& Socket
	);
	
	UFUNCTION(Server, Reliable)
	void ActiveWeapon_Server();

	UFUNCTION(Server, Reliable)
	void SwitchWeapon_Server();

	UFUNCTION(Server, Reliable)
	void ActiveAction_Server(
		const FSocket_FASI& Socket,
		bool bIsAutomaticStop
	);

	UFUNCTION(Server, Reliable)
	void CancelAction_Server(
		const FSocket_FASI& Socket
	);

	void SwitchWeaponImp(const FGameplayTag& NewWeaponSocket);

	bool ActivedCorrespondingWeapon(const TSharedPtr<FActiveSkillProxy>& ActiveSkillUnitPtr);
	
	UFUNCTION()
	void OnRep_AllocationChanged();

	UFUNCTION()
	void OnRep_CurrentActivedSocketChanged();

	TMap<FGameplayTag, TSharedPtr<FSocket_FASI>>SkillsMap;

};