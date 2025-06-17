#include "ItemProxyVisitor.h"

#include "InputProcessorSubSystem_Imp.h"
#include "Blueprint/UserWidget.h"

#include "ItemProxy_Skills.h"
#include "UICommon.h"
#include "ItemDecriptionBaseWidget.h"
#include "ItemDecription.h"

void FItemProxyVisitor_Hover::Visit_PassiveSkillProxy(
	const TSharedPtr<FPassiveSkillProxy>& ProxySPtr
	)
{
	Visit_Generic(ProxySPtr);
}

void FItemProxyVisitor_Hover::Visit_Generic(
	const TSharedPtr<FBasicProxy>& ProxySPtr
	)
{
	auto ProxyDTSPtr = GetTableRowProxy(ProxySPtr->GetProxyType());
	if (ProxyDTSPtr)
	{
		if (ItemDecriptionPtr)
		{
		}
		else
		{
			if (!ProxyDTSPtr->ItemProxy_Description || !ProxyDTSPtr->ItemDecriptionClass)
			{
				return;
			}

			ItemDecriptionPtr = CreateWidget<UItemDecription>(
			                                                  GEngine->GetFirstLocalPlayerController(GetWorldImp()),
			                                                  ProxyDTSPtr->ItemDecriptionClass
			                                                 );
		}
		if (ItemDecriptionPtr)
		{
			ItemDecriptionPtr->BindData(ProxySPtr, ProxyDTSPtr->ItemProxy_Description);

			ItemDecriptionPtr->AddToViewport(EUIOrder::kHoverDecription);
		}
	}
}

void FItemProxyVisitor_Hover::Visit_Generic(
	const FGameplayTag& ProxyType
	)
{
	auto ProxyDTSPtr = GetTableRowProxy(ProxyType);
	if (ProxyDTSPtr)
	{
		if (ItemDecriptionPtr)
		{
		}
		else
		{
			if (!ProxyDTSPtr->ItemProxy_Description || !ProxyDTSPtr->ItemDecriptionClass)
			{
				return;
			}

			ItemDecriptionPtr = CreateWidget<UItemDecription>(
			                                                  GEngine->GetFirstLocalPlayerController(GetWorldImp()),
			                                                  ProxyDTSPtr->ItemDecriptionClass
			                                                 );
		}
		if (ItemDecriptionPtr)
		{
			ItemDecriptionPtr->BindData(ProxyType, ProxyDTSPtr->ItemProxy_Description);

			ItemDecriptionPtr->AddToViewport(EUIOrder::kHoverDecription);
		}
	}
}

void FItemProxyVisitor_Hover::StopHoverInProxy()
{
	if (ItemDecriptionPtr)
	{
		ItemDecriptionPtr->RemoveFromParent();
	}
	ItemDecriptionPtr = nullptr;
}

void FItemProxyVisitor_InAllocation::Visit_PassiveSkillProxy(
	const TSharedPtr<FPassiveSkillProxy>& InProxySPtr
	)
{
	ProxySPtr = InProxySPtr;
	if (ProxySPtr)
	{
	}
}

void FItemProxyVisitor_InAllocation::Visit_Stop()
{
}

void FItemProxyVisitor_InAllocation::Upgrade()
{
}
