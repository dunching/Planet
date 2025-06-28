#include "LayoutInterfacetion.h"

void ILayoutInterfacetion::Enable()
{
	bIsActive = true;

	RemovedWidgets();
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

UOverlaySlot* ILayoutInterfacetion::DisplayWidget(
	const TSubclassOf<UUserWidget>& WidgetClass,
	const std::function<void(UUserWidget*)>& Initializer
	)
{
	return nullptr;
}

bool ILayoutInterfacetion::RemovedWidgets()
{
	return false;
}

void ILayoutInterfacetion::RemoveWidget(
	const TSubclassOf<UUserWidget>& WidgetClass
	)
{
}

ELayoutCommon ILayoutItemInterfacetion::GetLayoutType() const
{
	return ELayoutCommon::kEmptyLayout;
}

