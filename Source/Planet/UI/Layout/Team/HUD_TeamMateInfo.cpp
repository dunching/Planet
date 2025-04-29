#include "HUD_TeamMateInfo.h"

#include <Kismet/GameplayStatics.h>
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Containers/UnrealString.h"
#include "Components/SizeBox.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Engine/Texture2D.h"
#include "ToolsLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/ProgressBar.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Components/Border.h"

#include <StateTagExtendInfo.h>
#include "AssetRefMap.h"
#include "ItemProxyDragDropOperation.h"
#include "ItemProxyDragDropOperationWidget.h"
#include "ItemProxy_Minimal.h"
#include "PlanetControllerInterface.h"
#include "HUD_TeamMateInfo.h"
#include "GameplayTagsLibrary.h"
#include "CharacterBase.h"
#include "TemplateHelper.h"
#include "ItemProxy_Character.h"

struct FHUD_TeamMateInfo : public TStructVariable<FHUD_TeamMateInfo>
{
	const FName Texture = TEXT("Texture");

	const FName Text = TEXT("Text");

	const FName Content = TEXT("Content");

	const FName Default = TEXT("Default");

	const FName TalentStateSocket = TEXT("TalentStateSocket");
};

void UHUD_TeamMateInfo::NativeConstruct()
{
	Super::NativeConstruct();

	ResetToolUIByData(nullptr);
}

void UHUD_TeamMateInfo::InvokeReset(
	UUserWidget* BaseWidgetPtr
)
{
}

void UHUD_TeamMateInfo::ResetToolUIByData(
	const TSharedPtr<FBasicProxy>& BasicProxyPtr
)
{
	if (BasicProxyPtr && BasicProxyPtr->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Character))
	{
		GroupMateProxyPtr = DynamicCastSharedPtr<FCharacterProxy>(BasicProxyPtr);
		{
			auto UIPtr = Cast<UImage>(GetWidgetFromName(FHUD_TeamMateInfo::Get().Texture));
			if (UIPtr)
			{
				AsyncLoadText(GroupMateProxyPtr->GetIcon(), UIPtr);
			}
		}
		{
			auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FHUD_TeamMateInfo::Get().Text));
			if (UIPtr)
			{
				auto CharacterAttributesSPtr =
					GroupMateProxyPtr->CharacterAttributesSPtr;
				if (GroupMateProxyPtr->Name.IsEmpty())
				{
					UIPtr->SetText(
						FText::FromString(
							FString::Printf(
								TEXT("%s(%d)"),
								*GroupMateProxyPtr->Title,
								GroupMateProxyPtr->Level
							)
						)
					);
				}
				else
				{
					UIPtr->SetText(
						FText::FromString(
							FString::Printf(
								TEXT("%s %s(%d)"),
								*GroupMateProxyPtr->Title,
								*GroupMateProxyPtr->Name,
								GroupMateProxyPtr->Level
							)
						)
					);
				}
			}
		}

		auto PCPtr = Cast<IPlanetControllerInterface>(UGameplayStatics::GetPlayerController(this, 0));
		if (!PCPtr)
		{
			return;
		}
	}
}

void UHUD_TeamMateInfo::EnableIcon(
	bool bIsEnable
)
{
}
