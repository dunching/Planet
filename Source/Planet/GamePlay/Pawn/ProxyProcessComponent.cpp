#include "ProxyProcessComponent.h"

#include <queue>
#include <map>

#include "GameplayAbilitySpec.h"
#include "Net/UnrealNetwork.h"


#include "GAEvent_Helper.h"
#include "CharacterBase.h"
#include "CharacterAttributesComponent.h"
#include "GenerateType.h"
#include "PlanetPlayerState.h"
#include "BasicFuturesBase.h"
#include "GAEvent_Send.h"
#include "GAEvent_Received.h"
#include "ItemProxy.h"
#include "Weapon_Base.h"
#include "Skill_Base.h"
#include "Skill_Active_Base.h"
#include "Skill_WeaponActive_PickAxe.h"
#include "Skill_WeaponActive_HandProtection.h"
#include "Skill_WeaponActive_RangeTest.h"
#include "Weapon_HandProtection.h"
#include "Weapon_PickAxe.h"
#include "Weapon_RangeTest.h"
#include "AssetRefMap.h"
#include "Skill_Talent_NuQi.h"
#include "Skill_Talent_YinYang.h"
#include "Skill_Element_Gold.h"
#include "InputProcessorSubSystem.h"
#include "Tool_PickAxe.h"
#include "HumanRegularProcessor.h"
#include "HumanCharacter.h"
#include "GameplayTagsSubSystem.h"
#include "SceneUnitTable.h"
#include "Skill_Active_Control.h"
#include "BaseFeatureComponent.h"
#include "Skill_WeaponActive_Base.h"
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
		AllocationSkills_Container.CharacterPtr = GetOwner<ACharacterBase>();
	}
}

void UProxyProcessComponent::BeginPlay()
{
	Super::BeginPlay();

#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		struct FMyStruct
		{
			FGameplayTag SocketTag;
			FKey Key;
		};
		TArray<FMyStruct> Ary
		{
			{UGameplayTagsSubSystem::ActiveSocket_1, EKeys::Q},
			{UGameplayTagsSubSystem::ActiveSocket_2, EKeys::E},
			{UGameplayTagsSubSystem::ActiveSocket_3, EKeys::R},
			{UGameplayTagsSubSystem::ActiveSocket_4, EKeys::F},

			{UGameplayTagsSubSystem::WeaponSocket_1, EKeys::LeftMouseButton},
			{UGameplayTagsSubSystem::WeaponSocket_2, EKeys::LeftMouseButton},

			{UGameplayTagsSubSystem::WeaponActiveSocket_1, EKeys::LeftMouseButton},
			{UGameplayTagsSubSystem::WeaponActiveSocket_2, EKeys::LeftMouseButton},

			{UGameplayTagsSubSystem::ConsumableSocket1, EKeys::One},
			{UGameplayTagsSubSystem::ConsumableSocket2, EKeys::Two},
			{UGameplayTagsSubSystem::ConsumableSocket3, EKeys::Three},
			{UGameplayTagsSubSystem::ConsumableSocket4, EKeys::Four},

			{UGameplayTagsSubSystem::PassiveSocket_1, EKeys::Invalid},
			{UGameplayTagsSubSystem::PassiveSocket_2, EKeys::Invalid},
			{UGameplayTagsSubSystem::PassiveSocket_3, EKeys::Invalid},
			{UGameplayTagsSubSystem::PassiveSocket_4, EKeys::Invalid},
			{UGameplayTagsSubSystem::PassiveSocket_5, EKeys::Invalid},
		};

		for (const auto& Iter : Ary)
		{
			auto SocketSPtr = MakeShared<FSocket_FASI>();

			SocketSPtr->Key = Iter.Key;
			SocketSPtr->Socket = Iter.SocketTag;

			SocketMap.Add(SocketSPtr->Socket, SocketSPtr);
			AllocationSkills_Container.AddItem(SocketSPtr);
		}
	}
#endif
}

void UProxyProcessComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UProxyProcessComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UProxyProcessComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;

	DOREPLIFETIME_CONDITION(ThisClass, AllocationSkills_Container, COND_None);
	DOREPLIFETIME_CONDITION(ThisClass, CurrentWeaponSocket, COND_None);
}

void UProxyProcessComponent::UpdateSocket(
	const TSharedPtr<FSocket_FASI>& SkillsSocketInfo
)
{
	if (SkillsSocketInfo->ProxySPtr)
	{
		SkillsSocketInfo->SkillProxyID = SkillsSocketInfo->ProxySPtr->GetID();
	}
	else
	{
		SkillsSocketInfo->SkillProxyID = FGuid();
	}

#if UE_EDITOR || UE_CLIENT
	if (GetNetMode() == NM_Client)
	{
		UpdateSocket_Server(*SkillsSocketInfo);
	}
#endif

	Update(SkillsSocketInfo);
}

void UProxyProcessComponent::UpdateSocket_Server_Implementation(const FSocket_FASI& Socket)
{
	auto SkillsSocketInfo = MakeShared<FSocket_FASI>();
	*SkillsSocketInfo = Socket;

	SkillsSocketInfo->ProxySPtr = AllocationSkills_Container.CharacterPtr->GetHoldingItemsComponent()->FindProxy(
		Socket.SkillProxyID);

	Update(SkillsSocketInfo);
	// 	if (SocketMap.Contains(Socket.Socket))
	// 	{
	// 		SocketMap[Socket.Socket]->ProxySPtr = AllocationSkills_Container.HoldingItemsComponentPtr->FindProxy(Socket.SkillProxyID);
	// 
	// 		Update(SocketMap[Socket.Socket]);
	// 	}
}

void UProxyProcessComponent::ActiveWeapon_Server_Implementation()
{
	ActiveWeaponImp();
}

void UProxyProcessComponent::ActiveWeaponImp()
{
#if UE_EDITOR || UE_SERVER
	if (GetOwnerRole() == ROLE_Authority)
	{
	}
#endif

#if UE_EDITOR || UE_CLIENT
	if (GetOwnerRole() == ROLE_AutonomousProxy)
	{
	}
#endif

	const auto WeaponsMap = SocketMap;

	if (WeaponsMap.Contains(UGameplayTagsSubSystem::WeaponSocket_1))
	{
		auto WeaponSocketSPtr = WeaponsMap[UGameplayTagsSubSystem::WeaponSocket_1];
		if (WeaponSocketSPtr->SkillProxyID.IsValid())
		{
			SwitchWeaponImp(WeaponSocketSPtr->Socket);
			return;
		}
	}

	if (WeaponsMap.Contains(UGameplayTagsSubSystem::WeaponSocket_2))
	{
		auto WeaponSocketSPtr = WeaponsMap[UGameplayTagsSubSystem::WeaponSocket_2];
		if (WeaponSocketSPtr->SkillProxyID.IsValid())
		{
			SwitchWeaponImp(WeaponSocketSPtr->Socket);
			return;
		}
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
	if (CanActiveSocketMap.Contains(SocketTag))
	{
		if (CanActiveSocketMap[SocketTag]->ProxySPtr)
		{
			if (CanActiveSocketMap[SocketTag]->ProxySPtr->CanActive())
			{
				ActiveAction_Server(SocketTag, bIsAutomaticStop);
				return true;
			}
		}
	}
	else if (SocketMap.Contains(SocketTag))
	{
		if (SocketMap[SocketTag]->ProxySPtr)
		{
			if (SocketMap[SocketTag]->ProxySPtr->CanActive())
			{
				ActiveAction_Server(SocketTag, bIsAutomaticStop);
				return true;
			}
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
	const auto WeaponsMap = SocketMap;
	if (
		(WeaponsMap.Contains(UGameplayTagsSubSystem::WeaponSocket_1)) ||
		(WeaponsMap.Contains(UGameplayTagsSubSystem::WeaponSocket_2))
	)
	{
		if (UGameplayTagsSubSystem::WeaponSocket_1 == CurrentWeaponSocket)
		{
			SwitchWeaponImp(UGameplayTagsSubSystem::WeaponSocket_2);
		}
		else
		{
			SwitchWeaponImp(UGameplayTagsSubSystem::WeaponSocket_1);
		}
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
	TSharedPtr<FSocket_FASI>& FirstWeaponSocketInfoSPtr,
	TSharedPtr<FSocket_FASI>& SecondWeaponSocketInfoSPtr
)
{
	const auto WeaponsMap = SocketMap;
	if (WeaponsMap.Contains(UGameplayTagsSubSystem::WeaponSocket_1))
	{
		FirstWeaponSocketInfoSPtr = WeaponsMap[UGameplayTagsSubSystem::WeaponSocket_1];
	}
	if (WeaponsMap.Contains(UGameplayTagsSubSystem::WeaponSocket_2))
	{
		SecondWeaponSocketInfoSPtr = WeaponsMap[UGameplayTagsSubSystem::WeaponSocket_2];
	}
}

void UProxyProcessComponent::GetWeaponProxy(
	TSharedPtr<FWeaponProxy>& FirstWeaponProxySPtr,
	TSharedPtr<FWeaponProxy>& SecondWeaponProxySPtr
)
{
	TSharedPtr<FSocket_FASI> FirstWeaponSocketInfoSPtr;
	TSharedPtr<FSocket_FASI> SecondWeaponSocketInfoSPtr;
	GetWeaponSocket(FirstWeaponSocketInfoSPtr, SecondWeaponSocketInfoSPtr);
	if (FirstWeaponSocketInfoSPtr && FirstWeaponSocketInfoSPtr->ProxySPtr)
	{
		FirstWeaponProxySPtr = DynamicCastSharedPtr<FWeaponProxy>(FirstWeaponSocketInfoSPtr->ProxySPtr);
	}
	if (SecondWeaponSocketInfoSPtr && SecondWeaponSocketInfoSPtr->ProxySPtr)
	{
		SecondWeaponProxySPtr = DynamicCastSharedPtr<FWeaponProxy>(SecondWeaponSocketInfoSPtr->ProxySPtr);
	}
}

void UProxyProcessComponent::GetWeaponSkills(
	TSharedPtr<FWeaponSkillProxy>& FirstWeaponSkillSPtr,
	TSharedPtr<FWeaponSkillProxy>& SecondWeaponSkillSPtr
)
{
	TSharedPtr<FSocket_FASI> FirstWeaponSocketInfoSPtr;
	TSharedPtr<FSocket_FASI> SecondWeaponSocketInfoSPtr;
	GetWeaponSocket(FirstWeaponSocketInfoSPtr, SecondWeaponSocketInfoSPtr);
	if (FirstWeaponSocketInfoSPtr && FirstWeaponSocketInfoSPtr->ProxySPtr)
	{
		auto WeaponSPtr = DynamicCastSharedPtr<FWeaponProxy>(FirstWeaponSocketInfoSPtr->ProxySPtr);
		FirstWeaponSkillSPtr = DynamicCastSharedPtr<FWeaponSkillProxy>(WeaponSPtr->GetWeaponSkill());
	}
	if (SecondWeaponSocketInfoSPtr && SecondWeaponSocketInfoSPtr->ProxySPtr)
	{
		auto WeaponSPtr = DynamicCastSharedPtr<FWeaponProxy>(SecondWeaponSocketInfoSPtr->ProxySPtr);
		SecondWeaponSkillSPtr = DynamicCastSharedPtr<FWeaponSkillProxy>(WeaponSPtr->GetWeaponSkill());
	}
}

TSharedPtr<FWeaponSkillProxy> UProxyProcessComponent::GetWeaponSkillByType(const FGameplayTag& TypeTag)
{
	// 找到当前装备的 弓箭类武器
	TSharedPtr<FWeaponSkillProxy> TargetSkillSPtr = nullptr;

	TSharedPtr<FWeaponSkillProxy> FirstWeaponSkillSPtr;
	TSharedPtr<FWeaponSkillProxy> SecondWeaponSkillSPtr;
	GetWeaponSkills(FirstWeaponSkillSPtr, SecondWeaponSkillSPtr);
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
	if (SocketMap.Contains(Tag))
	{
		return DynamicCastSharedPtr<FWeaponProxy>(SocketMap[Tag]->ProxySPtr);
	}

	return nullptr;
}

TSharedPtr<FWeaponSkillProxy> UProxyProcessComponent::FindWeaponSkillSocket(const FGameplayTag& Tag) const
{
	if (SocketMap.Contains(Tag))
	{
		return DynamicCastSharedPtr<FWeaponSkillProxy>(SocketMap[Tag]->ProxySPtr);
	}

	return nullptr;
}

TArray<TSharedPtr<FSocket_FASI>> UProxyProcessComponent::GetCanbeActiveWeapon() const
{
	TArray<TSharedPtr<FSocket_FASI>> Result;

	if (SocketMap.Contains(CurrentWeaponSocket))
	{
		Result.Add(SocketMap[CurrentWeaponSocket]);
	}

	return Result;
}

bool UProxyProcessComponent::ActiveAction(
	const TSharedPtr<FSocket_FASI>& CanbeActivedInfoSPtr, bool bIsAutomaticStop /*= false */
)
{
	// 使用武器
	auto WeaponSPtr = FindSocket(CurrentWeaponSocket);
	if (WeaponSPtr && (WeaponSPtr->Socket == CanbeActivedInfoSPtr->Socket))
	{
		ActiveActionImp(WeaponSPtr->Socket, bIsAutomaticStop);
	}
	// 使用主动技能
	else if (CanActiveSocketMap.Contains(CanbeActivedInfoSPtr->Socket))
	{
		if (ActivedCorrespondingWeapon(CanbeActivedInfoSPtr->Socket))
		{
			ActiveActionImp(CanbeActivedInfoSPtr->Socket, bIsAutomaticStop);
		}
	}
	// 使用消耗品
	else
	{
		ActiveActionImp(CanbeActivedInfoSPtr->Socket, bIsAutomaticStop);
	}

	return true;
}

void UProxyProcessComponent::CancelAction(const TSharedPtr<FSocket_FASI>& CanbeActivedInfoSPtr)
{
	auto WeaponSPtr = FindSocket(CurrentWeaponSocket);
	if (WeaponSPtr->Key == CanbeActivedInfoSPtr->Key)
	{
		CancelAction_Server(WeaponSPtr->Socket);
	}
	else
	{
		CancelAction_Server(CanbeActivedInfoSPtr->Socket);
	}
}

TArray<TSharedPtr<FSocket_FASI>> UProxyProcessComponent::GetCanbeActiveConsumable() const
{
	TArray<TSharedPtr<FSocket_FASI>> Result;

	for (auto Iter : SocketMap)
	{
		if (Iter.Value->Socket.MatchesTag(UGameplayTagsSubSystem::ConsumableSocket))
		{
			Result.Add(Iter.Value);
		}
	}

	return Result;
}

bool UProxyProcessComponent::ActivedCorrespondingWeapon(const FGameplayTag& ActiveSkillSocketTag)
{
#if UE_EDITOR || UE_CLIENT
	if (GetNetMode() == NM_Client)
	{
		ActivedCorrespondingWeapon_Server(ActiveSkillSocketTag);
	}
#endif

	auto SocketSPtr = SocketMap[ActiveSkillSocketTag];
	if (SocketSPtr->ProxySPtr->GetUnitType().MatchesTag(UGameplayTagsSubSystem::Unit_Skill_Active))
	{
		auto ActiveSkillUnitPtr = DynamicCastSharedPtr<FActiveSkillProxy>(SocketSPtr->ProxySPtr);
		const auto RequireWeaponUnitType = ActiveSkillUnitPtr->GetTableRowUnit_ActiveSkillExtendInfo()->
		                                                       RequireWeaponUnitType;

		const auto WeaponsMap = SocketMap;

		if (WeaponsMap.Contains(UGameplayTagsSubSystem::WeaponSocket_1))
		{
			auto WeaponSocketSPtr = WeaponsMap[UGameplayTagsSubSystem::WeaponSocket_1];

			if (
				WeaponSocketSPtr->ProxySPtr &&
				(WeaponSocketSPtr->ProxySPtr->GetUnitType() == RequireWeaponUnitType)
			)
			{
				SwitchWeaponImp(WeaponSocketSPtr->Socket);

				return true;
			}
		}

		if (WeaponsMap.Contains(UGameplayTagsSubSystem::WeaponSocket_2))
		{
			auto WeaponSocketSPtr = WeaponsMap[UGameplayTagsSubSystem::WeaponSocket_2];

			if (
				WeaponSocketSPtr->ProxySPtr &&
				(WeaponSocketSPtr->ProxySPtr->GetUnitType() == RequireWeaponUnitType)
			)
			{
				SwitchWeaponImp(WeaponSocketSPtr->Socket);

				return true;
			}
		}

		return false;
	}
	else
	{
		return true;
	}
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

TMap<FGameplayTag, TSharedPtr<FSocket_FASI>> UProxyProcessComponent::GetAllSocket() const
{
	return SocketMap;
}

TSharedPtr<FSocket_FASI> UProxyProcessComponent::FindSocket(const FGameplayTag& Tag) const
{
	if (SocketMap.Contains(Tag))
	{
		return SocketMap[Tag];
	}

	return nullptr;
}

void UProxyProcessComponent::UpdateCanbeActiveSkills()
{
	CanActiveSocketMap.Empty();

	struct FMyStruct
	{
		FGameplayTag ActiveSocketTag;
	};
	TArray<FGameplayTag> Ary
	{
		UGameplayTagsSubSystem::ActiveSocket_1,
		UGameplayTagsSubSystem::ActiveSocket_2,
		UGameplayTagsSubSystem::ActiveSocket_3,
		UGameplayTagsSubSystem::ActiveSocket_4,
	};

	for (const auto& Iter : Ary)
	{
		auto SocketSPtr = FindSocket(Iter);
		if (
			SocketSPtr->ProxySPtr &&
			SocketSPtr->ProxySPtr->GetUnitType().MatchesTag(UGameplayTagsSubSystem::Unit_Skill_Active)
		)
		{
			CanActiveSocketMap.Add(Iter, SocketSPtr);
		}
	}
}

void UProxyProcessComponent::UpdateCanbeActiveSkills_UsePassiveSocket(
	const TMap<FGameplayTag, TSharedPtr<FSocket_FASI>>& InCanActiveSocketMap
)
{
	CanActiveSocketMap = InCanActiveSocketMap;
	OnCanAciveSkillChanged();
}

TMap<FGameplayTag, TSharedPtr<FSocket_FASI>> UProxyProcessComponent::GetCanbeActiveSkills() const
{
	return CanActiveSocketMap;
}

TSharedPtr<FActiveSkillProxy> UProxyProcessComponent::FindActiveSkillBySocket(const FGameplayTag& Tag) const
{
	if (SocketMap.Contains(Tag))
	{
		return DynamicCastSharedPtr<FActiveSkillProxy>(SocketMap[Tag]->ProxySPtr);
	}

	return nullptr;
}

TSharedPtr<FSocket_FASI> UProxyProcessComponent::FindActiveSkillByType(const FGameplayTag& TypeTag) const
{
	for (const auto& Iter : SocketMap)
	{
		if (Iter.Value->ProxySPtr && Iter.Value->ProxySPtr->GetUnitType().MatchesTag(
			UGameplayTagsSubSystem::Unit_Skill_Active_Switch
		))
		{
			return Iter.Value;
		}
	}

	return nullptr;
}

void UProxyProcessComponent::Add(const TSharedPtr<FSocket_FASI>& Socket)
{
	SocketMap.Add(Socket->Socket, Socket);
	if (Socket->ProxySPtr)
	{
		Socket->ProxySPtr->Allocation();

#if UE_EDITOR || UE_SERVER
		if (GetNetMode() == NM_DedicatedServer)
		{
			Socket->ProxySPtr->Update2Client();
		}
#endif
	}
}

void UProxyProcessComponent::Update(const TSharedPtr<FSocket_FASI>& Socket)
{
	if (SocketMap.Contains(Socket->Socket))
	{
		if (SocketMap[Socket->Socket]->ProxySPtr == Socket->ProxySPtr)
		{
		}
		else
		{
			if (SocketMap[Socket->Socket]->ProxySPtr)
			{
				SocketMap[Socket->Socket]->ProxySPtr->UnAllocation();
#if UE_EDITOR || UE_SERVER
				if (GetNetMode() == NM_DedicatedServer)
				{
					SocketMap[Socket->Socket]->ProxySPtr->Update2Client();
				}
#endif
			}

			if (Socket->ProxySPtr)
			{
				Socket->ProxySPtr->Allocation();
#if UE_EDITOR || UE_SERVER
				if (GetNetMode() == NM_DedicatedServer)
				{
					Socket->ProxySPtr->Update2Client();
				}
#endif
			}

			*SocketMap[Socket->Socket] = *Socket;

#if UE_EDITOR || UE_SERVER
			if (GetNetMode() == NM_DedicatedServer)
			{
				AllocationSkills_Container.UpdateItem(Socket);
			}
#endif
		}
	}
}

bool UProxyProcessComponent::Active(const TSharedPtr<FSocket_FASI>& Socket)
{
	return Active(Socket->Socket);
}

bool UProxyProcessComponent::Active(const FGameplayTag& Socket)
{
	if (CanActiveSocketMap.Contains(Socket))
	{
		if (CanActiveSocketMap[Socket]->ProxySPtr)
		{
			return CanActiveSocketMap[Socket]->ProxySPtr->Active();
		}
	}
	else if (SocketMap.Contains(Socket))
	{
		if (SocketMap[Socket]->ProxySPtr)
		{
			return SocketMap[Socket]->ProxySPtr->Active();
		}
	}

	return false;
}

void UProxyProcessComponent::Cancel(const TSharedPtr<FSocket_FASI>& Socket)
{
	Cancel(Socket->Socket);
}

void UProxyProcessComponent::Cancel(const FGameplayTag& Socket)
{
	if (SocketMap.Contains(Socket))
	{
		if (SocketMap[Socket]->ProxySPtr)
		{
			SocketMap[Socket]->ProxySPtr->Cancel();
		}
	}
}
