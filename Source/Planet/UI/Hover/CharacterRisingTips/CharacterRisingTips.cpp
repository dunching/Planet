#include "CharacterRisingTips.h"

#include <Kismet/GameplayStatics.h>
#include <Blueprint/WidgetLayoutLibrary.h>
#include <Components/SizeBox.h>
#include <Components/VerticalBox.h>
#include <Components/TextBlock.h>
#include <Components/Image.h>
#include "Components/WidgetSwitcher.h"

#include "CharacterBase.h"
#include "PlanetModule.h"
#include "GameOptions.h"
#include "GameplayTagsLibrary.h"
#include "OnEffectedTawrgetCallback.h"
#include "PlanetGameViewportClient.h"
#include "TemplateHelper.h"
#include "TextSubSystem.h"
#include "TextCollect.h"
#include "Components/Button.h"
#include "Kismet/KismetStringLibrary.h"

struct FCharacterRisingTips : public TStructVariable<FCharacterRisingTips>
{
	const FName VerticalBox = TEXT("VerticalBox");

	const FName Icon_Treatment = TEXT("Icon_Treatment");

	const FName Text = TEXT("Text");
};

void UCharacterRisingTips::NativeConstruct()
{
	Super::NativeConstruct();

	SetAnchorsInViewport(FAnchors(.5f));
	SetAlignmentInViewport(FVector2D(.5f, .5f));
}

bool UCharacterRisingTips::SetData(
	const FOnEffectedTawrgetCallback& ReceivedEventModifyDataCallback
	)
{
	if (!ReceivedEventModifyDataCallback.TargetCharacterPtr)
	{
		return false;
	}

	if (ReceivedEventModifyDataCallback.AllAssetTags.HasTag(UGameplayTagsLibrary::DataSource_Reply))
	{
		return false;
	}

	TargetCharacterPtr = ReceivedEventModifyDataCallback.TargetCharacterPtr;

	auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FCharacterRisingTips::Get().Text));
	if (UIPtr)
	{
		if (ReceivedEventModifyDataCallback.
			bIsEvade)
		{
			const auto Text = FText::FromString(TEXT("闪避"));

			UIPtr->SetText(Text);

			PlayDamageAnimation(
			                    ReceivedEventModifyDataCallback.
			                    bIsCritical,
			                    ReceivedEventModifyDataCallback.
			                    bIsEvade,
			                    ReceivedEventModifyDataCallback.
			                    ElementalType
			                   );
		}
		else if (ReceivedEventModifyDataCallback.
		         Damage > 0)
		{
			const auto Text = FText::FromString(FString::Printf(TEXT("%d"), ReceivedEventModifyDataCallback.Damage));

			UIPtr->SetText(Text);

			PlayDamageAnimation(
			                    ReceivedEventModifyDataCallback.
			                    bIsCritical,
			                    ReceivedEventModifyDataCallback.
			                    bIsEvade,
			                    ReceivedEventModifyDataCallback.
			                    ElementalType
			                   );
		}
		else if (ReceivedEventModifyDataCallback.
		         TherapeuticalDose > 0)
		{
			const auto Text = FText::FromString(FString::Printf(TEXT("+%d"), ReceivedEventModifyDataCallback.Damage));

			UIPtr->SetText(Text);

			PlayTreatmentAnimation();
		}
		else
		{
			for (const auto& Iter : ReceivedEventModifyDataCallback.SetByCallerTagMagnitudes)
			{
				if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Stamina))
				{
					if (Iter.Value > 0)
					{
						const auto Text = FText::FromString(FString::Printf(TEXT("+%.0lf"), Iter.Value));

						UIPtr->SetText(Text);
					}
					else
					{
						const auto Text = FText::FromString(FString::Printf(TEXT("%.0lf"), Iter.Value));

						UIPtr->SetText(Text);
					}

					PlayStaminaAnimation();
				}
				else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Mana))
				{
					if (Iter.Value > 0)
					{
						const auto Text = FText::FromString(FString::Printf(TEXT("+%.0lf"), Iter.Value));

						UIPtr->SetText(Text);
					}
					else
					{
						const auto Text = FText::FromString(FString::Printf(TEXT("%.0lf"), Iter.Value));

						UIPtr->SetText(Text);
					}

					PlayManaAnimation();
				}
			}
		}
	}

	return true;
}

FVector UCharacterRisingTips::GetHoverPosition()
{
	return TargetCharacterPtr->GetActorLocation();
}

void UCharacterRisingTips::PlayAnimationFinished()
{
	EndRising();
}

void UCharacterRisingTips::EndRising()
{
	auto ScreenLayer = UKismetGameLayerManagerLibrary::GetGameLayer<FHoverWidgetScreenLayer>(
		 GetWorld(),
		 TargetPointSharedLayerName
		);
	if (ScreenLayer)
	{
		ScreenLayer->RemoveHoverWidget(this);
	}
}
