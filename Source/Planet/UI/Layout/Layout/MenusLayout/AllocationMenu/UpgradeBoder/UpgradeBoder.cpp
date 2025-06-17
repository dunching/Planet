#include "UpgradeBoder.h"

#include "PlanetPlayerController.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"

#include "GameplayTagsLibrary.h"
#include "MaterialIcon.h"
#include "InventoryComponent.h"
#include "Components/Button.h"
#include "Components/ProgressBar.h"

void UUpgradeBoder::NativeConstruct()
{
	Super::NativeConstruct();

	UpgradeBtn->OnClicked.AddDynamic(this, &ThisClass::OnClickedUpgradeBtn);
	CancelBtn->OnClicked.AddDynamic(this, &ThisClass::OnClickedCancelBtn);
}

void UUpgradeBoder::BindData(
	const TSharedPtr<FPassiveSkillProxy>& InProxySPtr
	)
{
	ProxySPtr = InProxySPtr;

	if (ProxySPtr)
	{
		CurrentLevelText->SetText(FText::FromString(FString::Printf(TEXT("%d"), ProxySPtr->GetLevel())));

		ProgressBar->SetPercent(InProxySPtr->GetExperience() / static_cast<float>(ProxySPtr->GetLevelExperience()));

		ScrollBox->ClearChildren();

		auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
		if (PCPtr)
		{
			auto ProxyAry = PCPtr->GetInventoryComponent()->FindAllProxyType(
			                                                                 UGameplayTagsLibrary::Proxy_Material_PassiveSkill_Experience_Book
			                                                                );
			for (auto Iter : ProxyAry)
			{
				if (Iter)
				{
					auto UIPtr = CreateWidget<UMaterialIcon>(this, MaterialIconClass);
					if (UIPtr)
					{
						UIPtr->BindData(Iter);

						ScrollBox->AddChild(UIPtr);
					}
				}
			}
		}
	}
}

void UUpgradeBoder::OnClickedUpgradeBtn()
{
	if (OffsetLevel > 0)
	{
		
	}
	else
	{
		// 无效.
	}
}

void UUpgradeBoder::OnClickedCancelBtn()
{
	RemoveFromParent();
}
