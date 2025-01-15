
#include "MenuNavgationItem.h"

#include "Components/HorizontalBox.h"
#include "Components/Button.h"

#include "TemplateHelper.h"
#include "MenuNavgationItem.h"
#include "MainMenuLayout.h"
#include "UIManagerSubSystem.h"

struct FMenuNavgationItem : public TStructVariable<FMenuNavgationItem>
{
	FName Btn = TEXT("Btn");
};

void UMenuNavgationItem::NativeConstruct()
{
	Super::NativeConstruct();

	auto UIPtr = Cast<UButton>(GetWidgetFromName(FMenuNavgationItem::Get().Btn));
	if (UIPtr)
	{
		UIPtr->OnClicked.AddDynamic(this, &ThisClass::OnClicked);
	}
}

void UMenuNavgationItem::OnClicked()
{
	switch (MenuType)
	{
	case EMenuType::kAllocationSkill:
	{
		UUIManagerSubSystem::GetInstance()->ViewBackpack(true);
	}
	break;
	case EMenuType::kAllocationTalent:
	{
		UUIManagerSubSystem::GetInstance()->ViewTalentAllocation(true);
	}
	break;
	case EMenuType::kGroupManagger:
	{
		UUIManagerSubSystem::GetInstance()->ViewGroupMatesManagger(true);
	}
	break;
	case EMenuType::kRaffle:
	{
		UUIManagerSubSystem::GetInstance()->ViewRaffleMenu(true);
	}
	break;
	}
}

