#include "ItemProxy_Character.h"

#include "AbilitySystemComponent.h"

#include "AssetRefMap.h"
#include "CharacterBase.h"
#include "HumanCharacter_AI.h"
#include "SceneUnitExtendInfo.h"
#include "GameplayTagsLibrary.h"
#include "CharacterAttibutes.h"
#include "CharactersInfo.h"
#include "HoldingItemsComponent.h"

bool FMySocket_FASI::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	if (Ar.IsSaving())
	{
		Ar << Socket;
		Ar << SkillProxyID;
	}
	else if (Ar.IsLoading())
	{
		Ar << Socket;
		Ar << SkillProxyID;
	}

	return true;
}

void FMySocket_FASI::UpdateProxy(const TSharedPtr<FBasicProxy>& ProxySPtr)
{
	if (ProxySPtr)
	{
		SkillProxyID = ProxySPtr->GetID();
	}
}

bool FMySocket_FASI::operator()() const
{
	return SkillProxyID.IsValid();
}

bool FMySocket_FASI::IsValid() const
{
	return SkillProxyID.IsValid();
}

void FMySocket_FASI::ResetSocket()
{
	SkillProxyID = FGuid();
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
			FMySocket_FASI MySocket_FASI;
			MySocket_FASI.NetSerialize(Ar, Map, bOutSuccess);

			TeammateConfigureMap.Add(MySocket_FASI.Socket, MySocket_FASI);
		}
	}

	return true;
}

void FCharacterProxy::InitialUnit()
{
	Super::InitialUnit();

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

		{UGameplayTagsLibrary::ConsumableSocket1, EKeys::One},
		{UGameplayTagsLibrary::ConsumableSocket2, EKeys::Two},
		{UGameplayTagsLibrary::ConsumableSocket3, EKeys::Three},
		{UGameplayTagsLibrary::ConsumableSocket4, EKeys::Four},

		{UGameplayTagsLibrary::PassiveSocket_1, EKeys::Invalid},
		{UGameplayTagsLibrary::PassiveSocket_2, EKeys::Invalid},
		{UGameplayTagsLibrary::PassiveSocket_3, EKeys::Invalid},
		{UGameplayTagsLibrary::PassiveSocket_4, EKeys::Invalid},
		{UGameplayTagsLibrary::PassiveSocket_5, EKeys::Invalid},
	};

	for (const auto& Iter : Ary)
	{
		FMySocket_FASI MySocket_FASI;

		MySocket_FASI.Socket = Iter.SocketTag;

		TeammateConfigureMap.Add(MySocket_FASI.Socket, MySocket_FASI);
	}
}

FTableRowUnit_CharacterGrowthAttribute* FCharacterProxy::GetDT_CharacterInfo() const
{
	auto SceneUnitExtendInfoMapPtr = USceneUnitExtendInfoMap::GetInstance();
	auto DataTable = SceneUnitExtendInfoMapPtr->DataTable_Character_GrowthAttribute.LoadSynchronous();

	auto SceneUnitExtendInfoPtr =
		DataTable->FindRow<FTableRowUnit_CharacterGrowthAttribute>(
			*ProxyCharacterPtr->CharacterGrowthAttribute.ToString(), TEXT("GetUnit"));
	return SceneUnitExtendInfoPtr;
}

FTableRowUnit_CharacterType* FCharacterProxy::GetDT_CharacterType() const
{
	auto SceneUnitExtendInfoMapPtr = USceneUnitExtendInfoMap::GetInstance();
	auto DataTable = SceneUnitExtendInfoMapPtr->DataTable_Unit_CharacterInfo.LoadSynchronous();

	auto SceneUnitExtendInfoPtr =
		DataTable->FindRow<FTableRowUnit_CharacterType>(*UnitType.ToString(), TEXT("GetUnit"));
	return SceneUnitExtendInfoPtr;
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

		SpawnParameters.Owner = GetOwnerCharacterProxy().Pin()->ProxyCharacterPtr.Get();

		Result =
			HoldingItemsComponentPtr->GetWorld()->SpawnActor<AHumanCharacter_AI>(
				GetDT_CharacterType()->CharacterClass, Transform, SpawnParameters);

		ProxyCharacterPtr = Result;
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

FMySocket_FASI FCharacterProxy::FindSocket(const FGameplayTag& SocketID) const
{
	if (TeammateConfigureMap.Contains(SocketID))
	{
		return TeammateConfigureMap[SocketID];
	}

	return FMySocket_FASI();
}

FMySocket_FASI FCharacterProxy::FindSocketByType(const FGameplayTag& ProxyType) const
{
	if (ProxyCharacterPtr.IsValid())
	{
		for (const auto& Iter : TeammateConfigureMap)
		{
			auto ProxySPtr = HoldingItemsComponentPtr->FindProxy_BySocket(Iter.Value);
			if (ProxySPtr && ProxySPtr->GetUnitType() == ProxyType)
			{
				return Iter.Value;
			}
		}
	}

	return FMySocket_FASI();
}

void FCharacterProxy::GetWeaponSocket(FMySocket_FASI& FirstWeaponSocketInfoSPtr,
	FMySocket_FASI& SecondWeaponSocketInfoSPtr)
{
	FirstWeaponSocketInfoSPtr = FindSocket(UGameplayTagsLibrary::WeaponSocket_1);
	SecondWeaponSocketInfoSPtr = FindSocket(UGameplayTagsLibrary::WeaponSocket_2);
}

void FCharacterProxy::UpdateSocket(const FMySocket_FASI& Socket)
{
	if (TeammateConfigureMap.Contains(Socket.Socket))
	{
		TeammateConfigureMap[Socket.Socket] = Socket;
	}
}
