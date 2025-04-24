
#include "ViewTaskMenu.h"

struct FUViewTaskMenu : public TStructVariable<FUViewTaskMenu>
{
	FName Btn = TEXT("Btn");
	
	FName Image = TEXT("Image");
	
	FName Text = TEXT("Text");
};

void UViewTaskMenu::NativeConstruct()
{
	Super::NativeConstruct();
}

void UViewTaskMenu::NativeDestruct()
{
	Super::NativeDestruct();
}

inline void UViewTaskMenu::ResetUIByData()
{
}

inline void UViewTaskMenu::SyncData()
{
}

inline EMenuType UViewTaskMenu::GetMenuType() const
{
	return EMenuType::kViewTask;
}
