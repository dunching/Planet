
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

bool URaffleSubSystem::Raffle(ERaffleType RaffleType, int32 Count) const
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
		auto CoinUnitPtr = HoldItemPropertyRef.FindUnit(ECoinUnitType::kRaffleLimit);
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

bool URaffleSubSystem::RafflePermanent(int32 Count) const
{
	// Í¨¹ýHTTPÇëÇó

	RafflePermanentComplete();

	return true;
}

void URaffleSubSystem::RafflePermanentComplete() const
{
#if TESTRAFFLE
	TArray<FGuid> Ary
	{
		FGuid(TEXT("{46DF00CA-5D51-46FA-A136-B0895B1D6812}")),
	};

	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!CharacterPtr)
	{
		return;
	}

	auto SceneUnitExtendInfoMapPtr = USceneUnitExtendInfoMap::GetInstance();
	for (const auto &Iter : Ary)
	{
		for (const auto& SecondIter : SceneUnitExtendInfoMapPtr->SkillUnitMap)
		{
			if (Iter == SecondIter.Value->Guid)
			{
				auto& HoldItemPropertyRef = CharacterPtr->GetHoldingItemsComponent()->GetHoldItemProperty();
				HoldItemPropertyRef.AddUnit(SecondIter.Key);

				break;
			}
		}
	}
#else

#endif
}
