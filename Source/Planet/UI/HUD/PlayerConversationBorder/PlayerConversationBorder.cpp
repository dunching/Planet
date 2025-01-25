#include "PlayerConversationBorder.h"

#include <Components/GridPanel.h>
#include <Kismet/GameplayStatics.h>
#include "Components/TextBlock.h"

#include "ActionConsumablesIcon.h"
#include "CharacterBase.h"
#include "GuideActor.h"
#include "GuideSubSystem.h"
#include "HumanCharacter_Player.h"
#include "ProxyProcessComponent.h"
#include "TaskNode_Guide.h"
#include "GuideList.h"
#include "PawnStateConsumablesHUD.h"

struct FPlayerConversationBorder : public TStructVariable<FPlayerConversationBorder>
{
	const FName Text = TEXT("Text");
};

void UPlayerConversationBorder::NativeConstruct()
{
	Super::NativeConstruct();

	EndBorder();

	ResetUIByData();
}

void UPlayerConversationBorder::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UPlayerConversationBorder::ResetUIByData()
{
	auto PlayerCharacterPtr = Cast<AHumanCharacter_Player>(UGameplayStatics::GetPlayerCharacter(this, 0));

	PlayerCharacterPtr->GetPlayerConversationComponent()->OnPlayerHaveNewSentence.AddUObject(
		this, &ThisClass::OnPlayerHaveNewSentence);
}

void UPlayerConversationBorder::SetSentence(const FTaskNode_Conversation_SentenceInfo& InSentence)
{
	auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FPlayerConversationBorder::Get().Text));
	if (!UIPtr)
	{
		return;
	}

	SetVisibility(ESlateVisibility::Visible);

	UIPtr->SetText(FText::FromString(InSentence.Sentence));
}

void UPlayerConversationBorder::EndBorder()
{
	SetVisibility(ESlateVisibility::Hidden);
}

void UPlayerConversationBorder::OnPlayerHaveNewSentence(bool bIsDisplay,
                                                        const FTaskNode_Conversation_SentenceInfo& Sentence)
{
	if (bIsDisplay)
	{
		SetSentence(Sentence);
	}
	else
	{
		EndBorder();
	}
}
