#pragma once

#include <functional>
#include <set>

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "GAEvent_Helper.h"

#include "AllocationSkills.h"
#include "ItemProxy_Character.h"

#include "ProxyProcessComponent.generated.h"

struct FActiveSkillProxy;
struct FWeaponSocket;
struct FMySocket_FASI;
struct FMySocket_FASI;
class ACharacterBase;

/*
 * 释放技能
 * 技能配置相关
 * 仅玩家
 */
UCLASS(BlueprintType, Blueprintable)
class PLANET_API UProxyProcessComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	friend FMySocket_FASI;
	friend ACharacterBase;

	static FName ComponentName;

	using FOwnerType = ACharacterBase;

	using FOnCurrentWeaponChanged =
	TCallbackHandleContainer<void()>;

	using FOnCanAciveSkillChanged =
	TCallbackHandleContainer<void()>;

	UProxyProcessComponent(const FObjectInitializer& ObjectInitializer);

	virtual void InitializeComponent() override;

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void TickComponent(
		float DeltaTime,
		enum ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	bool ActiveAction(
		const FGameplayTag& CanbeActivedInfoSPtr,
		bool bIsAutomaticStop = false
	);

	virtual void CancelAction(
		const FGameplayTag& CanbeActivedInfoSPtr
	);

	TMap<FGameplayTag, FMySocket_FASI> GetAllSocket() const;

	FMySocket_FASI FindSocket(const FGameplayTag& SocketTag) const;

#pragma region Skills
	void UpdateCanbeActiveSkills();

	void UpdateCanbeActiveSkills_UsePassiveSocket(
		const TMap<FGameplayTag, FMySocket_FASI>& CanActiveSocketMap
	);

	TMap<FKey, FMySocket_FASI> GetCanbeActiveSkills() const;

	TSharedPtr<FActiveSkillProxy> FindActiveSkillBySocket(const FGameplayTag& SocketTag) const;

	FMySocket_FASI FindActiveSkillByType(const FGameplayTag& TypeTag) const;
#pragma endregion

#pragma region Weapon
	// 激活可用的武器
	void ActiveWeapon();

	// 切換主副武器
	void SwitchWeapon();

	void RetractputWeapon();

	int32 GetCurrentWeaponAttackDistance() const;

	void GetWeaponSocket(
		FMySocket_FASI& FirstWeaponSocketInfoSPtr,
		FMySocket_FASI& SecondWeaponSocketInfoSPtr
	);

	void GetWeaponProxy(
		TSharedPtr<FWeaponProxy>& FirstWeaponProxySPtr,
		TSharedPtr<FWeaponProxy>& SecondWeaponProxySPtr
	);

	TSharedPtr<FWeaponSkillProxy> GetWeaponSkillByType(
		const FGameplayTag& TypeTag
	);

	TSharedPtr<FWeaponProxy> GetActivedWeapon() const;

	TSharedPtr<FWeaponProxy> FindWeaponSocket(const FGameplayTag& Tag) const;

	TMap<FKey, FMySocket_FASI> GetCanbeActiveWeapon() const;
#pragma endregion

#pragma region Consumables
	TMap<FKey, FMySocket_FASI> GetCanbeActiveConsumable() const;
#pragma endregion

	FOnCurrentWeaponChanged OnCurrentWeaponChanged;

	FOnCanAciveSkillChanged OnCanAciveSkillChanged;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentActivedSocketChanged)
	FGameplayTag CurrentWeaponSocket;

protected:
	
	void Add(const FMySocket_FASI& Socket);

	void Update(const FMySocket_FASI& Socket);

	bool Active(const FMySocket_FASI& Socket);

	bool Active(const FGameplayTag& Socket);

	void Cancel(const FMySocket_FASI& Socket);

	void Cancel(const FGameplayTag& Socket);

	void SwitchWeaponImp(const FGameplayTag& NewWeaponSocket);

	bool ActivedCorrespondingWeapon(const FGameplayTag& ActiveSkillSocketTag);

#pragma region RPC
	UFUNCTION(Server, Reliable)
	void ActivedCorrespondingWeapon_Server(const FGameplayTag& ActiveSkillSocketTag);

	UFUNCTION(Server, Reliable)
	void ActiveWeapon_Server();

	void ActiveWeaponImp();

	UFUNCTION(Server, Reliable)
	void SwitchWeapon_Server();

	UFUNCTION(Server, Reliable)
	void RetractputWeapon_Server();

	UFUNCTION(Server, Reliable)
	void ActiveAction_Server(
		const FGameplayTag& SocketTag,
		bool bIsAutomaticStop
	);

	bool ActiveActionImp(
		const FGameplayTag& SocketTag,
		bool bIsAutomaticStop
	);

	UFUNCTION(Server, Reliable)
	void CancelAction_Server(
		const FGameplayTag& SocketTag
	);

	UFUNCTION()
	void OnRep_AllocationChanged();

	UFUNCTION()
	void OnRep_CurrentActivedSocketChanged(const FGameplayTag& NewWeaponSocket);
#pragma endregion
};
