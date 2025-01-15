
#include "ConversationBorder.h"

#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"

#include "CharacterBase.h"

struct FConversationBorder : public TStructVariable<FConversationBorder>
{
	const FName Text = TEXT("Text");
};

void UConversationBorder::NativeConstruct()
{
	Super::NativeConstruct();
 	
	// SetAnchorsInViewport(FAnchors(.5f, 1.f));
	// SetAlignmentInViewport(FVector2D(.5f, 1.f));
	//
	// if (CharacterPtr)
	// {
	// 	float Radius = 0.f;
	// 	CharacterPtr->GetCapsuleComponent()->GetScaledCapsuleSize(Radius, HalfHeight);
	// }

	// TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &ThisClass::ResetPosition));
}

void UConversationBorder::NativeDestruct()
 {
 	FTSTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);

 	Super::NativeDestruct();
}

void UConversationBorder::SetSentence(const FTaskNode_Conversation_SentenceInfo& InSentence)
{
	Sentence = InSentence;

	auto WidgetPtr = Cast<UTextBlock>(GetWidgetFromName(FConversationBorder::Get().Text));
	if (WidgetPtr)
	{
		WidgetPtr->SetText(FText::FromString(Sentence.Sentence));
	}
}

bool UConversationBorder::ResetPosition(float InDeltaTime)
 {
 	FVector2D ScreenPosition = FVector2D::ZeroVector;
 	UGameplayStatics::ProjectWorldToScreen(
		 UGameplayStatics::GetPlayerController(this, 0),
		 CharacterPtr->GetActorLocation() - (CharacterPtr->GetGravityDirection() * (Offset + HalfHeight)),
		 ScreenPosition
	 );

 	SetPositionInViewport(ScreenPosition);

 	return true;
}
