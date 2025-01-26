#include "ProxyProcessComponent.h"

#include "Net/UnrealNetwork.h"

#include "CharacterBase.h"
#include "GameOptions.h"
#include "GameplayTagsManager.h"
#include "ItemProxy_Minimal.h"
#include "GameplayTagsLibrary.h"
#include "ItemProxy_Character.h"
#include "InventoryComponent.h"

FName UProxyProcessComponent::ComponentName = TEXT("ProxyProcessComponent");

UProxyProcessComponent::UProxyProcessComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;

	SetIsReplicatedByDefault(true);
}

void UProxyProcessComponent::InitializeComponent()
{
	Super::InitializeComponent();

	UWorld* World = GetWorld();
	if (World->IsGameWorld())
	{
	}
}

void UProxyProcessComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UProxyProcessComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UProxyProcessComponent::TickComponent(
	float DeltaTime,
	enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction
)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UProxyProcessComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;

	DOREPLIFETIME_CONDITION(ThisClass, CurrentWeaponSocket, COND_None);
}

void UProxyProcessComponent::ActiveWeapon_Server_Implementation()
{
	ActiveWeaponImp();
}

void UProxyProcessComponent::ActiveWeaponImp()
{
	auto CharacterPtr = GetOwner<FOwnerType>();

	const auto CharacterProxySPtr = CharacterPtr->GetCharacterProxy();

	const auto WeaponSocket_1 = CharacterProxySPtr->FindSocket(UGameplayTagsLibrary::WeaponSocket_1);
	const auto WeaponSocket_2 = CharacterProxySPtr->FindSocket(UGameplayTagsLibrary::WeaponSocket_2);

	if (WeaponSocket_1.AllocationedProxyID.IsValid())
	{
		SwitchWeaponImpAndCheck(UGameplayTagsLibrary::WeaponSocket_1);
	}
	else if (WeaponSocket_2.AllocationedProxyID.IsValid())
	{
		SwitchWeaponImpAndCheck(UGameplayTagsLibrary::WeaponSocket_2);
	}
}

void UProxyProcessComponent::SwitchWeapon_Server_Implementation()
{
	SwitchWeapon();
}

void UProxyProcessComponent::RetractputWeapon_Server_Implementation()
{
	RetractputWeapon();
}

void UProxyProcessComponent::ActiveAction_Server_Implementation(
	const FGameplayTag& SocketTag,
	bool bIsAutomaticStop
)
{
	Active(SocketTag);
}

bool UProxyProcessComponent::ActiveActionImp(
	const FGameplayTag& SocketTag,
	bool bIsAutomaticStop
)
{
	auto CharacterPtr = GetOwner<FOwnerType>();

	const auto InventoryComponentPtr = CharacterPtr->GetInventoryComponent();
	const auto CharacterProxySPtr = CharacterPtr->GetCharacterProxy();

	const auto CanActiveSocketMap = CharacterProxySPtr->GetSockets();
	if (CanActiveSocketMap.Contains(SocketTag))
	{
		auto ProxySPtr = InventoryComponentPtr->FindProxy(CanActiveSocketMap[SocketTag].AllocationedProxyID);
		if (ProxySPtr)
		{
			return ProxySPtr->Active();
		}
	}

	return false;
}

void UProxyProcessComponent::CancelAction_Server_Implementation(
	const FGameplayTag& SocketTag
)
{
	Cancel(SocketTag);
}

void UProxyProcessComponent::ActiveWeapon()
{
#if UE_EDITOR || UE_CLIENT
	if (GetNetMode() == NM_Client)
	{
		ActiveWeapon_Server();
		ActiveWeaponImp();
	}
#endif

#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		ActiveWeaponImp();
	}
#endif
}

void UProxyProcessComponent::SwitchWeapon()
{
	auto CharacterPtr = GetOwner<FOwnerType>();

	const auto CharacterProxySPtr = CharacterPtr->GetCharacterProxy();

	const auto WeaponSocket_1 = CharacterProxySPtr->FindSocket(UGameplayTagsLibrary::WeaponSocket_1);
	const auto WeaponSocket_2 = CharacterProxySPtr->FindSocket(UGameplayTagsLibrary::WeaponSocket_2);

	if (WeaponSocket_1.Socket == CurrentWeaponSocket)
	{
		SwitchWeaponImpAndCheck(UGameplayTagsLibrary::WeaponSocket_2);
	}
	else
	{
		SwitchWeaponImpAndCheck(UGameplayTagsLibrary::WeaponSocket_1);
	}

#if UE_EDITOR || UE_CLIENT
	if (GetNetMode() == NM_Client)
	{
		SwitchWeapon_Server();
	}
#endif
}

void UProxyProcessComponent::RetractputWeapon()
{
	SwitchWeaponImpAndCheck(FGameplayTag::EmptyTag);
#if UE_EDITOR || UE_CLIENT
	if (GetNetMode() == NM_Client)
	{
		RetractputWeapon_Server();
	}
#endif
}

int32 UProxyProcessComponent::GetCurrentWeaponAttackDistance() const
{
	auto SPtr = GetActivedWeapon();
	if (SPtr)
	{
		return SPtr->GetMaxAttackDistance();
	}

	return 100;
}

void UProxyProcessComponent::GetWeaponSocket(
	FCharacterSocket& FirstWeaponSocketInfoSPtr,
	FCharacterSocket& SecondWeaponSocketInfoSPtr
)
{
	auto CharacterPtr = GetOwner<FOwnerType>();

	if (CharacterPtr)
	{
		CharacterPtr->GetCharacterProxy()->GetWeaponSocket(FirstWeaponSocketInfoSPtr, SecondWeaponSocketInfoSPtr);
	}
}

void UProxyProcessComponent::GetWeaponProxy(
	TSharedPtr<FWeaponProxy>& FirstWeaponProxySPtr,
	TSharedPtr<FWeaponProxy>& SecondWeaponProxySPtr
)
{
	auto CharacterPtr = GetOwner<FOwnerType>();

	auto InventoryComponentPtr = CharacterPtr->GetInventoryComponent();

	FCharacterSocket FirstWeaponSocketInfoSPtr;
	FCharacterSocket SecondWeaponSocketInfoSPtr;
	GetWeaponSocket(FirstWeaponSocketInfoSPtr, SecondWeaponSocketInfoSPtr);
	FirstWeaponProxySPtr = DynamicCastSharedPtr<FWeaponProxy>(
		InventoryComponentPtr->FindProxy_BySocket(FirstWeaponSocketInfoSPtr));
	SecondWeaponProxySPtr = DynamicCastSharedPtr<FWeaponProxy>(
		InventoryComponentPtr->FindProxy_BySocket(SecondWeaponSocketInfoSPtr));
}

TSharedPtr<FWeaponSkillProxy> UProxyProcessComponent::GetWeaponSkillByType(const FGameplayTag& TypeTag)
{
	TSharedPtr<FWeaponProxy> FirstWeaponProxySPtr;
	TSharedPtr<FWeaponProxy> SecondWeaponProxySPtr;

	GetWeaponProxy(FirstWeaponProxySPtr, SecondWeaponProxySPtr);

	// 找到当前装备的 弓箭类武器
	auto WeaponSkillSPtr = FirstWeaponProxySPtr->GetWeaponSkill();

	if (WeaponSkillSPtr && WeaponSkillSPtr->GetProxyType() == TypeTag)
	{
		return WeaponSkillSPtr;
	}
	
	WeaponSkillSPtr = SecondWeaponProxySPtr->GetWeaponSkill();

	if (WeaponSkillSPtr && WeaponSkillSPtr->GetProxyType() == TypeTag)
	{
		return WeaponSkillSPtr;
	}
	return nullptr;
}

TSharedPtr<FWeaponProxy> UProxyProcessComponent::GetActivedWeapon() const
{
	return FindWeaponSocket(CurrentWeaponSocket);
}

TSharedPtr<FWeaponProxy> UProxyProcessComponent::FindWeaponSocket(const FGameplayTag& SocketTag) const
{
	auto CharacterPtr = GetOwner<FOwnerType>();

	auto InventoryComponentPtr = CharacterPtr->GetInventoryComponent();
	auto OwnerCharacterProxyPtr = CharacterPtr->GetCharacterProxy();
	if (OwnerCharacterProxyPtr)
	{
		auto Socket = OwnerCharacterProxyPtr->FindSocket(SocketTag);
		return DynamicCastSharedPtr<FWeaponProxy>(InventoryComponentPtr->FindProxy_BySocket(Socket));
	}

	return nullptr;
}

TSharedPtr<FConsumableProxy> UProxyProcessComponent::FindConsumablesBySocket(const FGameplayTag& SocketTag) const
{
	auto CharacterPtr = GetOwner<FOwnerType>();
	const auto ActionKeyMap = UGameOptions::GetInstance()->ActionKeyMap;

	const auto Sockets = CharacterPtr->GetCharacterProxy()->FindSocket(SocketTag);

	auto SkillProxySPtr = CharacterPtr->GetInventoryComponent()->FindProxy_Consumable(Sockets.AllocationedProxyID);
	if (SkillProxySPtr)
	{
		return DynamicCastSharedPtr<FConsumableProxy>(SkillProxySPtr);
	}
	return nullptr;
}

bool UProxyProcessComponent::ActiveAction(
	const FGameplayTag& CanbeActivedInfoSPtr,
	bool bIsAutomaticStop /*= false */
)
{
	// 使用武器
	if (CanbeActivedInfoSPtr.MatchesTag(UGameplayTagsLibrary::WeaponSocket))
	{
		ActiveAction_Server(CurrentWeaponSocket, bIsAutomaticStop);
	}
	// 使用主动技能
	else if (CanbeActivedInfoSPtr.MatchesTag(UGameplayTagsLibrary::ActiveSocket))
	{
		if (ActivedCorrespondingWeapon(CanbeActivedInfoSPtr))
		{
			ActiveAction_Server(CanbeActivedInfoSPtr, bIsAutomaticStop);
		}
	}
	// 使用消耗品
	else if (CanbeActivedInfoSPtr.MatchesTag(UGameplayTagsLibrary::ConsumableSocket))
	{
		ActiveAction_Server(CanbeActivedInfoSPtr, bIsAutomaticStop);
	}

	return true;
}

void UProxyProcessComponent::CancelAction(const FGameplayTag& CanbeActivedInfoSPtr)
{
	// 使用武器
	if (CanbeActivedInfoSPtr.MatchesTag(UGameplayTagsLibrary::WeaponSocket))
	{
		CancelAction_Server(CurrentWeaponSocket);
	}
	// 使用主动技能
	else if (CanbeActivedInfoSPtr.MatchesTag(UGameplayTagsLibrary::ActiveSocket))
	{
		if (ActivedCorrespondingWeapon(CanbeActivedInfoSPtr))
		{
			CancelAction_Server(CanbeActivedInfoSPtr);
		}
	}
	// 使用消耗品
	else if (CanbeActivedInfoSPtr.MatchesTag(UGameplayTagsLibrary::ConsumableSocket))
	{
		CancelAction_Server(CanbeActivedInfoSPtr);
	}
}

bool UProxyProcessComponent::ActivedCorrespondingWeapon(const FGameplayTag& ActiveSkillSocketTag)
{
	return true;
}

void UProxyProcessComponent::ActivedCorrespondingWeapon_Server_Implementation(const FGameplayTag& ActiveSkillSocketTag)
{
	ActivedCorrespondingWeapon(ActiveSkillSocketTag);
}

void UProxyProcessComponent::OnRep_AllocationChanged()
{
}

void UProxyProcessComponent::OnRep_CurrentActivedSocketChanged(const FGameplayTag& OldWeaponSocket)
{
	SwitchWeaponImp(CurrentWeaponSocket);
}

void UProxyProcessComponent::SwitchWeaponImpAndCheck(const FGameplayTag& NewWeaponSocket)
{
	if (NewWeaponSocket == CurrentWeaponSocket)
	{
	}
	else
	{
		SwitchWeaponImp(NewWeaponSocket);
	}
}

void UProxyProcessComponent::SwitchWeaponImp(const FGameplayTag& NewWeaponSocket)
{
	auto PreviousWeaponSocketSPtr = FindWeaponSocket(CurrentWeaponSocket);
	if (PreviousWeaponSocketSPtr)
	{
#if UE_EDITOR || UE_SERVER
		if (GetNetMode() == NM_DedicatedServer)
		{
			PreviousWeaponSocketSPtr->RetractputWeapon();
			PreviousWeaponSocketSPtr->Update2Client();
		}
#endif
	}

	auto NewWeaponSocketSPtr = FindWeaponSocket(NewWeaponSocket);
	if (NewWeaponSocketSPtr)
	{
#if UE_EDITOR || UE_SERVER
		if (GetNetMode() == NM_DedicatedServer)
		{
			NewWeaponSocketSPtr->ActiveWeapon();
			NewWeaponSocketSPtr->Update2Client();
		}
#endif
	}

	CurrentWeaponSocket = NewWeaponSocket;

	OnCurrentWeaponChanged();
}

TMap<FGameplayTag, FCharacterSocket> UProxyProcessComponent::GetAllSocket() const
{
	TMap<FGameplayTag, FCharacterSocket> Result;

	auto CharacterPtr = GetOwner<FOwnerType>();

	return  CharacterPtr->GetCharacterProxy()->GetSockets();
}

FCharacterSocket UProxyProcessComponent::FindSocket(const FGameplayTag& Tag) const
{
	auto CharacterPtr = GetOwner<FOwnerType>();

	return CharacterPtr->GetCharacterProxy()->FindSocket(Tag);
}

void UProxyProcessComponent::UpdateCanbeActiveSkills()
{
	struct FMyStruct
	{
		FGameplayTag ActiveSocketTag;
	};
	TArray<FGameplayTag> Ary
	{
		UGameplayTagsLibrary::ActiveSocket_1,
		UGameplayTagsLibrary::ActiveSocket_2,
		UGameplayTagsLibrary::ActiveSocket_3,
		UGameplayTagsLibrary::ActiveSocket_4,
	};
}

void UProxyProcessComponent::UpdateCanbeActiveSkills_UsePassiveSocket(
	const TMap<FGameplayTag, FCharacterSocket>& InCanActiveSocketMap
)
{
	OnCanAciveSkillChanged();
}

TMap<FCharacterSocket, FKey> UProxyProcessComponent::GetCanbeActiveSocket() const
{
	TMap<FCharacterSocket, FKey> Result;

	auto CharacterPtr = GetOwner<FOwnerType>();
	const auto ActionKeyMap = UGameOptions::GetInstance()->ActionKeyMap;

	const auto Sockets = CharacterPtr->GetCharacterProxy()->GetSockets();

	for (const auto& Iter : Sockets)
	{
		if (ActionKeyMap.Contains(Iter.Key) && Iter.Value.AllocationedProxyID.IsValid())
		{
			Result.Add(Iter.Value,ActionKeyMap[Iter.Key]);
		}
	}

	return Result;
}

TSharedPtr<FActiveSkillProxy> UProxyProcessComponent::FindActiveSkillBySocket(const FGameplayTag& SocketTag) const
{
	auto CharacterPtr = GetOwner<FOwnerType>();
	const auto ActionKeyMap = UGameOptions::GetInstance()->ActionKeyMap;

	const auto Sockets = CharacterPtr->GetCharacterProxy()->FindSocket(SocketTag);

	auto SkillProxySPtr = CharacterPtr->GetInventoryComponent()->FindProxy_Skill(Sockets.AllocationedProxyID);
	if (SkillProxySPtr)
	{
		return DynamicCastSharedPtr<FActiveSkillProxy>(SkillProxySPtr);
	}
	return nullptr;
}

FCharacterSocket UProxyProcessComponent::FindActiveSkillByType(const FGameplayTag& TypeTag) const
{
	auto CharacterPtr = GetOwner<FOwnerType>();

	return CharacterPtr->GetCharacterProxy()->FindSocketByType(TypeTag);
}

void UProxyProcessComponent::Add(const FCharacterSocket& Socket)
{
}

void UProxyProcessComponent::Update(const FCharacterSocket& Socket)
{
}

bool UProxyProcessComponent::Active(const FCharacterSocket& Socket)
{
	return Active(Socket.Socket);
}

bool UProxyProcessComponent::Active(const FGameplayTag& Socket)
{
	return ActiveActionImp(Socket, true);
}

void UProxyProcessComponent::Cancel(const FCharacterSocket& Socket)
{
	Cancel(Socket.Socket);
}

void UProxyProcessComponent::Cancel(const FGameplayTag& SocketTag)
{
	auto CharacterPtr = GetOwner<FOwnerType>();

	const auto InventoryComponentPtr = CharacterPtr->GetInventoryComponent();
	const auto CharacterProxySPtr = CharacterPtr->GetCharacterProxy();

	const auto CanActiveSocketMap = CharacterProxySPtr->GetSockets();
	if (CanActiveSocketMap.Contains(SocketTag))
	{
		auto ProxySPtr = InventoryComponentPtr->FindProxy(CanActiveSocketMap[SocketTag].AllocationedProxyID);
		if (ProxySPtr)
		{
			ProxySPtr->Cancel();
		}
	}
}
