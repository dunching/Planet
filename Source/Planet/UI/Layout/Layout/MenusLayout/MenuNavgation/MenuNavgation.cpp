
#include "MenuNavgation.h"

#include "Components/HorizontalBox.h"

#include "TemplateHelper.h"
#include "MenuNavgationItem.h"
#include "MainMenuLayout.h"
#include "UIManagerSubSystem.h"

struct FMenuNavgation : public TStructVariable<FMenuNavgation>
{
	FName HorizontalBox = TEXT("HorizontalBox");
};

void UMenuNavgation::NativeConstruct()
{
	Super::NativeConstruct();

	auto UIPtr = Cast<UHorizontalBox>(GetWidgetFromName(FMenuNavgation::Get().HorizontalBox));
	if (UIPtr)
	{
		auto Ary = UIPtr->GetAllChildren();
		for (auto Iter : Ary)
		{
			auto MenuNavgationItemPtr = Cast<UMenuNavgationItem>(Iter);
			if (MenuNavgationItemPtr)
			{
			}
		}
	}
}

