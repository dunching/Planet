#include "ViewTaskMenu.h"

#include "QuestSubSystem.h"
#include "PlanetWorldSettings.h"
#include "TaskItem.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"

#include "QuestChain.h"
#include "HumanRegularProcessor.h"
#include "InputProcessorSubSystemBase.h"
#include "TaskItemCategory.h"
#include "ProxyIcon.h"
#include "HumanCharacter_Player.h"
#include "InputProcessorSubSystem_Imp.h"

struct FUViewTaskMenu : public TStructVariable<FUViewTaskMenu>
{
	FName ActiveGuideThreadBtn = TEXT("ActiveGuideThreadBtn");

	FName ActiveGuideThreadText = TEXT("ActiveGuideThreadText");

	FName TaskDetails = TEXT("TaskDetails");

	FName TaskName = TEXT("TaskName");

	FName TaskDescription = TEXT("TaskDescription");

	FName TaskList = TEXT("TaskList");

	FName RewardList = TEXT("RewardList");
};

void UViewTaskMenu::NativeConstruct()
{
	Super::NativeConstruct();

	{
		auto UIPtr = Cast<UButton>(GetWidgetFromName(FUViewTaskMenu::Get().ActiveGuideThreadBtn));
		if (UIPtr)
		{
			UIPtr->OnClicked.AddDynamic(this, &ThisClass::OnActiveGuideThread);
		}
	}
}

void UViewTaskMenu::NativeDestruct()
{
	Super::NativeDestruct();
}

inline void UViewTaskMenu::EnableMenu()
{
	{
		auto UIPtr = Cast<UCanvasPanel>(GetWidgetFromName(FUViewTaskMenu::Get().TaskDetails));
		if (UIPtr)
		{
			UIPtr->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	{
		auto UIPtr = Cast<UScrollBox>(GetWidgetFromName(FUViewTaskMenu::Get().RewardList));
		if (UIPtr)
		{
			UIPtr->ClearChildren();
		}
	}
	{
		auto UIPtr = Cast<UScrollBox>(GetWidgetFromName(FUViewTaskMenu::Get().TaskList));
		if (UIPtr)
		{
			UIPtr->ClearChildren();
			{
				auto WeidgetPtr = CreateWidget<UTaskItemCategory>(this, TaskItemCategoryClass);
				if (WeidgetPtr)
				{
					WeidgetPtr->SetTaskType(EQuestChainType::kMain);
					UIPtr->AddChild(WeidgetPtr);
				}
			}
			{
				auto WeidgetPtr = CreateWidget<UTaskItem>(this, TaskItemClass);
				if (WeidgetPtr)
				{
					const auto CurrentMainGuideThread = UQuestSubSystem::GetInstance()->GetCurrentMainGuideThread();

					WeidgetPtr->OnSeleted.BindUObject(this, &ThisClass::OnSelected);
					WeidgetPtr->SetGuideThreadType(CurrentMainGuideThread);
					UIPtr->AddChild(WeidgetPtr);
				}
			}

			const auto GuideThreadAry = UQuestSubSystem::GetInstance()->GetGuideThreadAry();
			if (GuideThreadAry.IsEmpty())
			{
			}
			else
			{
				{
					auto WeidgetPtr = CreateWidget<UTaskItemCategory>(this, TaskItemCategoryClass);
					if (WeidgetPtr)
					{
						WeidgetPtr->SetTaskType(EQuestChainType::kBrand);
						UIPtr->AddChild(WeidgetPtr);
					}
				}
				{
					for (const auto& Iter : GuideThreadAry)
					{
						auto WeidgetPtr = CreateWidget<UTaskItem>(this, TaskItemClass);
						if (WeidgetPtr)
						{
							WeidgetPtr->OnSeleted.BindUObject(this, &ThisClass::OnSelected);
							WeidgetPtr->SetGuideThreadType(Iter);
							UIPtr->AddChild(WeidgetPtr);
						}
					}
				}
			}
		}
	}

	ActiveCurrentCorrespondingItem();
}

inline void UViewTaskMenu::DisEnableMenu()
{
}

inline EMenuType UViewTaskMenu::GetMenuType() const
{
	return EMenuType::kViewTask;
}

void UViewTaskMenu::ActiveCurrentCorrespondingItem()
{
	auto UIPtr = Cast<UScrollBox>(GetWidgetFromName(FUViewTaskMenu::Get().TaskList));
	if (UIPtr)
	{
		const auto CurrentGuideThread = UQuestSubSystem::GetInstance()->GetCurrentGuideThread();
		if (!CurrentGuideThread)
		{
			return;
		}
		auto ChildrensAry = UIPtr->GetAllChildren();
		for (const auto& Iter : ChildrensAry)
		{
			auto TempUIPtr = Cast<UTaskItem>(Iter);
			if (TempUIPtr)
			{
				if (TempUIPtr->MainGuideThreadClass && CurrentGuideThread->IsA(TempUIPtr->MainGuideThreadClass.Get()))
				{
					TempUIPtr->OnClicked();
					return;
				}
				if (TempUIPtr->BrandGuideThreadClass && CurrentGuideThread->IsA(TempUIPtr->BrandGuideThreadClass.Get()))
				{
					TempUIPtr->OnClicked();
					return;
				}
			}
		}
	}
}

void UViewTaskMenu::OnSelected(
	UTaskItem* ItemPtr
	)
{
	if (ItemPtr && ItemPtr != TaskItemPtr)
	{
		TaskItemPtr = ItemPtr;

		{
			auto UIPtr = GetWidgetFromName(FUViewTaskMenu::Get().TaskDetails);
			if (UIPtr)
			{
				UIPtr->SetVisibility(ESlateVisibility::Visible);
			}
		}

		ModifyTaskText();
		ModifyActiveGuideThreadText();
		ModifyTaskList();
		ModifyRewardProxysList();
	}
}

void UViewTaskMenu::OnActiveGuideThread()
{
	if (!TaskItemPtr)
	{
		return;
	}

	if (TaskItemPtr->MainGuideThreadClass)
	{
		UQuestSubSystem::GetInstance()->ActiveMainThread();
	}
	else if (TaskItemPtr->BrandGuideThreadClass)
	{
		UQuestSubSystem::GetInstance()->ActiveBrandGuideThread(TaskItemPtr->BrandGuideThreadClass);
	}

	UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<HumanProcessor::FHumanRegularProcessor>();
}

void UViewTaskMenu::ModifyTaskText()
{
	AQuestChainBase* GuideThreadPtr = nullptr;
	if (TaskItemPtr->MainGuideThreadClass)
	{
		GuideThreadPtr = TaskItemPtr->MainGuideThreadClass.GetDefaultObject();
	}
	else if (TaskItemPtr->BrandGuideThreadClass)
	{
		GuideThreadPtr = TaskItemPtr->BrandGuideThreadClass.GetDefaultObject();
	}

	if (!GuideThreadPtr)
	{
		return;
	}
	{
		auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FUViewTaskMenu::Get().TaskName));
		if (UIPtr)
		{
			UIPtr->SetText(FText::FromString(GuideThreadPtr->GetGuideThreadTitle()));
		}
	}

	{
		auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FUViewTaskMenu::Get().TaskDescription));
		if (UIPtr)
		{
			UIPtr->SetText(FText::FromString(GuideThreadPtr->Description));
		}
	}
}

void UViewTaskMenu::ModifyActiveGuideThreadText()
{
	const auto CurrentGuideThread = UQuestSubSystem::GetInstance()->GetCurrentGuideThread();
	if (!CurrentGuideThread)
	{
		return;
	}

	if (TaskItemPtr->MainGuideThreadClass)
	{
		if (TaskItemPtr->MainGuideThreadClass == CurrentGuideThread->GetClass())
		{
			auto TextUIPtr = Cast<UTextBlock>(
			                                  GetWidgetFromName(FUViewTaskMenu::Get().ActiveGuideThreadText)
			                                 );
			if (TextUIPtr)
			{
				TextUIPtr->SetText(FText::FromString(TEXT("正在追踪")));
			}

			return;
		}
	}
	else if (TaskItemPtr->BrandGuideThreadClass)
	{
		if (TaskItemPtr->BrandGuideThreadClass == CurrentGuideThread->GetClass())
		{
			auto TextUIPtr = Cast<UTextBlock>(
			                                  GetWidgetFromName(FUViewTaskMenu::Get().ActiveGuideThreadText)
			                                 );
			if (TextUIPtr)
			{
				TextUIPtr->SetText(FText::FromString(TEXT("正在追踪")));
			}

			return;
		}
	}

	auto TextUIPtr = Cast<UTextBlock>(
	                                  GetWidgetFromName(FUViewTaskMenu::Get().ActiveGuideThreadText)
	                                 );
	if (TextUIPtr)
	{
		TextUIPtr->SetText(FText::FromString(TEXT("追踪任务")));
	}
}

void UViewTaskMenu::ModifyTaskList()
{
	auto UIPtr = Cast<UScrollBox>(GetWidgetFromName(FUViewTaskMenu::Get().TaskList));
	if (UIPtr)
	{
		auto ChildrensAry = UIPtr->GetAllChildren();
		for (const auto& Iter : ChildrensAry)
		{
			if (Iter == TaskItemPtr)
			{
				continue;
			}
			auto TempUIPtr = Cast<UTaskItem>(Iter);
			if (TempUIPtr)
			{
				TempUIPtr->SwitchSelected(false);
			}
		}
	}
}

void UViewTaskMenu::ModifyRewardProxysList()
{
	auto UIPtr = Cast<UScrollBox>(GetWidgetFromName(FUViewTaskMenu::Get().RewardList));
	if (UIPtr)
	{
		UIPtr->ClearChildren();
		if (TaskItemPtr->MainGuideThreadClass)
		{
			auto GuideThreadPtr = TaskItemPtr->MainGuideThreadClass.GetDefaultObject();
			if (GuideThreadPtr)
			{
				for (const auto& Iter : GuideThreadPtr->RewardProxysMap)
				{
					auto RewardProxyPtr = CreateWidget<UProxyIcon>(this, RewardProxysClass);
					UIPtr->AddChild(RewardProxyPtr);
				}
			}
		}
		else if (TaskItemPtr->BrandGuideThreadClass)
		{
			auto GuideThreadPtr = TaskItemPtr->BrandGuideThreadClass.GetDefaultObject();
			if (GuideThreadPtr)
			{
				for (const auto& Iter : GuideThreadPtr->RewardProxysMap)
				{
					auto RewardProxyPtr = CreateWidget<UProxyIcon>(this, RewardProxysClass);
					RewardProxyPtr->ResetToolUIByData(Iter.Key);
					UIPtr->AddChild(RewardProxyPtr);
				}
			}
		}
	}
}
