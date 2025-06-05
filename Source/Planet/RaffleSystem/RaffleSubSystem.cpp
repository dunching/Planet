
#include "RaffleSubSystem.h"

#include <Subsystems/SubsystemBlueprintLibrary.h>
#include <Kismet/GameplayStatics.h>

#include "Planet_Tools.h"
#include "CharacterBase.h"
#include "PlanetPlayerState.h"
#include "InventoryComponent.h"
#include "DataTableCollection.h"
#include "GameplayTagsLibrary.h"
#include "ItemProxy_Container.h"
#include "Tools.h"

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
		CharacterPtr->GetInventoryComponent();

	switch (RaffleType)
	{
	case ERaffleType::kRafflePermanent:
	{
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
	CharacterPtr->GetInventoryComponent()->SyncPendingProxy(ApendingID);
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

	auto SceneProxyExtendInfoMapPtr = UDataTableCollection::GetInstance();
}
