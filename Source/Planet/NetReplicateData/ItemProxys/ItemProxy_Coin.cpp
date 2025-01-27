
#include "ItemProxy_Coin.h"

#include "AllocationSkills.h"
#include "ItemProxy_Container.h"
#include "TeamMatesHelperComponent.h"

FCoinProxy::FCoinProxy()
{

}

bool FCoinProxy::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);

	Ar << Num;
	Ar << OffsetNum;

	return true;
}

void FCoinProxy::UpdateByRemote(const TSharedPtr<FCoinProxy>& RemoteSPtr)
{
	Super::UpdateByRemote(RemoteSPtr);

	Num = RemoteSPtr->Num;
	OffsetNum = RemoteSPtr->OffsetNum;
}

void FCoinProxy::AddCurrentValue(int32 val)
{
	const auto Old = Num;
	Num += val;
	
	OffsetNum = val;

	CallbackContainerHelper.ValueChanged(Old, Num);
}

int32 FCoinProxy::GetCurrentValue() const
{
	return Num;
}

int32 FCoinProxy::GetOffsetNum() const
{
	return OffsetNum;
}
