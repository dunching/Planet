#include "MaterialIcon.h"

#include "ProxyIcon.h"
#include "UpgradeBoder.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UMaterialIcon::NativeConstruct()
{
	Super::NativeConstruct();

	AddAllBtn->OnClicked.AddDynamic(this, &ThisClass::OnClickedAddAllBtn);
	AddBtn->OnClicked.AddDynamic(this, &ThisClass::OnClickedAddBtn);
	SubBtn->OnClicked.AddDynamic(this, &ThisClass::OnClickedSubBtn);
}

void UMaterialIcon::NativeDestruct()
{
	DelegateHandle.Reset();
	
	Super::NativeDestruct();
}

void UMaterialIcon::BindData(
	const TSharedPtr<FMaterialProxy>& InProxySPtr
	)
{
	ProxySPtr = InProxySPtr;
	if (ProxyIcon)
	{
		ProxyIcon->ResetToolUIByData(ProxySPtr);

		DelegateHandle = ProxySPtr->CallbackContainerHelper.AddOnValueChanged(
		                                                     std::bind(
		                                                               &ThisClass::OnValueChanged,
		                                                               this,
		                                                               std::placeholders::_1,
		                                                               std::placeholders::_2
		                                                              )
		                                                    );
	}
	Update();
}

void UMaterialIcon::OnValueChanged(
	int32 OldValue,
	int32 NewValue
	)
{
	CurrentNum = 0;
	Update();
}

void UMaterialIcon::OnClickedAddAllBtn()
{
	const auto Num = ProxySPtr->GetNum();

	CurrentNum = Num;

	if (UpgradeBoderPtr)
	{
		auto EffectNum = UpgradeBoderPtr->OnUpdateMaterial(ProxySPtr, CurrentNum, true);
		CurrentNum = EffectNum;

		Update();
	}
}

void UMaterialIcon::OnClickedAddBtn()
{
	const auto Num = ProxySPtr->GetNum();
	if (CurrentNum < Num)
	{
		CurrentNum++;

		auto EffectNum = UpgradeBoderPtr->OnUpdateMaterial(ProxySPtr, CurrentNum, true);
		CurrentNum = EffectNum;

		Update();
	}
}

void UMaterialIcon::OnClickedSubBtn()
{
	if (CurrentNum > 0)
	{
		CurrentNum--;

		UpgradeBoderPtr->OnUpdateMaterial(ProxySPtr, CurrentNum, false);

		Update();
	}
}

void UMaterialIcon::Update()
{
	if (ProxySPtr)
	{
		Text->SetText(FText::FromString(FString::Printf(TEXT("%d\\%d"), CurrentNum, ProxySPtr->GetNum())));
	}
}
