
#include "EmptyLayout.h"

#include "LayoutCommon.h"
#include "MainMenuLayout.h"

#include "Components/WidgetSwitcher.h"

#include "UICommon.h"
#include "UIInterfaces.h"
#include "MenuInterface.h"

struct FMenuLayout : public TStructVariable<FMenuLayout>
{
	FName WidgetSwitcher = TEXT("WidgetSwitcher");
};

ELayoutCommon UEmptyLayout::GetLayoutType() const
{
	return ELayoutCommon::kEmptyLayout;
}
