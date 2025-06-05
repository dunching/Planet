#include "CharacterTitleComponent.h"

#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Planet_Tools.h"
#include "ConversationBorder.h"
#include "STT_CommonData.h"
#include "CharacterBase.h"
#include "CharacterTitle.h"
#include "GameOptions.h"

#include "Components/VerticalBox.h"

FName UCharacterTitleComponent::ComponentName = TEXT("CharacterTitleComponent");

UCharacterTitleComponent::UCharacterTitleComponent(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 1.f;
}

void UCharacterTitleComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCharacterTitleComponent::TickComponent(
	float DeltaTime,
	enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction
	)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

#if UE_EDITOR || UE_CLIENT
	if (GetOwnerRole() < ROLE_Authority)
	{
		auto PCPtr = GEngine->GetFirstLocalPlayerController(GetWorld());
		if (PCPtr)
		{
			const auto Distance = FVector::Dist(GetOwner()->GetActorLocation(), PCPtr->GetPawn()->GetActorLocation());
			SetVisibility(Distance < UGameOptions::GetInstance()->CharacterTitleDisplayRange);
		}
		// ResetPosition(DeltaTime);
	}
#endif
}

void UCharacterTitleComponent::OnGroupManaggerReady(
	AGroupManagger* NewGroupSharedInfoPtr
	)
{
#if UE_EDITOR || UE_CLIENT
	if (GetOwnerRole() < ROLE_Authority)
	{
		auto OwnerCharacterOtr = GetOwner<FOwnerType>();

		float Radius = 0.f;
		OwnerCharacterOtr->GetCapsuleComponent()->GetScaledCapsuleSize(Radius, HalfHeight);

		auto CharacterTitleBoxPtr = Cast<UCharacterTitleBox>(GetUserWidgetObject());
		if (CharacterTitleBoxPtr)
		{
			CharacterTitleBoxPtr->SetData(OwnerCharacterOtr);
			// if (GetOwnerRole() == ROLE_AutonomousProxy)
			// {
			// 	CharacterTitleBoxPtr->AddToViewport(EUIOrder::kPlayer_Character_State_HUD);
			// }
			// else
			// {
			// 	CharacterTitleBoxPtr->AddToViewport(EUIOrder::kOtherPlayer_Character_State_HUD);
			// }
		}
	}
#endif
}

void UCharacterTitleComponent::SetCampType(
	ECharacterCampType CharacterCampType
	)
{
	auto CharacterTitleBoxPtr = Cast<UCharacterTitleBox>(GetUserWidgetObject());
	if (CharacterTitleBoxPtr)
	{
		CharacterTitleBoxPtr->SetCampType(CharacterCampType);
	}
}

bool UCharacterTitleComponent::ResetPosition(
	float InDeltaTime
	)
{
	// if (CharacterTitleBoxPtr)
	// {
	// 	FVector2D ScreenPosition = FVector2D::ZeroVector;
	//
	// 	auto OwnerCharacterOtr = GetOwner<FOwnerType>();
	//
	// 	UGameplayStatics::ProjectWorldToScreen(
	// 		UGameplayStatics::GetPlayerController(this, 0),
	// 		OwnerCharacterOtr->GetActorLocation() - (OwnerCharacterOtr->GetGravityDirection() * (Offset + HalfHeight)),
	// 		ScreenPosition
	// 	);
	//
	// 	CharacterTitleBoxPtr->SetPositionInViewport(ScreenPosition);
	// }

	return true;
}

void UCharacterTitleComponent::DisplaySentence(
	const FTaskNode_Conversation_SentenceInfo& Sentence
	)
{
	auto CharacterTitleBoxPtr = Cast<UCharacterTitleBox>(GetUserWidgetObject());
	if (!CharacterTitleBoxPtr)
	{
		return;
	}
	CharacterTitleBoxPtr->DisplaySentence(Sentence);
}

void UCharacterTitleComponent::CloseConversationborder()
{
	auto CharacterTitleBoxPtr = Cast<UCharacterTitleBox>(GetUserWidgetObject());
	if (!CharacterTitleBoxPtr)
	{
		return;
	}
	CharacterTitleBoxPtr->CloseConversationborder();
}
