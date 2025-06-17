#include "SkillsIcon.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Containers/UnrealString.h"
#include <Kismet/GameplayStatics.h>
#include "Components/CanvasPanel.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Components/Border.h"
#include "Components/Overlay.h"
#include "Components/ProgressBar.h"
#include "Kismet/KismetStringLibrary.h"
#include "Components/SizeBox.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Engine/Texture2D.h"

#include "ToolsLibrary.h"
#include "BackpackIcon.h"
#include "StateTagExtendInfo.h"
#include "AssetRefMap.h"
#include "ItemProxyDragDropOperation.h"
#include "ItemProxyDragDropOperationWidget.h"
#include "CharacterBase.h"
#include "ProxyProcessComponent.h"
#include "Skill_Base.h"
#include "GameplayTagsLibrary.h"
#include "VisitorSubsystem.h"

namespace SkillsIcon
{
	const FName Content = TEXT("Content");

	const FName Enable = TEXT("Enable");

	const FName Icon = TEXT("Icon");
}

USkillsIcon::USkillsIcon(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
}

void USkillsIcon::ResetToolUIByData(
	const TSharedPtr<FBasicProxy>& InBasicProxyPtr
	)
{
	if (InBasicProxyPtr == DynamicCastSharedPtr<FBasicProxy>(BasicProxyPtr))
	{
		return;
	}

	auto PreviousProxyPtr = BasicProxyPtr;
	TSharedPtr<IProxy_Allocationble> NewProxyPtr = nullptr;
	if (InBasicProxyPtr && InBasicProxyPtr->GetProxyType().MatchesTag(ProxyType))
	{
		NewProxyPtr = DynamicCastSharedPtr<IProxy_Allocationble>(InBasicProxyPtr);
	}
	else
	{
		if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Passve))
		{
			auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
			if (!CharacterPtr)
			{
				return;
			}

			// 装备了可以在“被动”槽上备注“主动技能”的主动技能
			if (CharacterPtr->GetProxyProcessComponent()->FindActiveSkillByType(
			                                                                    UGameplayTagsLibrary::Proxy_Skill_Active_Switch
			                                                                   ).IsValid())
			{
				NewProxyPtr = DynamicCastSharedPtr<IProxy_Allocationble>(InBasicProxyPtr);
			}
		}
	}

	if (!bPaseInvokeOnResetProxyEvent)
	{
		OnResetProxy.ExcuteCallback(PreviousProxyPtr, NewProxyPtr, IconSocket);
	}

	BasicProxyPtr = NewProxyPtr;

	if (!bPaseInvokeOnResetProxyEvent)
	{
		OnResetData(this);
	}

	SetItemType();

	SetLevel();
}

void USkillsIcon::EnableIcon(
	bool bIsEnable
	)
{
	auto ImagePtr = Cast<UImage>(GetWidgetFromName(SkillsIcon::Enable));
	if (ImagePtr)
	{
		ImagePtr->SetVisibility(bIsEnable ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
	}
}

void USkillsIcon::SetLevel()
{
}

void USkillsIcon::NativeConstruct()
{
	Super::NativeConstruct();

	ResetToolUIByData(nullptr);
}

void USkillsIcon::NativeDestruct()
{
	Super::NativeDestruct();
}

void USkillsIcon::OnDragIcon(
	bool bIsDragging,
	const TSharedPtr<IProxy_Allocationble>& ProxyPtr
	)
{
	if (bIsDragging)
	{
		if (ProxyPtr && DynamicCastSharedPtr<FBasicProxy>(ProxyPtr)->GetProxyType().MatchesTag(ProxyType))
		{
			EnableIcon(true);
		}
		else
		{
			if (
				ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Passve) &&
				DynamicCastSharedPtr<FBasicProxy>(ProxyPtr)->GetProxyType().MatchesTag(
					 UGameplayTagsLibrary::Proxy_Skill_Active
					)
			)
			{
				auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
				if (!CharacterPtr)
				{
					return;
				}

				// 装备了可以在“被动”槽上备注“主动技能”的主动技能
				if (CharacterPtr->GetProxyProcessComponent()->FindActiveSkillByType(
					 UGameplayTagsLibrary::Proxy_Skill_Active_Switch
					).IsValid())
				{
					EnableIcon(true);

					return;
				}
			}

			EnableIcon(false);
		}
	}
	else
	{
		EnableIcon(true);
	}
}
