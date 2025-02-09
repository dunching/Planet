#include "ItemProxy_Character.h"

#include "AbilitySystemComponent.h"

#include "AssetRefMap.h"
#include "CharacterBase.h"
#include "HumanCharacter_AI.h"
#include "SceneProxyExtendInfo.h"
#include "GameplayTagsLibrary.h"
#include "CharacterAttibutes.h"
#include "CharactersInfo.h"
#include "InventoryComponent.h"

bool FCharacterSocket::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
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

void FCharacterSocket::UpdateProxy(const TSharedPtr<FBasicProxy>& ProxySPtr)
{
	if (ProxySPtr)
	{
		AllocationedProxyID = ProxySPtr->GetID();
	}
}

bool FCharacterSocket::operator()() const
{
	return AllocationedProxyID.IsValid();
}

bool FCharacterSocket::IsValid() const
{
	return AllocationedProxyID.IsValid();
}

void FCharacterSocket::ResetSocket()
{
	AllocationedProxyID = FGuid();
}

FCharacterProxy::FCharacterProxy()
{
	CharacterAttributesSPtr = MakeShared<FCharacterAttributes>();
}

FCharacterProxy::FCharacterProxy(const IDType& InID):
	FCharacterProxy()
{
	ID = InID;
}

void FCharacterProxy::UpdateByRemote(const TSharedPtr<FCharacterProxy>& RemoteSPtr)
{
	Super::UpdateByRemote(RemoteSPtr);

	ProxyCharacterPtr = RemoteSPtr->ProxyCharacterPtr;
}

bool FCharacterProxy::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);

	Ar << Title;
	Ar << Name;
	Ar << Level;
	Ar << ProxyCharacterPtr;
	CharacterAttributesSPtr->NetSerialize(Ar, Map, bOutSuccess);

	if (Ar.IsSaving())
	{
		auto Num= TeammateConfigureMap.Num();
		Ar << Num;
		for (auto& Iter : TeammateConfigureMap)
		{
			Iter.Value.NetSerialize(Ar, Map, bOutSuccess);
		}
	}
	else if (Ar.IsLoading())
	{
		TeammateConfigureMap.Empty();
		
		int32 Num= 0;
		Ar << Num;
		for (int32 Index = 0; Index < Num; Index++)
		{
			FCharacterSocket MySocket_FASI;
			MySocket_FASI.NetSerialize(Ar, Map, bOutSuccess);

			TeammateConfigureMap.Add(MySocket_FASI.Socket, MySocket_FASI);
		}
	}

	return true;
}

void FCharacterProxy::InitialProxy(const FGameplayTag& InProxyType)
{
	Super::InitialProxy(InProxyType);

	Title = GetDT_CharacterType()->Title;

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

FTableRowProxy_CharacterGrowthAttribute* FCharacterProxy::GetDT_CharacterInfo() const
{
	auto SceneProxyExtendInfoMapPtr = USceneProxyExtendInfoMap::GetInstance();
	auto DataTable = SceneProxyExtendInfoMapPtr->DataTable_Character_GrowthAttribute.LoadSynchronous();

	auto SceneProxyExtendInfoPtr =
		DataTable->FindRow<FTableRowProxy_CharacterGrowthAttribute>(
			*ProxyCharacterPtr->GetCharacterAttributesComponent()->CharacterGrowthAttribute.ToString(), TEXT("GetProxy"));
	return SceneProxyExtendInfoPtr;
}

FTableRowProxy_CharacterType* FCharacterProxy::GetDT_CharacterType() const
{
	auto SceneProxyExtendInfoMapPtr = USceneProxyExtendInfoMap::GetInstance();
	auto DataTable = SceneProxyExtendInfoMapPtr->DataTable_Proxy_CharacterInfo.LoadSynchronous();

	auto SceneProxyExtendInfoPtr =
		DataTable->FindRow<FTableRowProxy_CharacterType>(*ProxyType.ToString(), TEXT("GetProxy"));
	return SceneProxyExtendInfoPtr;
}

void FCharacterProxy::RelieveRootBind()
{
}

AHumanCharacter_AI* FCharacterProxy::SpwanCharacter(const FTransform& Transform)
{
	AHumanCharacter_AI* Result = nullptr;
	if (ProxyCharacterPtr.IsValid())
	{
	}
	else
	{
		FActorSpawnParameters SpawnParameters;

		Result =
			InventoryComponentPtr->GetWorld()->SpawnActor<AHumanCharacter_AI>(
				GetDT_CharacterType()->CharacterClass, Transform, SpawnParameters);

		ProxyCharacterPtr = Result;

		Update2Client();
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

FCharacterSocket FCharacterProxy::FindSocket(const FGameplayTag& SocketID) const
{
	if (TeammateConfigureMap.Contains(SocketID))
	{
		return TeammateConfigureMap[SocketID];
	}

	return FCharacterSocket();
}

FCharacterSocket FCharacterProxy::FindSocketByType(const FGameplayTag& InProxyType) const
{
	if (ProxyCharacterPtr.IsValid())
	{
		for (const auto& Iter : TeammateConfigureMap)
		{
			auto ProxySPtr = InventoryComponentPtr->FindProxy_BySocket(Iter.Value);
			if (ProxySPtr && ProxySPtr->GetProxyType() == ProxyType)
			{
				return Iter.Value;
			}
		}
	}

	return FCharacterSocket();
}

void FCharacterProxy::GetWeaponSocket(FCharacterSocket& FirstWeaponSocketInfoSPtr,
	FCharacterSocket& SecondWeaponSocketInfoSPtr)
{
	FirstWeaponSocketInfoSPtr = FindSocket(UGameplayTagsLibrary::WeaponSocket_1);
	SecondWeaponSocketInfoSPtr = FindSocket(UGameplayTagsLibrary::WeaponSocket_2);
}

TMap<FGameplayTag, FCharacterSocket> FCharacterProxy::GetSockets() const
{
	return  TeammateConfigureMap;
}

TWeakObjectPtr<FCharacterProxy::FPawnType> FCharacterProxy::GetCharacterActor() const
{
	return ProxyCharacterPtr;
}

void FCharacterProxy::UpdateSocket(const FCharacterSocket& Socket)
{
	if (TeammateConfigureMap.Contains(Socket.Socket))
	{
		TeammateConfigureMap[Socket.Socket] = Socket;
	}
}
