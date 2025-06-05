#include "PlayerConversationBorder.h"

#include <Components/GridPanel.h>
#include <Kismet/GameplayStatics.h>
#include "Components/TextBlock.h"

#include "ActionConsumablesIcon.h"
#include "CharacterBase.h"
#include "GuideActorBase.h"
#include "GuideSubSystem.h"
#include "HumanCharacter_Player.h"
#include "ProxyProcessComponent.h"

#include "GuideList.h"
#include "PawnStateConsumablesHUD.h"
#include "STT_CommonData.h"
#include "Components/Border.h"
#include "Components/Button.h"

struct FPlayerConversationBorder : public TStructVariable<FPlayerConversationBorder>
{
	const FName Text = TEXT("Text");
	
	const FName Border = TEXT("Border");
};

void UPlayerConversationBorder::NativeConstruct()
{
	Super::NativeConstruct();

	auto UIPtr = Cast<UButton>(GetWidgetFromName(FPlayerConversationBorder::Get().Border));
	if (!UIPtr)
	{
		return;
	}
	UIPtr->OnClicked.AddDynamic(this, &ThisClass::On_Skip_Clicked);

	EndBorder();
}

void UPlayerConversationBorder::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
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

void UPlayerConversationBorder::Enable()
{
	ILayoutItemInterfacetion::Enable();
	
	auto PlayerCharacterPtr = Cast<AHumanCharacter_Player>(UGameplayStatics::GetPlayerCharacter(this, 0));

	OnPlayerHaveNewSentenceDelegateHandle = PlayerCharacterPtr->GetPlayerConversationComponent()->OnPlayerHaveNewSentence.AddUObject(
		this, &ThisClass::OnPlayerHaveNewSentence);
}

void UPlayerConversationBorder::DisEnable()
{
	auto PlayerCharacterPtr = Cast<AHumanCharacter_Player>(UGameplayStatics::GetPlayerCharacter(this, 0));

	PlayerCharacterPtr->GetPlayerConversationComponent()->OnPlayerHaveNewSentence.Remove(OnPlayerHaveNewSentenceDelegateHandle);
	
	ILayoutItemInterfacetion::DisEnable();
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

void UPlayerConversationBorder::On_Skip_Clicked()
{
	auto PlayerCharacterPtr = Cast<AHumanCharacter_Player>(UGameplayStatics::GetPlayerCharacter(this, 0));

	PlayerCharacterPtr->GetPlayerConversationComponent()->SentenceStop();
}
