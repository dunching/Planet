#include "LayoutInterfacetion.h"

void ILayoutInterfacetion::Enable()
{
	bIsActive = true;
}

void ILayoutInterfacetion::DisEnable()
{
	if (OnQuit)
	{
		OnQuit();
		OnQuit = FOnQuit();
	}
	
	bIsActive = false;
}

ELayoutCommon ILayoutItemInterfacetion::GetLayoutType() const
{
	return ELayoutCommon::kEmptyLayout;
}

