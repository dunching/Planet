#include "ProxyProcessComponent.h"

#include "Net/UnrealNetwork.h"

#include "CharacterBase.h"
#include "GameplayTagsManager.h"
#include "ItemProxy.h"
#include "GameplayTagsLibrary.h"
#include "ItemProxy_Character.h"
#include "HoldingItemsComponent.h"

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
}

void UProxyProcessComponent::RetractputWeapon()
{
	SwitchWeaponImp(FGameplayTag::EmptyTag);
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
	FMySocket_FASI& FirstWeaponSocketInfoSPtr,
	FMySocket_FASI& SecondWeaponSocketInfoSPtr
)
{
	auto CharacterPtr = GetOwner<FOwnerType>();

	auto HoldingItemsComponentPtr = CharacterPtr->GetHoldingItemsComponent();
	auto OwnerCharacterProxyPtr = HoldingItemsComponentPtr->GetOwnerCharacterProxy();
	if (OwnerCharacterProxyPtr)
	{
		FirstWeaponSocketInfoSPtr = OwnerCharacterProxyPtr->FindSocket(UGameplayTagsLibrary::WeaponSocket_1);
		SecondWeaponSocketInfoSPtr = OwnerCharacterProxyPtr->FindSocket(UGameplayTagsLibrary::WeaponSocket_2);
	}
}

void UProxyProcessComponent::GetWeaponProxy(
	TSharedPtr<FWeaponProxy>& FirstWeaponProxySPtr,
	TSharedPtr<FWeaponProxy>& SecondWeaponProxySPtr
)
{
	auto CharacterPtr = GetOwner<FOwnerType>();

	auto HoldingItemsComponentPtr = CharacterPtr->GetHoldingItemsComponent();

	FMySocket_FASI FirstWeaponSocketInfoSPtr;
	FMySocket_FASI SecondWeaponSocketInfoSPtr;
	GetWeaponSocket(FirstWeaponSocketInfoSPtr, SecondWeaponSocketInfoSPtr);
	FirstWeaponProxySPtr = DynamicCastSharedPtr<FWeaponProxy>(
		HoldingItemsComponentPtr->FindProxy_BySocket(FirstWeaponSocketInfoSPtr));
	SecondWeaponProxySPtr = DynamicCastSharedPtr<FWeaponProxy>(
		HoldingItemsComponentPtr->FindProxy_BySocket(SecondWeaponSocketInfoSPtr));
}

TSharedPtr<FWeaponSkillProxy> UProxyProcessComponent::GetWeaponSkillByType(const FGameplayTag& TypeTag)
{
	// 找到当前装备的 弓箭类武器
	TSharedPtr<FWeaponSkillProxy> TargetSkillSPtr = nullptr;

	TSharedPtr<FWeaponProxy> FirstWeaponProxySPtr;
	TSharedPtr<FWeaponProxy> SecondWeaponProxySPtr;

	GetWeaponProxy(FirstWeaponProxySPtr, SecondWeaponProxySPtr);

	TSharedPtr<FWeaponSkillProxy> FirstWeaponSkillSPtr = FirstWeaponProxySPtr->GetWeaponSkill();
	TSharedPtr<FWeaponSkillProxy> SecondWeaponSkillSPtr = SecondWeaponProxySPtr->GetWeaponSkill();

	if (FirstWeaponSkillSPtr && FirstWeaponSkillSPtr->GetUnitType() == TypeTag)
	{
		TargetSkillSPtr = FirstWeaponSkillSPtr;
	}
	else if (SecondWeaponSkillSPtr && SecondWeaponSkillSPtr->GetUnitType() == TypeTag)
	{
		TargetSkillSPtr = SecondWeaponSkillSPtr;
	}
	return TargetSkillSPtr;
}

TSharedPtr<FWeaponProxy> UProxyProcessComponent::GetActivedWeapon() const
{
	return FindWeaponSocket(CurrentWeaponSocket);
}

TSharedPtr<FWeaponProxy> UProxyProcessComponent::FindWeaponSocket(const FGameplayTag& Tag) const
{
	auto CharacterPtr = GetOwner<FOwnerType>();

	auto HoldingItemsComponentPtr = CharacterPtr->GetHoldingItemsComponent();
	auto OwnerCharacterProxyPtr = HoldingItemsComponentPtr->GetOwnerCharacterProxy();
	if (OwnerCharacterProxyPtr)
	{
		auto Socket = OwnerCharacterProxyPtr->FindSocket(UGameplayTagsLibrary::WeaponSocket_1);
		return DynamicCastSharedPtr<FWeaponProxy>(HoldingItemsComponentPtr->FindProxy_BySocket(Socket));
	}

	return nullptr;
}

TMap<FKey, FMySocket_FASI> UProxyProcessComponent::GetCanbeActiveWeapon() const
{
	TMap<FKey, FMySocket_FASI> Result;

	return Result;
}

bool UProxyProcessComponent::ActiveAction(
	const FGameplayTag& CanbeActivedInfoSPtr,
	bool bIsAutomaticStop /*= false */
)
{
	return true;
}

void UProxyProcessComponent::CancelAction(const FGameplayTag& CanbeActivedInfoSPtr)
{
}

TMap<FKey, FMySocket_FASI> UProxyProcessComponent::GetCanbeActiveConsumable() const
{
	TMap<FKey, FMySocket_FASI> Result;

	return Result;
}

bool UProxyProcessComponent::ActivedCorrespondingWeapon(const FGameplayTag& ActiveSkillSocketTag)
{
	return false;
}

void UProxyProcessComponent::ActivedCorrespondingWeapon_Server_Implementation(const FGameplayTag& ActiveSkillSocketTag)
{
	ActivedCorrespondingWeapon(ActiveSkillSocketTag);
}

void UProxyProcessComponent::OnRep_AllocationChanged()
{
}

void UProxyProcessComponent::OnRep_CurrentActivedSocketChanged(const FGameplayTag& NewWeaponSocket)
{
	if (CurrentWeaponSocket == NewWeaponSocket)
	{
	}
	else
	{
		SwitchWeaponImp(NewWeaponSocket);
	}
}

void UProxyProcessComponent::SwitchWeaponImp(const FGameplayTag& NewWeaponSocket)
{
	if (NewWeaponSocket == CurrentWeaponSocket)
	{
	}
	else
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
}

TMap<FGameplayTag, FMySocket_FASI> UProxyProcessComponent::GetAllSocket() const
{
	TMap<FGameplayTag, FMySocket_FASI> Result;
	
	auto CharacterPtr = GetOwner<FOwnerType>();

	auto HoldingItemsComponentPtr = CharacterPtr->GetHoldingItemsComponent();
	auto OwnerCharacterProxyPtr = HoldingItemsComponentPtr->GetOwnerCharacterProxy();
	if (OwnerCharacterProxyPtr)
	{
		 Result= OwnerCharacterProxyPtr->TeammateConfigureMap;
	}
	return Result;
}

FMySocket_FASI UProxyProcessComponent::FindSocket(const FGameplayTag& Tag) const
{
	auto CharacterPtr = GetOwner<FOwnerType>();

	auto HoldingItemsComponentPtr = CharacterPtr->GetHoldingItemsComponent();

	return HoldingItemsComponentPtr->GetOwnerCharacterProxy()->FindSocket(Tag);
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
	const TMap<FGameplayTag, FMySocket_FASI>& InCanActiveSocketMap
)
{
	OnCanAciveSkillChanged();
}

TMap<FKey, FMySocket_FASI> UProxyProcessComponent::GetCanbeActiveSkills() const
{
	TMap<FKey, FMySocket_FASI> Result;

	return Result;
}

TSharedPtr<FActiveSkillProxy> UProxyProcessComponent::FindActiveSkillBySocket(const FGameplayTag& Tag) const
{
	return nullptr;
}

FMySocket_FASI UProxyProcessComponent::FindActiveSkillByType(const FGameplayTag& TypeTag) const
{
	auto CharacterPtr = GetOwner<FOwnerType>();

	auto HoldingItemsComponentPtr = CharacterPtr->GetHoldingItemsComponent();

	return HoldingItemsComponentPtr->GetOwnerCharacterProxy()->FindSocketByType(TypeTag);
}

void UProxyProcessComponent::Add(const FMySocket_FASI& Socket)
{
}

void UProxyProcessComponent::Update(const FMySocket_FASI& Socket)
{
}

bool UProxyProcessComponent::Active(const FMySocket_FASI& Socket)
{
	return Active(Socket.Socket);
}

bool UProxyProcessComponent::Active(const FGameplayTag& Socket)
{
	return false;
}

void UProxyProcessComponent::Cancel(const FMySocket_FASI& Socket)
{
	Cancel(Socket.Socket);
}

void UProxyProcessComponent::Cancel(const FGameplayTag& Socket)
{
}
