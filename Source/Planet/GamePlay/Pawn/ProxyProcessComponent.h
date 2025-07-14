#pragma once

#include <functional>
#include <set>

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "AllocationSkills.h"
#include "GroupManaggerInterface.h"
#include "ItemProxy_Character.h"

#include "ProxyProcessComponent.generated.h"

struct FActiveSkillProxy;
struct FPassiveSkillProxy;
struct FWeaponProxy;
struct FWeaponSkillProxy;
struct FConsumableProxy;

struct FWeaponSocket;
struct FCharacterSocket;
struct FCharacterSocket;

class ACharacterBase;

/*
 * 释放技能
 * 技能配置相关
 * 仅玩家
 */
UCLASS(BlueprintType, Blueprintable)
class PLANET_API UProxyProcessComponent :
	public UActorComponent,
	public IGroupManaggerInterface
{
	GENERATED_BODY()

public:
	friend FCharacterSocket;
	friend ACharacterBase;

	static FName ComponentName;

	using FOwnerType = ACharacterBase;

	using FOnCurrentWeaponChanged =
	TCallbackHandleContainer<void()>;

	using FOnCanAciveSkillChanged =
	TCallbackHandleContainer<void()>;

	using FOnCloseCombatChanged =
	TCallbackHandleContainer<void(
		ACharacterBase*,
		bool
		)>;

	UProxyProcessComponent(
		const FObjectInitializer& ObjectInitializer
		);

	virtual void InitializeComponent() override;

	virtual void BeginPlay() override;

	virtual void EndPlay(
		const EEndPlayReason::Type EndPlayReason
		) override;

	virtual void TickComponent(
		float DeltaTime,
		enum ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
		) override;

	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
		) const override;

	virtual void OnSelfGroupManaggerReady(
		AGroupManagger* NewGroupSharedInfoPtr
		) override;

	virtual void OnPlayerGroupManaggerReady(
		AGroupManagger* NewGroupSharedInfoPtr
		) override;

	bool ActiveAction(
		const FGameplayTag& CanbeActivedInfoSPtr,
		bool bIsAutomaticStop = false
		);

	virtual void CancelAction(
		const FGameplayTag& CanbeActivedInfoSPtr
		);

	TMap<FGameplayTag, FCharacterSocket> GetAllSocket() const;

	FCharacterSocket FindSocket(
		const FGameplayTag& SocketTag
		) const;

	TMap<FCharacterSocket, FKey> GetCanbeActiveSocket() const;

#pragma region Skills
	// 通过配置更新技能
	void UpdateCanbeActiveSkills();

	void UpdateCanbeActiveSkills_UsePassiveSocket(
		const TMap<FGameplayTag, FCharacterSocket>& CanActiveSocketMap
		);

	TSharedPtr<FActiveSkillProxy> FindActiveSkillBySocket(
		const FGameplayTag& SocketTag
		) const;

	TSharedPtr<FPassiveSkillProxy> FindPassiveSkillBySocket(
		const FGameplayTag& SocketTag
		) const;

	FCharacterSocket FindActiveSkillByType(
		const FGameplayTag& TypeTag
		) const;
#pragma endregion

#pragma region Weapon
	// 激活可用的武器
	void ActiveWeapon();

	/**
	 * 切換主副武器
	 * @return 是否切换成功
	 */
	bool SwitchWeapon();

	void RetractputWeapon();

	int32 GetCurrentWeaponAttackDistance() const;

	/**
	 * 是否是近战状态
	 * @return 
	 */
	bool GetIsCloseCombat() const;

	void GetWeaponSocket(
		FCharacterSocket& FirstWeaponSocketInfoSPtr,
		FCharacterSocket& SecondWeaponSocketInfoSPtr
		);

	void GetWeaponProxy(
		TSharedPtr<FWeaponProxy>& FirstWeaponProxySPtr,
		TSharedPtr<FWeaponProxy>& SecondWeaponProxySPtr
		);

	TSharedPtr<FWeaponSkillProxy> GetWeaponSkillByType(
		const FGameplayTag& TypeTag
		);

	TSharedPtr<FWeaponProxy> GetActivedWeapon() const;

	TSharedPtr<FWeaponProxy> FindWeaponSocket(
		const FGameplayTag& SocketTag
		) const;
#pragma endregion

#pragma region Consumables
	TSharedPtr<FConsumableProxy> FindConsumablesBySocket(
		const FGameplayTag& SocketTag
		) const;
#pragma endregion

	FOnCurrentWeaponChanged OnCurrentWeaponChanged;

	FOnCanAciveSkillChanged OnCanAciveSkillChanged;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentActivedSocketChanged)
	FCharacterSocket CurrentWeaponSocket;

	FOnCloseCombatChanged OnCloseCombatChanged;

protected:
	void Add(
		const FCharacterSocket& Socket
		);

	void Update(
		const FCharacterSocket& Socket
		);

	bool Active(
		const FCharacterSocket& Socket
		);

	bool Active(
		const FGameplayTag& Socket
		);

	void Cancel(
		const FCharacterSocket& Socket
		);

	void Cancel(
		const FGameplayTag& Socket
		);

	bool SwitchWeaponImpAndCheck(
		const FCharacterSocket& NewWeaponSocket
		);

	bool SwitchWeaponImp(
		const FCharacterSocket& NewWeaponSocket
		);

	bool ActivedCorrespondingWeapon(
		const FGameplayTag& ActiveSkillSocketTag
		);

#pragma region RPC
	UFUNCTION(Server, Reliable)
	void ActivedCorrespondingWeapon_Server(
		const FGameplayTag& ActiveSkillSocketTag
		);

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
	void OnRep_CurrentActivedSocketChanged();

	UFUNCTION()
	void OnRep_IsCloseCombat();
#pragma endregion

	void SetCloseCombat(
		bool bIsCloseCombat_
		);

	/**
	 * 是否处于近战状态
	 */
	UPROPERTY(ReplicatedUsing = OnRep_IsCloseCombat)
	bool bIsCloseCombat = false;
};
