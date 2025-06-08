#include "ViewMapMenu.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "PlanetPlayerController.h"
#include "PlayerGameplayTasks.h"
#include "Teleport.h"
#include "TeleportPointWidget.h"
#include "OpenWorldSystem.h"

void UViewMapMenu::NativeConstruct()
{
	Super::NativeConstruct();

	if (TeleportBtn)
	{
		TeleportBtn->OnClicked.AddDynamic(this, &UViewMapMenu::OnTeleportClicked);
	}
}

FReply UViewMapMenu::NativeOnMouseButtonDown(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent
	)
{
	if (TeleportDecriptionBorder)
	{
		TeleportDecriptionBorder->SetVisibility(ESlateVisibility::Collapsed);
	}
	TeleportPtr = nullptr;

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UViewMapMenu::NativeOnMouseButtonUp(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent
	)
{
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

inline void UViewMapMenu::EnableMenu()
{
	if (TeleportDecriptionBorder)
	{
		TeleportDecriptionBorder->SetVisibility(ESlateVisibility::Collapsed);
	}

	WidgetTree->ForEachWidget(
	                          [this](
	                          UWidget* Widget
	                          )
	                          {
		                          if (Widget && Widget->IsA<UTeleportPointWidget>())
		                          {
			                          auto UIPtr = Cast<UTeleportPointWidget>(Widget);
			                          if (!UIPtr)
			                          {
				                          return;
			                          }

			                          UIPtr->OnClicked.BindUObject(this, &ThisClass::OnTeleportPointWidgetClicked);
		                          }
	                          }
	                         );
}

void UViewMapMenu::DisEnableMenu()
{
	WidgetTree->ForEachWidget(
	                          [this](
	                          UWidget* Widget
	                          )
	                          {
		                          if (Widget && Widget->IsA<UTeleportPointWidget>())
		                          {
			                          auto UIPtr = Cast<UTeleportPointWidget>(Widget);
			                          if (!UIPtr)
			                          {
				                          return;
			                          }

			                          UIPtr->OnClicked.Unbind();
		                          }
	                          }
	                         );
}

inline EMenuType UViewMapMenu::GetMenuType() const
{
	return EMenuType::kViewMap;
}

void UViewMapMenu::OnTeleportPointWidgetClicked(
	UTeleportPointWidget* TeleportPointWidgetPtr
	)
{
	if (TeleportPointWidgetPtr)
	{
		if (!TeleportDecriptionBorder)
		{
			return;
		}

		if (!TeleportPointWidgetPtr->TeleportRef.ToSoftObjectPath().IsSubobject())
		{
			return;
		}

		TeleportDecriptionBorder->SetVisibility(ESlateVisibility::Visible);

		TeleportPtr = TeleportPointWidgetPtr->TeleportRef.LoadSynchronous();
		if (!TeleportPtr)
		{
			return;
		}

		if (TeleportName)
		{
			TeleportName->SetText(FText::FromString(TeleportPtr->Name));
		}
		if (TeleportDescription)
		{
			TeleportDescription->SetText(
			                             FText::FromString(
			                                               TeleportPtr->Description
			                                              )
			                            );
		}
	}
}

void UViewMapMenu::OnTeleportClicked()
{
	if (!TeleportPtr)
	{
		return;
	}

	auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
	if (!PCPtr)
	{
		return;
	}

	const auto TeleportType = UOpenWorldSubSystem::GetInstance()->GetTeleportType(
		 TeleportPtr
		);
	PCPtr->GetGameplayTasksComponent()->EntryChallengeLevel(
	                                                        TeleportType
	                                                       );
}
