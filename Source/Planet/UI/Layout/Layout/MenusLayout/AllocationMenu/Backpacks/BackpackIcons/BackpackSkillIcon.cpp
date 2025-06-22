#include "BackpackSkillIcon.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Containers/UnrealString.h"
#include "Components/SizeBox.h"
#include "Engine/Texture2D.h"
#include "ToolsLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/ProgressBar.h"

#include "StateTagExtendInfo.h"
#include "ItemProxyDragDropOperation.h"
#include "ItemProxyDragDropOperationWidget.h"
#include "ItemProxy_Minimal.h"
#include "GameplayTagsLibrary.h"

UBackpackSkillIcon::UBackpackSkillIcon(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
}

void UBackpackSkillIcon::NativeDestruct()
{
	DelageteHandle.Reset();
	
	Super::NativeDestruct();
}

void UBackpackSkillIcon::InvokeReset(
	UUserWidget* BaseWidgetPtr
	)
{
	if (BaseWidgetPtr)
	{
		auto NewPtr = Cast<ThisClass>(BaseWidgetPtr);
		if (NewPtr)
		{
			ResetToolUIByData(NewPtr->ProxyPtr);
		}
	}
}

void UBackpackSkillIcon::ResetToolUIByData(
	const TSharedPtr<FBasicProxy>& InBasicProxyPtr
	)
{
	Super::ResetToolUIByData(InBasicProxyPtr);

	if (InBasicProxyPtr)
	{
		if (
			InBasicProxyPtr->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Active)
		)
		{
			ProxyPtr = DynamicCastSharedPtr<FSkillProxy>(InBasicProxyPtr);
			
			SetLevel(-1);

			return;
		}
		
		if (
			InBasicProxyPtr->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Passve)
		)
		{
			auto TempProxyPtr = DynamicCastSharedPtr<FPassiveSkillProxy>(InBasicProxyPtr);

			DelageteHandle = TempProxyPtr->LevelChangedDelegate.AddOnValueChanged(
				 std::bind(&ThisClass::SetLevel, this, std::placeholders::_2)
				);
			SetLevel(TempProxyPtr->GetLevel());

			ProxyPtr = TempProxyPtr;
			return;
		}
	}
}

void UBackpackSkillIcon::EnableIcon(
	bool bIsEnable
	)
{
}

void UBackpackSkillIcon::SetNum(
	int32 NewNum
	)
{
	auto NumTextPtr = Cast<UTextBlock>(GetWidgetFromName(TEXT("Number")));
	if (!NumTextPtr)
	{
		return;
	}
	if (NewNum > 0)
	{
		const auto NumStr = FString::Printf(TEXT("%d"), NewNum);

		NumTextPtr->SetText(FText::FromString(NumStr));
	}
	else
	{
		NumTextPtr->SetText(FText::FromString(TEXT("")));
	}
}

void UBackpackSkillIcon::SetValue(
	int32 Value
	)
{
	auto ProgressBarPtr = Cast<UProgressBar>(GetWidgetFromName(TEXT("ProgressBar")));
	if (!ProgressBarPtr)
	{
		return;
	}
}

void UBackpackSkillIcon::SetLevel(
	int32 InLevel
	)
{
	if (LevelText)
	{
		if (InLevel>0)
		{
			LevelText->SetVisibility(ESlateVisibility::Visible);
			LevelText->SetText(FText::FromString(FString::Printf(TEXT("%d"), InLevel)));
		}
		else
		{
			LevelText->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UBackpackSkillIcon::ResetSize(
	const FVector2D& Size
	)
{
	auto SizeBoxPtr = Cast<USizeBox>(GetWidgetFromName(TEXT("IconSize")));
	if (SizeBoxPtr)
	{
		SizeBoxPtr->SetWidthOverride(Size.X);
		SizeBoxPtr->SetHeightOverride(Size.Y);
	}
}

void UBackpackSkillIcon::NativeConstruct()
{
	Super::NativeConstruct();
}
