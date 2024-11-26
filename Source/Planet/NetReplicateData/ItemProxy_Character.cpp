#include "ItemProxy_Character.h"

#include "AbilitySystemComponent.h"

#include "AssetRefMap.h"
#include "CharacterBase.h"
#include "HumanCharacter_AI.h"
#include "SceneUnitExtendInfo.h"
#include "GameplayTagsSubSystem.h"
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

	Ar << ProxyCharacterPtr;
	CharacterAttributesSPtr->NetSerialize(Ar, Map, bOutSuccess);

	for (auto &Iter : TeammateConfigureMap)
	{
		Iter.Value.NetSerialize(Ar, Map, bOutSuccess);
	}
	
	return true;
}

void FCharacterProxy::InitialUnit()
{
	Super::InitialUnit();

	CharacterAttributesSPtr->Title = GetDT_CharacterType()->Title;

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
