
#include "RaffleSubSystem.h"

#include <Subsystems/SubsystemBlueprintLibrary.h>
#include <Kismet/GameplayStatics.h>

#include "Planet_Tools.h"
#include "CharacterBase.h"
#include "PlanetPlayerState.h"
#include "HoldingItemsComponent.h"
#include "SceneUnitExtendInfo.h"

URaffleSubSystem* URaffleSubSystem::GetInstance()
{
	return Cast<URaffleSubSystem>(
		USubsystemBlueprintLibrary::GetGameInstanceSubsystem(GetWorldImp(), URaffleSubSystem::StaticClass())
	);
}

void URaffleSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

bool URaffleSubSystem::Raffle(ERaffleType RaffleType, int32 Count)const
{
	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!CharacterPtr)
	{
		return false;
	}

	auto& HoldItemPropertyRef =
		CharacterPtr->GetPlayerState<APlanetPlayerState>()->GetHoldingItemsComponent()->GetHoldItemProperty();

	switch (RaffleType)
	{
	case ERaffleType::kRafflePermanent:
	{
		auto CoinUnitPtr = HoldItemPropertyRef.FindUnit_Coin(ECoinUnitType::kRaffleLimit);
		if (CoinUnitPtr)
		{
			if (CoinUnitPtr->GetCurrentValue() > Count)
			{
				CoinUnitPtr->AddCurrentValue(-Count);
				return RafflePermanent(Count);
			}
		}
	}
	break;
	case ERaffleType::kRaffleLimit:
	{

	}
	break;
	case ERaffleType::kNovice:
	{

	}
	break;
	}

	return false;
}

void URaffleSubSystem::SyncUnits2Player()const
{
	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!CharacterPtr)
	{
		return;
	}
	auto& HoldItemPropertyRef = CharacterPtr->GetHoldingItemsComponent()->GetHoldItemProperty();
	HoldItemPropertyRef.SyncApendingUnit(ApendingID);
}

bool URaffleSubSystem::RafflePermanent(int32 Count)const
{
	// Í¨¹ýHTTPÇëÇó

	RafflePermanentComplete(
#if WITH_EDITOR
		Count
#endif
	);

	return true;
}

void URaffleSubSystem::RafflePermanentComplete(
#if WITH_EDITOR
	int32 Count
#endif
)const
{
	TArray<FGuid> Ary;
#if TESTRAFFLE
	Ary.Append(
		{
			FGuid(TEXT("{46DF00CA-5D51-46FA-A136-B0895B1D6812}")),
			FGuid(TEXT("{2C712123-CFD0-4775-AF12-CC5C6521CE69}")),
			FGuid(TEXT("{B3B51225-735D-425B-8368-A3955D670FD5}")),
			FGuid(TEXT("{4C8652EC-DA76-43E0-85EB-6032E23E6833}")),
			FGuid(TEXT("{30CB1E1B-80B1-476E-837F-3D2BE43831DF}")),
			FGuid(TEXT("{E1AF2E92-BBB6-4B5D-97D3-3D7F10957A20}")),

			FGuid(TEXT("{8705713D-8ADA-45D1-84DC-E1B63B9E0AAA}")),
		}
		);
#else

#endif
	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!CharacterPtr)
	{
		return;
	}
	auto& HoldItemPropertyRef = CharacterPtr->GetHoldingItemsComponent()->GetHoldItemProperty();

	auto SceneUnitExtendInfoMapPtr = USceneUnitExtendInfoMap::GetInstance();

	auto DataTable = SceneUnitExtendInfoMapPtr->DataTable.LoadSynchronous();

	TArray<TPair<FTableRowUnit, TSubclassOf<UBasicUnit>>>GetUnitAry;
	for (const auto& Iter : Ary)
	{
		auto RowPtr = DataTable->FindRow<FTableRowUnit>(*Iter.ToString(), TEXT("GetUnit"));

// 		HoldItemPropertyRef.AddUnit_Apending(SecondIter.Key, ApendingID);
// 
// 		GetUnitAry.Add({ SecondIter.Value , SceneUnitExtendInfoMapPtr->SkillToolsMap[SecondIter.Key] });
	}

	OnGetUnitAry.ExcuteCallback(GetUnitAry);
}
