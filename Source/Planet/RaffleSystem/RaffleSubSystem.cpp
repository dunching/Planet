
#include "RaffleSubSystem.h"

#include <Subsystems/SubsystemBlueprintLibrary.h>
#include <Kismet/GameplayStatics.h>

#include "Planet_Tools.h"
#include "CharacterBase.h"
#include "PlanetPlayerState.h"
#include "HoldingItemsComponent.h"
#include "SceneUnitExtendInfo.h"
#include "GameplayTagsSubSystem.h"
#include "SceneUnitContainer.h"

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

	const auto HoldItemPropertyRef =
		CharacterPtr->GetHoldingItemsComponent();

	switch (RaffleType)
	{
	case ERaffleType::kRafflePermanent:
	{
		auto CoinUnitPtr = HoldItemPropertyRef->FindUnit_Coin(UGameplayTagsSubSystem::GetInstance()->Unit_Coin_RaffleLimit);
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
	CharacterPtr->GetHoldingItemsComponent()->SyncApendingUnit(ApendingID);
}

bool URaffleSubSystem::RafflePermanent(int32 Count)const
{
	// 通过HTTP请求

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
	TArray<FGameplayTag> Ary;
#if TESTRAFFLE
	Ary.Append(
		{
			UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Active_Displacement,
			UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Active_GroupTherapy,
			UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Active_ContinuousGroupTherapy,
			UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Active_Tornado,
			UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Active_FlyAway,

			UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Passve_ZMJZ,
		}
		);
#else

#endif
	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!CharacterPtr)
	{
		return;
	}

	auto SceneUnitExtendInfoMapPtr = USceneUnitExtendInfoMap::GetInstance();

	auto DataTable = SceneUnitExtendInfoMapPtr->DataTable_Unit.LoadSynchronous();

	TArray<FTableRowUnit*>GetUnitAry;
	for (const auto& Iter : Ary)
	{
		auto RowPtr = DataTable->FindRow<FTableRowUnit>(*Iter.ToString(), TEXT("GetUnit"));

		CharacterPtr->GetHoldingItemsComponent()->AddUnit_Apending(Iter, 1, ApendingID);
 
 		GetUnitAry.Add(SceneUnitExtendInfoMapPtr->GetTableRowUnit(Iter));
	}

	OnGetUnitAry.ExcuteCallback(GetUnitAry);
}
