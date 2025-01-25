#include "CharacterTitleComponent.h"

#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Planet_Tools.h"
#include "ConversationBorder.h"
#include "UICommon.h"
#include "CharacterBase.h"
#include "CharacterTitle.h"
#include "TaskNode.h"
#include "Components/VerticalBox.h"

struct FCharacterTitleBox : public TStructVariable<FCharacterTitleBox>
{
	const FName VerticalBox = TEXT("VerticalBox");

	const FName CharacterTitle = TEXT("CharacterTitle");
};

FName UCharacterTitleComponent::ComponentName = TEXT("CharacterTitleComponent");

void UCharacterTitleBox::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UCharacterTitleBox::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetAnchorsInViewport(FAnchors(.5f));
	SetAlignmentInViewport(FVector2D(.5f, 1.f));
	
	auto UIPtr = Cast<UCharacterTitle>(GetWidgetFromName(FCharacterTitleBox::Get().CharacterTitle));
	if (UIPtr)
	{
		UIPtr->SetData(CharacterPtr);
	}
}

UCharacterTitleComponent::UCharacterTitleComponent(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
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
		ResetPosition(DeltaTime);
	}
#endif
}

void UCharacterTitleComponent::OnGroupSharedInfoReady(AGroupSharedInfo* NewGroupSharedInfoPtr)
{
#if UE_EDITOR || UE_CLIENT
	if (GetOwnerRole() < ROLE_Authority)
	{
		auto OwnerCharacterOtr = GetOwner<FOwnerType>();
	
		float Radius = 0.f;
		OwnerCharacterOtr->GetCapsuleComponent()->GetScaledCapsuleSize(Radius, HalfHeight);

		CharacterTitleBoxPtr = CreateWidget<UCharacterTitleBox>(GetWorld(), CharacterTitleBoxClass);
		if (CharacterTitleBoxPtr)
		{
			CharacterTitleBoxPtr->CharacterPtr = OwnerCharacterOtr;
			if (GetOwnerRole() == ROLE_AutonomousProxy)
			{
				CharacterTitleBoxPtr->AddToViewport(EUIOrder::kPlayer_Character_State_HUD);
			}
			else
			{
				CharacterTitleBoxPtr->AddToViewport(EUIOrder::kOtherPlayer_Character_State_HUD);
			}
			auto UIPtr = Cast<UVerticalBox>(CharacterTitleBoxPtr->GetWidgetFromName(FCharacterTitleBox::Get().VerticalBox));
			if (UIPtr)
			{
				UIPtr->ClearChildren();
			}
		}
	}
#endif
}

void UCharacterTitleComponent::SetCampType(ECharacterCampType CharacterCampType)
{
	auto UIPtr = Cast<UCharacterTitle>(
		CharacterTitleBoxPtr->GetWidgetFromName(FCharacterTitleBox::Get().CharacterTitle)
		);
	if (UIPtr)
	{
		UIPtr->SetCampType(CharacterCampType);
	}
}

bool UCharacterTitleComponent::ResetPosition(float InDeltaTime)
{
	if (CharacterTitleBoxPtr)
	{
		FVector2D ScreenPosition = FVector2D::ZeroVector;

		auto OwnerCharacterOtr = GetOwner<FOwnerType>();
	
		UGameplayStatics::ProjectWorldToScreen(
			UGameplayStatics::GetPlayerController(this, 0),
			OwnerCharacterOtr->GetActorLocation() - (OwnerCharacterOtr->GetGravityDirection() * (Offset + HalfHeight)),
			ScreenPosition
		);

		CharacterTitleBoxPtr->SetPositionInViewport(ScreenPosition);
	}

	return true;
}

void UCharacterTitleComponent::DisplaySentence(const FTaskNode_Conversation_SentenceInfo& Sentence) 
{
	if (ConversationBorderPtr)
	{
		ConversationBorderPtr->SetSentence(Sentence);
	}
	else
	{
		ConversationBorderPtr = CreateWidget<UConversationBorder>(GetWorld(), ConversationBorderClass);
		if (ConversationBorderPtr)
		{
			ConversationBorderPtr->CharacterPtr = GetOwner<ACharacterBase>();
			ConversationBorderPtr->SetSentence(Sentence);
			auto UIPtr = Cast<UVerticalBox>(CharacterTitleBoxPtr->GetWidgetFromName(FCharacterTitleBox::Get().VerticalBox));
			if (UIPtr)
			{
				UIPtr->AddChild(ConversationBorderPtr);
			}
		}
	}
}

void UCharacterTitleComponent::CloseConversationborder()
{
	if (ConversationBorderPtr)
	{
		ConversationBorderPtr->RemoveFromParent();
		ConversationBorderPtr = nullptr;
	}
}
