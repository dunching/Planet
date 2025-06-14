#include "TalentAllocation.h"

#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include <Blueprint/WidgetTree.h>
#include <Components/ScrollBar.h>
#include <Components/ScrollBox.h>

#include "CharacterBase.h"
#include "InventoryComponent.h"
#include "PlanetPlayerController.h"
#include "TalentAllocationComponent.h"
#include "TalentIcon.h"
#include "GroupmateIcon.h"

struct FTalentAllocation : public TStructVariable<FTalentAllocation>
{
	const FName UsedNum = TEXT("UsedNum");
};

void UTalentAllocation::NativeConstruct()
{
	Super::NativeConstruct();
}

void UTalentAllocation::NativeDestruct()
{
	Super::NativeDestruct();
}

void UTalentAllocation::EnableMenu()
{
	InitialGroupmateList();

	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!CharacterPtr)
	{
		return;
	}

	OnSelectedCharacterProxy(CharacterPtr->GetCharacterProxy());
}

void UTalentAllocation::DisEnableMenu()
{
	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (CharacterPtr)
	{
		auto TalentAllocationComponentPtr = CharacterPtr->GetTalentAllocationComponent();
		if (TalentAllocationComponentPtr)
		{
		}
	}
}

EMenuType UTalentAllocation::GetMenuType() const
{
	return EMenuType::kAllocationTalent;
}

void UTalentAllocation::UpdateAllTalenIconState()
{
	WidgetTree->ForEachWidget(
	                          [this](
	                          UWidget* Widget
	                          )
	                          {
		                          if (Widget && Widget->IsA<UTalentIcon>())
		                          {
			                          auto UIPtr = Cast<UTalentIcon>(Widget);
			                          if (!UIPtr)
			                          {
				                          return;
			                          }

			                          UIPtr->Reset();
		                          }
	                          }
	                         );

	for (auto Iter : EnableSocletIconSet)
	{
		if (Iter)
		{
			UpdateTalenIconState(Iter);
		}
	}
}

void UTalentAllocation::UpdateTalenIconState(
	UTalentIcon* TalentIconPtr
	)
{
	if (TalentIconPtr)
	{
		TalentIconPtr->Update();
		
		auto& CharacterTalentRef = CurrentProxyPtr->GetCharacterTalent();

		if (CharacterTalentRef.AllocationMap.Contains(TalentIconPtr->TalentSocket))
		{
			const auto Value = CharacterTalentRef.AllocationMap[TalentIconPtr->TalentSocket];
			if (Value >= TalentIconPtr->MaxNum)
			{
				for (auto Iter : TalentIconPtr->NextSocletIconSet)
				{
					UpdateTalenIconState(Iter);
				}
			}
		}
	}
}

void UTalentAllocation::OnUsedTalentNumChanged(
	int32 UsedNum,
	int32 TatolNum
	)
{
	auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FTalentAllocation::Get().UsedNum));
	if (UIPtr)
	{
		UIPtr->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), UsedNum, TatolNum)));
	}
}

bool UTalentAllocation::OnAddPoint(
	UTalentIcon* TalentIconPtr,
	bool bIsAdd
	)
{
	if (TalentIconPtr)
	{
		if (CurrentProxyPtr)
		{
			auto& CharacterTalentRef = CurrentProxyPtr->GetCharacterTalent();
			const auto TalentNum = CurrentProxyPtr->GetTalentNum();

			// 总使用
			int32 TotalNum = 0;

			// 当前插槽使用
			int32 CurrentNum = 0;
			for (const auto& Iter : CharacterTalentRef.AllocationMap)
			{
				TotalNum += Iter.Value;
			}

			if (bIsAdd)
			{
				TotalNum++;

				if (CharacterTalentRef.AllocationMap.Contains(TalentIconPtr->TalentSocket))
				{
					CurrentNum = CharacterTalentRef.AllocationMap[TalentIconPtr->TalentSocket] + 1;
				}
				else
				{
					CurrentNum = 1;
				}

				if (TotalNum <= TalentNum && CurrentNum <= TalentIconPtr->MaxNum)
				{
					// 本地先更新
					CurrentProxyPtr->UpdateTalentSocket(TalentIconPtr->TalentSocket, CurrentNum);

					// 更新服务器上的数据
					auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
					if (PCPtr)
					{
						PCPtr->UpdateCharacterTalent(CurrentProxyPtr->GetID(), TalentIconPtr->TalentSocket, CurrentNum);
					}

					OnUsedTalentNumChanged(TotalNum, TalentNum);

					UpdateAllTalenIconState();
					
					return true;
				}
			}
			else
			{
				for (auto Iter : TalentIconPtr->NextSocletIconSet)
				{
					if (Iter)
					{
						if (CharacterTalentRef.AllocationMap.Contains(Iter->TalentSocket))
						{
							// 如果这个Icon的下一级有被分配的点，则此Icon上分配的点无法被减少
							return false;
						}
					}
				}
				if (CharacterTalentRef.AllocationMap.Contains(TalentIconPtr->TalentSocket))
				{
					CurrentNum = CharacterTalentRef.AllocationMap[TalentIconPtr->TalentSocket] - 1;

					// 本地先更新
					CurrentProxyPtr->UpdateTalentSocket(TalentIconPtr->TalentSocket, CurrentNum);

					// 更新服务器上的数据
					auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
					if (PCPtr)
					{
						PCPtr->UpdateCharacterTalent(CurrentProxyPtr->GetID(), TalentIconPtr->TalentSocket, CurrentNum);
					}

					OnUsedTalentNumChanged(TotalNum, TalentNum);
					
					UpdateAllTalenIconState();
					
					return true;
				}
			}
		}
	}
	return false;
}

void UTalentAllocation::InitialGroupmateList()
{
	auto CharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	{
		if (!GroupmateList)
		{
			return;
		}
		GroupmateList->ClearChildren();

		auto HICPtr = CharacterPtr->GetInventoryComponent();
		auto GroupmateProxyAry = HICPtr->GetCharacterProxyAry();

		for (int32 Index = 0; Index < GroupmateProxyAry.Num(); Index++)
		{
			auto WidgetPtr = CreateWidget<UGroupmateIcon>(this, GroupmateIconClass);
			if (WidgetPtr)
			{
				WidgetPtr->ResetToolUIByData(GroupmateProxyAry[Index]);
				{
					auto Handle = WidgetPtr->OnSelected.AddCallback(
					                                                std::bind(
					                                                          &ThisClass::OnSelectedCharacterProxy,
					                                                          this,
					                                                          std::placeholders::_1
					                                                         )
					                                               );
					Handle->bIsAutoUnregister = false;
				}
				GroupmateList->AddChild(WidgetPtr);

				if (Index == 0)
				{
					WidgetPtr->SwitchSelectState(true);
				}
			}
		}
	}
}

void UTalentAllocation::OnSelectedCharacterProxy(
	const TSharedPtr<FCharacterProxy>& ProxyPtr
	)
{
	CurrentProxyPtr = ProxyPtr;

	if (CurrentProxyPtr)
	{
		const auto& CharacterTalentRef = CurrentProxyPtr->GetCharacterTalent();
		const auto TalentNum = CurrentProxyPtr->GetTalentNum();

		int32 Num = 0;
		for (const auto& Iter : CharacterTalentRef.AllocationMap)
		{
			Num += Iter.Value;
		}

		OnUsedTalentNumChanged(Num, TalentNum);
	}
	
	if (WidgetTree)
	{
		WidgetTree->ForEachWidget(
		                          [this](
		                          UWidget* Widget
		                          )
		                          {
			                          if (Widget && Widget->IsA<UTalentIcon>())
			                          {
				                          auto UIPtr = Cast<UTalentIcon>(Widget);
				                          if (!UIPtr)
				                          {
					                          return;
				                          }

				                          UIPtr->CurrentProxyPtr = CurrentProxyPtr;
				                          UIPtr->OnValueChanged.BindUObject(this, &ThisClass::OnAddPoint);
				                          UIPtr->Reset();
			                          }
		                          }
		                         );
	}

	UpdateAllTalenIconState();

	if (!GroupmateList)
	{
		return;
	}
	auto ChildrensAry = GroupmateList->GetAllChildren();
	for (auto Iter : ChildrensAry)
	{
		auto CharacterIconPtr = Cast<UGroupmateIcon>(Iter);
		if (CharacterIconPtr)
		{
			if (CharacterIconPtr->ProxyPtr == ProxyPtr)
			{
				DisEnableMenu();

				continue;
			}
			else
			{
				CharacterIconPtr->SwitchSelectState(false);
			}
		}
	}
}
