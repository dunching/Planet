#include "RewardsTD.h"

#include "PlanetWorldSettings.h"
#include "Planet_Tools.h"

bool FRewardsItem::IsValid() const
{
	return !RewardsMap.IsEmpty();
}

UPAD_RewardsItems* UPAD_RewardsItems::GetInstance()
{
	auto WorldSetting = Cast<APlanetWorldSettings>(GetWorldImp()->GetWorldSettings());
	return WorldSetting->GetTableRow_RewardsTD();
}

FRewardsItem UPAD_RewardsItems::GetRewardsItem(const FGuid& RewardsItemID)
{
	FRewardsItem Result;
	if (ProxyMap.Contains(RewardsItemID))
	{
		Result = ProxyMap[RewardsItemID]; 
	}
	return Result;
}
