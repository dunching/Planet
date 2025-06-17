#include "MaterialIcon.h"

#include "ProxyIcon.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UMaterialIcon::NativeConstruct()
{
	Super::NativeConstruct();

	AddBtn->OnClicked.AddDynamic(this, &ThisClass::OnClickedAddBtn);
	SubBtn->OnClicked.AddDynamic(this, &ThisClass::OnClickedSubBtn);
}

void UMaterialIcon::BindData(
	const TSharedPtr<FBasicProxy>& ProxySPtr
	)
{
	if (ProxyIcon)
	{
		ProxyIcon->ResetToolUIByData(ProxySPtr);
	}

	auto Proxy_UniqueSPtr = DynamicCastSharedPtr<IProxy_Unique>(ProxySPtr);
	if (Proxy_UniqueSPtr)
	{
		Text->SetText(FText::FromString(FString::Printf(TEXT("%d\\%d"), CurrentNum, Proxy_UniqueSPtr->GetNum())));
	}
	else
	{
		Text->SetText(FText::FromString(FString::Printf(TEXT("%d\\%d"), CurrentNum, Proxy_UniqueSPtr->GetNum())));
	}
}

void UMaterialIcon::OnClickedAddBtn()
{
}

void UMaterialIcon::OnClickedSubBtn()
{
}
