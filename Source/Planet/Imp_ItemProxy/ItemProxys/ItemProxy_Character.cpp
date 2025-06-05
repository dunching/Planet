#include "ItemProxy_Character.h"

#include "AssetRefMap.h"
#include "GameplayTagsLibrary.h"
#include "GroupManagger.h"
#include "HumanCharacter_AI.h"
#include "InventoryComponentBase.h"
#include "InventoryComponent.h"
#include "PAD_ItemProxyCollection.h"
#include "ItemProxy_Skills.h"
#include "ItemProxy_Weapon.h"
#include "AIComponent.h"
#include "DataTableCollection.h"

UItemProxy_Description_Character::UItemProxy_Description_Character(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(ObjectInitializer)
{
	CharacterGrowthAttributeAry.SetNum(27);
}

bool FCharacterSocket::NetSerialize(
	FArchive& Ar,
	class UPackageMap* Map,
	bool& bOutSuccess
	)
{
	if (Ar.IsSaving())
	{
		Ar << Socket;
		Ar << AllocationedProxyID;
	}
	else if (Ar.IsLoading())
	{
		Ar << Socket;
		Ar << AllocationedProxyID;
	}

	return true;
}

void FCharacterSocket::UpdateProxy(
	const TSharedPtr<FBasicProxy>& ProxySPtr
	)
{
	if (ProxySPtr)
	{
		AllocationedProxyID = ProxySPtr->GetID();
	}
}

void FCharacterSocket::SetAllocationedProxyID(
	const FGuid& NewID
	)
{
	AllocationedProxyID = NewID;
}

FGuid FCharacterSocket::GetAllocationedProxyID() const
{
	return AllocationedProxyID;
}

bool FCharacterSocket::operator()() const
{
	return AllocationedProxyID.IsValid();
}

bool FCharacterSocket::IsValid() const
{
	return AllocationedProxyID.IsValid();
}

void FCharacterSocket::ResetAllocatedProxy()
{
	AllocationedProxyID = FGuid();
}

FCharacterProxy::FCharacterProxy()
{
}

FCharacterProxy::FCharacterProxy(
	const IDType& InID
	):
	 FCharacterProxy()
{
	SetID(InID);
}

void FCharacterProxy::UpdateByRemote(
	const TSharedPtr<FCharacterProxy>& RemoteSPtr
	)
{
	Super::UpdateByRemote(RemoteSPtr);
	UpdateByRemote_Allocationble(RemoteSPtr);

	TeammateConfigureMap = RemoteSPtr->TeammateConfigureMap;
	ProxyCharacterPtr = RemoteSPtr->ProxyCharacterPtr;

	const auto OldLevel = Level;
	if (RemoteSPtr->Level == Level)
	{
	}
	else
	{
		Level = RemoteSPtr->Level;
		LevelChangedDelegate.ValueChanged(OldLevel, Level);
	}

	const auto OldExperience = Experience;
	if (RemoteSPtr->Experience == Experience)
	{
	}
	else
	{
		Experience = RemoteSPtr->Experience;
		ExperienceChangedDelegate.ValueChanged(OldExperience, Experience);
	}
}

bool FCharacterProxy::NetSerialize(
	FArchive& Ar,
	class UPackageMap* Map,
	bool& bOutSuccess
	)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);
	NetSerialize_Allocationble(Ar, Map, bOutSuccess);

	Ar << Title;
	Ar << Name;
	Ar << Level;
	Ar << ProxyCharacterPtr;

	if (Ar.IsSaving())
	{
		auto TeammateConfigureMapNum = TeammateConfigureMap.Num();
		Ar << TeammateConfigureMapNum;
		for (auto& Iter : TeammateConfigureMap)
		{
			Iter.Value.NetSerialize(Ar, Map, bOutSuccess);
		}
	}
	else if (Ar.IsLoading())
	{
		TeammateConfigureMap.Empty();

		int32 TeammateConfigureMapNum = 0;
		Ar << TeammateConfigureMapNum;
		for (int32 Index = 0; Index < TeammateConfigureMapNum; Index++)
		{
			FCharacterSocket MySocket_FASI;
			MySocket_FASI.NetSerialize(Ar, Map, bOutSuccess);

			TeammateConfigureMap.Add(MySocket_FASI.Socket, MySocket_FASI);
		}
	}

	CharacterTalent.NetSerialize(Ar, Map, bOutSuccess);
	Ar << Level;
	Ar << Experience;

	return true;
}

bool FCharacterTalent::NetSerialize(
	FArchive& Ar,
	class UPackageMap* Map,
	bool& bOutSuccess
	)
{
	return true;
}

void FCharacterProxy::InitialProxy(
	const FGameplayTag& InProxyType
	)
{
	Super::InitialProxy(InProxyType);

	ProxyPtr = this;

	Title = GetTableRowProxy_Character()->Title;

	struct FMyStruct
	{
		FGameplayTag SocketTag;
		FKey Key;
	};
	TArray<FMyStruct> Ary
	{
		{UGameplayTagsLibrary::ActiveSocket_1, EKeys::Q},
		{UGameplayTagsLibrary::ActiveSocket_2, EKeys::E},
		{UGameplayTagsLibrary::ActiveSocket_3, EKeys::R},
		{UGameplayTagsLibrary::ActiveSocket_4, EKeys::F},

		{UGameplayTagsLibrary::WeaponSocket_1, EKeys::LeftMouseButton},
		{UGameplayTagsLibrary::WeaponSocket_2, EKeys::LeftMouseButton},

		{UGameplayTagsLibrary::ConsumableSocket_1, EKeys::One},
		{UGameplayTagsLibrary::ConsumableSocket_2, EKeys::Two},
		{UGameplayTagsLibrary::ConsumableSocket_3, EKeys::Three},
		{UGameplayTagsLibrary::ConsumableSocket_4, EKeys::Four},

		{UGameplayTagsLibrary::PassiveSocket_1, EKeys::Invalid},
		{UGameplayTagsLibrary::PassiveSocket_2, EKeys::Invalid},
		{UGameplayTagsLibrary::PassiveSocket_3, EKeys::Invalid},
		{UGameplayTagsLibrary::PassiveSocket_4, EKeys::Invalid},
		{UGameplayTagsLibrary::PassiveSocket_5, EKeys::Invalid},
	};

	for (const auto& Iter : Ary)
	{
		FCharacterSocket MySocket_FASI;

		MySocket_FASI.Socket = Iter.SocketTag;

		TeammateConfigureMap.Add(MySocket_FASI.Socket, MySocket_FASI);
	}
}

UItemProxy_Description_Character* FCharacterProxy::GetTableRowProxy_Character() const
{
	auto TableRowPtr = GetTableRowProxy();
	auto ItemProxy_Description_SkillPtr = Cast<UItemProxy_Description_Character>(
		 TableRowPtr->ItemProxy_Description.LoadSynchronous()
		);
	return ItemProxy_Description_SkillPtr;
}

void FCharacterProxy::RelieveRootBind()
{
}

AHumanCharacter_AI* FCharacterProxy::SpwanCharacter(
	const FTransform& Transform
	)
{
	AHumanCharacter_AI* Result = nullptr;
	if (ProxyCharacterPtr.IsValid())
	{
	}
	else
	{
		FActorSpawnParameters SpawnParameters;

		SpawnParameters.CustomPreSpawnInitalization = [this](
			auto ActorPtr
			)
			{
				auto GroupSharedInfoPtr = GetInventoryComponent()->GetOwner<AGroupManagger>();

				auto AICharacterPtr = Cast<AHumanCharacter_AI>(ActorPtr);

				if (AICharacterPtr && GroupSharedInfoPtr)
				{
					AICharacterPtr->GetAIComponent()->bIsTeammate = true;
					AICharacterPtr->GetCharacterAttributesComponent()->SetCharacterID(GetID());
					AICharacterPtr->SetGroupSharedInfo(GroupSharedInfoPtr);
				}
			};

		Result =
			GetInventoryComponent()->GetWorld()->SpawnActor<AHumanCharacter_AI>(
			                                                                    GetTableRowProxy_Character()->
			                                                                    CharacterClass,
			                                                                    Transform,
			                                                                    SpawnParameters
			                                                                   );

		ProxyCharacterPtr = Result;

		// 注册
		const auto CanActiveSocketMap = GetSockets();
		for (const auto& Iter : CanActiveSocketMap)
		{
			auto ProxySPtr = GetInventoryComponent()->FindProxy(Iter.Value.GetAllocationedProxyID());
			if (ProxySPtr)
			{
				// 
				if (ProxySPtr->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Weapon))
				{
					auto WeaponProxySPtr = DynamicCastSharedPtr<FWeaponProxy>(ProxySPtr);
					if (WeaponProxySPtr)
					{
						WeaponProxySPtr->GetWeaponSkill()->RegisterSkill();
					}
				}
				else if (ProxySPtr->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Weapon))
				{
					auto SkillProxySPtr = DynamicCastSharedPtr<FSkillProxy>(ProxySPtr);
					if (SkillProxySPtr)
					{
						SkillProxySPtr->RegisterSkill();
					}
				}
			}
		}

		UpdateData();
	}
	return Result;
}

void FCharacterProxy::DestroyCharacter()
{
	if (ProxyCharacterPtr.IsValid())
	{
		ProxyCharacterPtr->Destroy();
	}

	ProxyCharacterPtr = nullptr;
}

FCharacterSocket FCharacterProxy::FindSocket(
	const FGameplayTag& SocketID
	) const
{
	if (TeammateConfigureMap.Contains(SocketID))
	{
		return TeammateConfigureMap[SocketID];
	}

	return FCharacterSocket();
}

FCharacterSocket FCharacterProxy::FindSocketByType(
	const FGameplayTag& InProxyType
	) const
{
	if (ProxyCharacterPtr.IsValid())
	{
		for (const auto& Iter : TeammateConfigureMap)
		{
			auto ProxySPtr = DynamicCastSharedPtr<FBasicProxy>(GetInventoryComponent()->FindProxy_BySocket(Iter.Value));
			if (ProxySPtr && ProxySPtr->GetProxyType() == GetProxyType())
			{
				return Iter.Value;
			}
		}
	}

	return FCharacterSocket();
}

void FCharacterProxy::GetWeaponSocket(
	FCharacterSocket& FirstWeaponSocketInfoSPtr,
	FCharacterSocket& SecondWeaponSocketInfoSPtr
	)
{
	FirstWeaponSocketInfoSPtr = FindSocket(UGameplayTagsLibrary::WeaponSocket_1);
	SecondWeaponSocketInfoSPtr = FindSocket(UGameplayTagsLibrary::WeaponSocket_2);
}

TMap<FGameplayTag, FCharacterSocket> FCharacterProxy::GetSockets() const
{
	return TeammateConfigureMap;
}

TWeakObjectPtr<FCharacterProxy::FPawnType> FCharacterProxy::GetCharacterActor() const
{
	return ProxyCharacterPtr;
}

void FCharacterProxy::UpdateSocket(
	const FCharacterSocket& Socket
	)
{
	// 这里做一个转发，
	// 同步到服务器
	if (GetInventoryComponent()->GetNetMode() == NM_Client)
	{
		GetInventoryComponent()->UpdateSocket(GetID(), Socket);
	}

	// 判断插槽是否可以存放这个物品代理
	if (Socket.Socket.MatchesTag(UGameplayTagsLibrary::ConsumableSocket))
	{
		auto ProxySPtr = GetInventoryComponent()->FindProxy(Socket.GetAllocationedProxyID());
		if (ProxySPtr)
		{
			if (!ProxySPtr->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Consumables))
			{
				return;
			}
		}
	}
	else if (Socket.Socket.MatchesTag(UGameplayTagsLibrary::WeaponSocket))
	{
		auto ProxySPtr = GetInventoryComponent()->FindProxy(Socket.GetAllocationedProxyID());
		if (ProxySPtr)
		{
			if (!ProxySPtr->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Weapon))
			{
				return;
			}
		}
	}
	else if (Socket.Socket.MatchesTag(UGameplayTagsLibrary::ActiveSocket))
	{
		auto ProxySPtr = GetInventoryComponent()->FindProxy(Socket.GetAllocationedProxyID());
		if (ProxySPtr)
		{
			if (!ProxySPtr->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Active))
			{
				return;
			}
		}
	}
	else if (Socket.Socket.MatchesTag(UGameplayTagsLibrary::PassiveSocket))
	{
		auto ProxySPtr = GetInventoryComponent()->FindProxy(Socket.GetAllocationedProxyID());
		if (ProxySPtr)
		{
			if (!ProxySPtr->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Passve))
			{
				return;
			}
		}
	}
	else
	{
		return;
	}

	if (TeammateConfigureMap.Contains(Socket.Socket))
	{
		TeammateConfigureMap[Socket.Socket] = Socket;

		OnCharacterSocketUpdated.Broadcast(Socket, GetProxyType());
	}

	UpdateData();
}

FString FCharacterProxy::GetDisplayTitle() const
{
	if (Name.IsEmpty())
	{
		return Title;
	}
	else
	{
		return FString::Printf(TEXT("%s-%s"), *Title, *Name);
	}
}

void FCharacterProxy::AddExperience(
	uint32 Value
	)
{
	const auto CharacterGrowthAttributeAry = GetTableRowProxy_Character()->CharacterGrowthAttributeAry;

	if (CharacterGrowthAttributeAry.Num() <= Level)
	{
		//满级了
		return;
	}

	const auto OldExperience = Experience;

	Experience += Value;

	int32 LevelExperience = 0;

	int32 NewLevelExperience = 0;

	ON_SCOPE_EXIT
	{
#if UE_EDITOR || UE_SERVER
		if (GetInventoryComponent()->GetNetMode() == NM_DedicatedServer)
		{
			GetInventoryComponent()->UpdateProxy(GetID());
		}
#endif

		ExperienceChangedDelegate.ValueChanged(OldExperience, Experience);

		if (LevelExperience > 0 && NewLevelExperience > 0)
		{
			LevelExperienceChangedDelegate.ValueChanged(LevelExperience, NewLevelExperience);
		}
	};

	auto CharacterPtr = GetCharacterActor();
	UPlanetAbilitySystemComponent* GASPtr = nullptr;
	if (CharacterPtr.IsValid())
	{
		GASPtr = CharacterPtr->GetAbilitySystemComponent();
	}

	for (; Level > 0 && Level < CharacterGrowthAttributeAry.Num();)
	{
		const auto& CurrentLevelAttribute = CharacterGrowthAttributeAry[Level - 1];

		// 当前等级升级所需经验
		LevelExperience = CurrentLevelAttribute.LevelExperience;

		if (Experience < LevelExperience)
		{
			return;
		}

		// 增加属性
		if (GASPtr)
		{
			auto SpecHandle = GASPtr->MakeOutgoingSpec(
			                                           UAssetRefMap::GetInstance()->OnceGEClass,
			                                           1,
			                                           GASPtr->MakeEffectContext()
			                                          );

			SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyType_BaseValue_Addtive);
			SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyItem_Stamina);

			SpecHandle.Data.Get()->SetSetByCallerMagnitude(
			                                               UGameplayTagsLibrary::GEData_ModifyItem_HP,
			                                               CharacterGrowthAttributeAry[Level].Max_HP
			                                              );
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(
			                                               UGameplayTagsLibrary::GEData_ModifyItem_MaxHP,
			                                               CharacterGrowthAttributeAry[Level].Max_HP
			                                              );
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(
			                                               UGameplayTagsLibrary::GEData_ModifyItem_Stamina,
			                                               CharacterGrowthAttributeAry[Level].Max_Stamina
			                                              );
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(
			                                               UGameplayTagsLibrary::GEData_ModifyItem_MaxStamina,
			                                               CharacterGrowthAttributeAry[Level].Max_Stamina
			                                              );
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(
			                                               UGameplayTagsLibrary::GEData_ModifyItem_Mana,
			                                               CharacterGrowthAttributeAry[Level].Max_Mana
			                                              );
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(
			                                               UGameplayTagsLibrary::GEData_ModifyItem_MaxMana,
			                                               CharacterGrowthAttributeAry[Level].Max_Mana
			                                              );

			GASPtr->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}

		// 调整数据
		const auto OldLevel = Level;
		Level++;

		LevelChangedDelegate.ValueChanged(OldLevel, Level);

		if (Level < CharacterGrowthAttributeAry.Num())
		{
			Experience -= LevelExperience;

			LevelExperience = CurrentLevelAttribute.LevelExperience;

			NewLevelExperience = CharacterGrowthAttributeAry[Level].LevelExperience;
		}
		else
		{
			Experience = CurrentLevelAttribute.LevelExperience;

			LevelExperience = CurrentLevelAttribute.LevelExperience;

			NewLevelExperience = CurrentLevelAttribute.LevelExperience;
		}
	}
}

uint8 FCharacterProxy::GetTalentNum() const
{
	return GetLevel();
}

uint8 FCharacterProxy::GetLevel() const
{
	return Level;
}

uint8 FCharacterProxy::GetExperience() const
{
	return Experience;
}

uint8 FCharacterProxy::GetLevelExperience() const
{
	const auto CharacterGrowthAttributeAry = GetTableRowProxy_Character()->CharacterGrowthAttributeAry;

	if (CharacterGrowthAttributeAry.Num() < 0)
	{
		return -1;
	}

	return CharacterGrowthAttributeAry[Level - 1].LevelExperience;
}

const FCharacterTalent& FCharacterProxy::GetCharacterTalent() const
{
	return CharacterTalent;
}

void FCharacterProxy::UpdateTalentSocket(
	const FGameplayTag& TalentSocketTag,
	int32 Num
	)
{
	const auto TargetTalent = UDataTableCollection::GetInstance()->GetTableRow_TalenSocket(TalentSocketTag);

	ON_SCOPE_EXIT
	{
#if UE_EDITOR || UE_SERVER
		if (GetInventoryComponent()->GetNetMode() == NM_DedicatedServer)
		{
			GetInventoryComponent()->UpdateProxy(GetID());
		}
#endif
	};

	if (Num > 0)
	{
		if (CharacterTalent.AllocationMap.Contains(TalentSocketTag))
		{
			CharacterTalent.AllocationMap[TalentSocketTag] = Num;
		}
		else
		{
			CharacterTalent.AllocationMap.Add(TalentSocketTag, Num);
		}

#if UE_EDITOR || UE_SERVER
		if (GetInventoryComponent()->GetNetMode() == NM_DedicatedServer)
		{
			auto CharacterPtr = GetCharacterActor();
			if (CharacterPtr.IsValid())
			{
				auto GASPtr = CharacterPtr->GetAbilitySystemComponent();
				auto SpecHandle = GASPtr->MakeOutgoingSpec(
				                                           UAssetRefMap::GetInstance()->OnceGEClass,
				                                           1,
				                                           GASPtr->MakeEffectContext()
				                                          );

				switch (TargetTalent->TalentType)
				{
				case ETalentType::kData:
					{
						SpecHandle.Data.Get()->AddDynamicAssetTag(
						                                          UGameplayTagsLibrary::GEData_ModifyType_Temporary_Data
						                                         );

						SpecHandle.Data.Get()->SetSetByCallerMagnitude(
						                                               TalentSocketTag,
						                                               Num * TargetTalent->Value
						                                              );
					}
					break;
				case ETalentType::kPercent:
					{
						SpecHandle.Data.Get()->AddDynamicAssetTag(
						                                          UGameplayTagsLibrary::GEData_ModifyType_Temporary_Percent
						                                         );

						SpecHandle.Data.Get()->SetSetByCallerMagnitude(
						                                               TalentSocketTag,
						                                               Num * TargetTalent->Value
						                                              );
					}
					break;
				}

				SpecHandle.Data.Get()->AddDynamicAssetTag(TargetTalent->ModifyDataTypeTag);

				GASPtr->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
#endif
	}
	else
	{
#if UE_EDITOR || UE_SERVER
		if (GetInventoryComponent()->GetNetMode() == NM_DedicatedServer)
		{
			auto CharacterPtr = GetCharacterActor();
			if (CharacterPtr.IsValid())
			{
				auto GASPtr = CharacterPtr->GetAbilitySystemComponent();
				auto SpecHandle = GASPtr->MakeOutgoingSpec(
				                                           UAssetRefMap::GetInstance()->OnceGEClass,
				                                           1,
				                                           GASPtr->MakeEffectContext()
				                                          );

				switch (TargetTalent->TalentType)
				{
				case ETalentType::kData:
					{
						SpecHandle.Data.Get()->AddDynamicAssetTag(
						                                          UGameplayTagsLibrary::GEData_ModifyType_RemoveTemporary_Data
						                                         );
					}
					break;
				case ETalentType::kPercent:
					{
						SpecHandle.Data.Get()->AddDynamicAssetTag(
						                                          UGameplayTagsLibrary::GEData_ModifyType_RemoveTemporary_Percent
						                                         );
					}
					break;
				}

				SpecHandle.Data.Get()->AddDynamicAssetTag(TargetTalent->ModifyDataTypeTag);
				SpecHandle.Data.Get()->SetSetByCallerMagnitude(
				                                               TalentSocketTag,
				                                               0
				                                              );

				GASPtr->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
#endif

		CharacterTalent.AllocationMap.Remove(TalentSocketTag);
	}
}
