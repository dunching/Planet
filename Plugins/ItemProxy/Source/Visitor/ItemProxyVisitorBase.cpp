#include "ItemProxyVisitorBase.h"

FItemProxyVisitorBase::~FItemProxyVisitorBase()
{
}

void FItemProxyVisitor_Hover_Base::Visit_PassiveSkillProxy(
	const TSharedPtr<FPassiveSkillProxy>& ProxySPtr
	)
{
}

void FItemProxyVisitor_Hover_Base::Visit_Generic(
	const TSharedPtr<FBasicProxy>& ProxySPtr
	)
{
}

void FItemProxyVisitor_Hover_Base::Visit_Generic(
	const FGameplayTag& ProxyType
	)
{
}

void FItemProxyVisitor_Hover_Base::StopHoverInProxy()
{
}

void FItemProxyVisitor_InAllocation_Base::Visit_PassiveSkillProxy(
	const TSharedPtr<FPassiveSkillProxy>& ProxySPtr
	) 
{
}

void FItemProxyVisitor_InAllocation_Base::Visit_Stop() 
{
}
