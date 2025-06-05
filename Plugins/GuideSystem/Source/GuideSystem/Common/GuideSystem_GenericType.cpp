
#include "GuideSystem_GenericType.h"

bool FTaskNodeResuleHelper::GetIsValid() const
{
	return TaskID.IsValid();
}

FTaskNodeDescript::FTaskNodeDescript()
{
}

FTaskNodeDescript::FTaskNodeDescript(
	bool bIsOnlyFresh
	):
	 bIsOnlyFresh(bIsOnlyFresh)
{
}

bool FTaskNodeDescript::GetIsValid() const
{
	return /* bIsFreshPreviouDescription && */ (!Description.IsEmpty());
}

FTaskNodeDescript FTaskNodeDescript::Refresh = FTaskNodeDescript(true);
