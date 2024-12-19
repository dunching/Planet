
#include "ItemProxy_Coin.h"

#include "AllocationSkills.h"
#include "ItemProxy_Container.h"
#include "TeamMatesHelperComponent.h"

FCoinProxy::FCoinProxy()
{

}

void FCoinProxy::UpdateByRemote(const TSharedPtr<FCoinProxy>& RemoteSPtr)
{
	Super::UpdateByRemote(RemoteSPtr);

	Num = RemoteSPtr->Num;
}

void FCoinProxy::AddCurrentValue(int32 val)
{
	const auto Old = Num;
	Num += val;

	CallbackContainerHelper.ValueChanged(Old, Num);
}

int32 FCoinProxy::GetCurrentValue() const
{
	return Num;
}
