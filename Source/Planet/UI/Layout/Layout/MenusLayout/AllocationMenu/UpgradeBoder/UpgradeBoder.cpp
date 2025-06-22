#include "UpgradeBoder.h"

#include "PlanetPlayerController.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"

#include "GameplayTagsLibrary.h"
#include "MaterialIcon.h"
#include "InventoryComponent.h"
#include "ModifyItemProxyStrategy.h"
#include "Components/Button.h"
#include "Components/ProgressBar.h"

void UUpgradeBoder::NativeConstruct()
{
	Super::NativeConstruct();

	UpgradeBtn->OnClicked.AddDynamic(this, &ThisClass::OnClickedUpgradeBtn);
	CancelBtn->OnClicked.AddDynamic(this, &ThisClass::OnClickedCancelBtn);
}

void UUpgradeBoder::NativeDestruct()
{
	OnProxyChangedDelegateHandle.Reset();

	Super::NativeDestruct();
}

void UUpgradeBoder::BindData(
	const TSharedPtr<FPassiveSkillProxy>& InProxySPtr
	)
{
	ProxySPtr = InProxySPtr;

	CurrentLevelText->SetText(FText::FromString(FString::Printf(TEXT("%d"), ProxySPtr->GetLevel())));

	const auto Level = ProxySPtr->GetLevel();
	const auto Experience = ProxySPtr->GetCurrentExperience();

	auto LevelExperience = ProxySPtr->GetLevelExperience(OffsetLevel + Level);

	OffsetProgressBar->SetPercent(0);

	if (Level >= ProxySPtr->GetMaxLevel())
	{
		ProgressBar->SetPercent(1.f);
	}
	else
	{
		ProgressBar->SetPercent(Experience / static_cast<float>(LevelExperience));
	}
	
	OffsetLevelText->SetText(FText::FromString(TEXT("")));

	ScrollBox->ClearChildren();

	auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
	if (PCPtr)
	{
		auto InventoryComponentPtr = PCPtr->GetInventoryComponent();

		auto ProxyAry = InventoryComponentPtr->FindProxyType<FModifyItemProxyStrategy_MaterialProxy>(
			 UGameplayTagsLibrary::Proxy_Material_PassiveSkill_Experience_Book
			);
		for (auto Iter : ProxyAry)
		{
			if (Iter)
			{
				auto UIPtr = CreateWidget<UMaterialIcon>(this, MaterialIconClass);
				if (UIPtr)
				{
					UIPtr->UpgradeBoderPtr = this;
					UIPtr->BindData(Iter);

					ScrollBox->AddChild(UIPtr);
				}
			}
		}

		auto ModifyItemProxyStrategy_MaterialProxySPtr = InventoryComponentPtr->GetModifyItemProxyStrategy<
			FModifyItemProxyStrategy_MaterialProxy>();
		if (ModifyItemProxyStrategy_MaterialProxySPtr)
		{
			OnProxyChangedDelegateHandle = ModifyItemProxyStrategy_MaterialProxySPtr->OnProxyChanged.AddCallback(
				 std::bind(
				           &ThisClass::OnProxyChanged,
				           this,
				           std::placeholders::_1,
				           std::placeholders::_2,
				           std::placeholders::_3
				          )
				);
		}
	}
}

int32 UUpgradeBoder::OnUpdateMaterial(
	const TSharedPtr<FMaterialProxy>& InProxySPtr,
	int32 Num,
	bool bIsAdd
	)
{
	int32 Result = Num;

	CosumeProxysSet.Add(InProxySPtr, Result);

	if (ProxySPtr)
	{
		CurrentLevelText->SetText(FText::FromString(FString::Printf(TEXT("%d"), ProxySPtr->GetLevel())));

		OffsetLevel = 0;

		const auto Level = ProxySPtr->GetLevel();
		const auto MaxLevel = ProxySPtr->GetMaxLevel();
		const auto Experience = ProxySPtr->GetCurrentExperience();

		auto LevelExperience = ProxySPtr->GetLevelExperience(Level);

		auto OffsetExperience = Experience;

		// 计算之前的
		for (const auto& Iter : CosumeProxysSet)
		{
			if (bIsAdd && (Iter.Key == InProxySPtr))
			{
				continue;
			}

			auto ExperienceMaterialProxySPtr = DynamicCastSharedPtr<FExperienceMaterialProxy>(Iter.Key);
			OffsetExperience += ExperienceMaterialProxySPtr->GetExperienceValue() * Iter.Value;

			for (; OffsetExperience > LevelExperience;)
			{
				OffsetLevel++;
				if ((OffsetLevel + Level) < MaxLevel)
				{
					OffsetExperience -= LevelExperience;
					LevelExperience = ProxySPtr->GetLevelExperience(OffsetLevel + Level);
				}
				else
				{
					break;
				}
			}

			if ((OffsetLevel + Level) < MaxLevel)
			{
			}
			else
			{
				break;
			}
		}

		// 计算新增的
		if (bIsAdd)
		{
			if ((OffsetLevel + Level) < MaxLevel)
			{
				auto ExperienceMaterialProxySPtr = DynamicCastSharedPtr<FExperienceMaterialProxy>(InProxySPtr);
				for (int32 Index = 1; Index <= Num; Index++)
				{
					OffsetExperience += ExperienceMaterialProxySPtr->GetExperienceValue();

					for (; OffsetExperience > LevelExperience;)
					{
						OffsetLevel++;
						if ((OffsetLevel + Level) < MaxLevel)
						{
							OffsetExperience -= LevelExperience;
							LevelExperience = ProxySPtr->GetLevelExperience(OffsetLevel + Level);
						}
						else
						{
							Result = Index;
							break;
						}
					}

					if ((OffsetLevel + Level) < MaxLevel)
					{
					}
					else
					{
						break;
					}
				}

				CosumeProxysSet.Add(InProxySPtr, Result);
			}
			else
			{
				Result = 0;
				CosumeProxysSet.Add(InProxySPtr, Result);
			}
		}

		if (OffsetLevel > 0)
		{
			OffsetProgressBar->SetPercent(OffsetExperience / static_cast<float>(LevelExperience));
			ProgressBar->SetPercent(0);
			OffsetLevelText->SetText(FText::FromString(FString::Printf(TEXT("+%d"), OffsetLevel)));
		}
		else
		{
			OffsetProgressBar->SetPercent(OffsetExperience / static_cast<float>(LevelExperience));
			ProgressBar->SetPercent(Experience / static_cast<float>(LevelExperience));
			OffsetLevelText->SetText(FText::FromString(TEXT("")));
		}
	}

	return Result;
}

void UUpgradeBoder::OnClickedUpgradeBtn()
{
	if (OffsetLevel > 0)
	{
		auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
		if (PCPtr)
		{
			TArray<FGuid> CosumeProxysIDS;
			TArray<uint32> CosumeProxyNums;

			for (auto Iter :CosumeProxysSet)
			{
				CosumeProxysIDS.Add(Iter.Key->GetID());
				CosumeProxyNums.Add(Iter.Value);
			}
			
			PCPtr->UpgradeSkill(ProxySPtr->GetID(), CosumeProxysIDS, CosumeProxyNums);
		}
	}
	else
	{
		// 无效.
	}
	
	CosumeProxysSet.Empty();
}

void UUpgradeBoder::OnClickedCancelBtn()
{
	RemoveFromParent();
}

void UUpgradeBoder::OnProxyChanged(
	const TSharedPtr<FMaterialProxy>& InProxySPtr,
	EProxyModifyType ProxyModifyType,
	int32 Num
	)
{
	switch (ProxyModifyType)
	{
	case EProxyModifyType::kNumChanged:
		break;
	case EProxyModifyType::kRemove:
		{
			if (ScrollBox)
			{
				const auto Childrens = ScrollBox->GetAllChildren();
				for (const auto& Iter : Childrens)
				{
					auto UIPtr = Cast<UMaterialIcon>(Iter);
					if (UIPtr && UIPtr->ProxySPtr == InProxySPtr)
					{
						UIPtr->RemoveFromParent();
						return;
					}
				}
			}
		}
		break;
	case EProxyModifyType::kPropertyChange:
		break;
	}
}
