
#include "MenuNavgationItem.h"

#include "Components/HorizontalBox.h"
#include "Components/Button.h"

#include "TemplateHelper.h"
#include "MenuNavgationItem.h"

#include "HumanViewTalentAllocation.h"
#include "InputProcessorSubSystemBase.h"
#include "MainMenuLayout.h"
#include "UIManagerSubSystem.h"
#include "HumanCharacter_Player.h"
#include "InputProcessorSubSystem_Imp.h"

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
	switch (MenuType) {
	case EMenuType::kEmpty:
		break;
	case EMenuType::kAllocationSkill:
		break;
	case EMenuType::kAllocationTalent:
		{
			UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<HumanProcessor::FHumanViewTalentAllocation>();
			return;
		}
		break;
	case EMenuType::kGroupManagger:
		break;
	case EMenuType::kRaffle:
		break;
	case EMenuType::kViewTask:
		break;
	case EMenuType::kViewMap:
		break;
	case EMenuType::kViewSetting:
		break;
	}
	UUIManagerSubSystem::GetInstance()->SwitchMenuLayout(MenuType);
}

