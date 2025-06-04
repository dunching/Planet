#include "InteractionConversationLayout.h"

#include "Components/Button.h"

#include "InputProcessorSubSystemBase.h"
#include "MainHUD.h"
#include "PlayerConversationBorder.h"
#include "HumanRegularProcessor.h"
#include "HumanCharacter_Player.h"
#include "InputProcessorSubSystem_Imp.h"

class AMainHUD;

namespace HumanProcessor
{
	class FHumanRegularProcessor;
	class FHumanInteractionWithNPCProcessor;
}

struct FConversationLayout : public TStructVariable<FConversationLayout>
{
	FName InteractionList = TEXT("InteractionList");

	FName PlayerConversationBorder = TEXT("PlayerConversationBorder");

	FName QuitBtn = TEXT("QuitBtn");
};

void UInteractionConversationLayout::NativeConstruct()
{
	Super::NativeConstruct();

	{
		auto UIPtr = Cast<UButton>(
			GetWidgetFromName(FConversationLayout::Get().QuitBtn)
		);
		if (UIPtr)
		{
			UIPtr->OnClicked.AddDynamic(this, &ThisClass::OnQuitBtnClicked);
		}
	}

	Enable();
}

void UInteractionConversationLayout::Enable()
{
	{
		auto UIPtr = Cast<UPlayerConversationBorder>(
			GetWidgetFromName(FConversationLayout::Get().PlayerConversationBorder)
		);
		if (UIPtr)
		{
			UIPtr->Enable();
		}
	}
}

ELayoutCommon UInteractionConversationLayout::GetLayoutType() const
{
	return ELayoutCommon::kConversationLayout;
}

void UInteractionConversationLayout::OnQuitBtnClicked()
{
	UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<HumanProcessor::FHumanRegularProcessor>();
}
