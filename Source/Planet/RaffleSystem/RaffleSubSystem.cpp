
#include "RaffleSubSystem.h"

#include <Subsystems/SubsystemBlueprintLibrary.h>
#include <Kismet/GameplayStatics.h>

#include "Planet_Tools.h"
#include "CharacterBase.h"
#include "PlanetPlayerState.h"
#include "HoldingItemsComponent.h"
#include "SceneProxyExtendInfo.h"
#include "GameplayTagsLibrary.h"
#include "ItemProxy_Container.h"

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
		auto CoinProxyPtr = HoldItemPropertyRef->FindProxy_Coin(UGameplayTagsLibrary::Proxy_Coin_RaffleLimit);
		if (CoinProxyPtr)
		{
			if (CoinProxyPtr->GetCurrentValue() > Count)
			{
				CoinProxyPtr->AddCurrentValue(-Count);
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

void URaffleSubSystem::SyncProxys2Player()const
{
	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!CharacterPtr)
	{
		return;
	}
	CharacterPtr->GetHoldingItemsComponent()->SyncPendingProxy(ApendingID);
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
			UGameplayTagsLibrary::Proxy_Skill_Active_Displacement,
			UGameplayTagsLibrary::Proxy_Skill_Active_GroupTherapy,
			UGameplayTagsLibrary::Proxy_Skill_Active_ContinuousGroupTherapy,
			UGameplayTagsLibrary::Proxy_Skill_Active_Tornado,
			UGameplayTagsLibrary::Proxy_Skill_Active_FlyAway,

			UGameplayTagsLibrary::Proxy_Skill_Passve_ZMJZ,
		}
		);
#else

#endif
	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!CharacterPtr)
	{
		return;
	}

	auto SceneProxyExtendInfoMapPtr = USceneProxyExtendInfoMap::GetInstance();

	auto DataTable = SceneProxyExtendInfoMapPtr->DataTable_Proxy.LoadSynchronous();

	TArray<FTableRowProxy*>GetProxyAry;
	for (const auto& Iter : Ary)
	{
		auto RowPtr = DataTable->FindRow<FTableRowProxy>(*Iter.ToString(), TEXT("GetProxy"));

		CharacterPtr->GetHoldingItemsComponent()->AddProxy_Pending(Iter, 1, ApendingID);
 
 		GetProxyAry.Add(SceneProxyExtendInfoMapPtr->GetTableRowProxy(Iter));
	}

	OnGetProxyAry.ExcuteCallback(GetProxyAry);
}
