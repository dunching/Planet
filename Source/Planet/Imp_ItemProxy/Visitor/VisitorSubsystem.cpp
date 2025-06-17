#include "VisitorSubsystem.h"

#include "Subsystems/SubsystemBlueprintLibrary.h"

#include "InputProcessorSubSystem_Imp.h"
#include "ItemProxyVisitor.h"
#include "ItemProxy_Skills.h"

UVisitorSubsystem* UVisitorSubsystem::GetInstance()
{
	return Cast<UVisitorSubsystem>(
	                               USubsystemBlueprintLibrary::GetGameInstanceSubsystem(
		                                GetWorldImp(),
		                                UVisitorSubsystem::StaticClass()
		                               )
	                              );
}

void UVisitorSubsystem::HoverInProxy(
	const TSharedPtr<FBasicProxy>& ProxySPtr
	)
{
	if (!VisitorWhenHoverSPtr)
	{
		VisitorWhenHoverSPtr = MakeShared<FItemProxyVisitor_Hover>();
	}

	VisitorWhenHoverSPtr->Visit_Generic(ProxySPtr);
}

void UVisitorSubsystem::HoverInProxy(
	const FGameplayTag& ProxyType
	)
{
	if (!VisitorWhenHoverSPtr)
	{
		VisitorWhenHoverSPtr = MakeShared<FItemProxyVisitor_Hover>();
	}

	VisitorWhenHoverSPtr->Visit_Generic(ProxyType);
}

void UVisitorSubsystem::StopHoverInProxy()
{
	if (!VisitorWhenHoverSPtr)
	{
		VisitorWhenHoverSPtr = MakeShared<FItemProxyVisitor_Hover>();
	}

	VisitorWhenHoverSPtr->StopHoverInProxy();
}

void UVisitorSubsystem::ClickedPassiveSkillProxyInAllocation(
	const TSharedPtr<FPassiveSkillProxy> ProxySPtr
	)
{
	if (!VisitorInAllocationSPtr)
	{
		VisitorInAllocationSPtr = MakeShared<FItemProxyVisitor_InAllocation>();
	}

	VisitorInAllocationSPtr->Visit_PassiveSkillProxy(ProxySPtr);
}
