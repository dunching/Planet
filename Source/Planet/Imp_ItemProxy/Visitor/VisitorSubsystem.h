// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ItemProxyVisitorBase.h"

#include "VisitorSubsystem.generated.h"

struct FItemProxyVisitor_Hover_Base;
struct FItemProxyVisitor_InAllocation_Base;

UCLASS(BlueprintType, Blueprintable)
class PLANET_API UVisitorSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static UVisitorSubsystem* GetInstance();

#pragma region 基础的，悬浮在图标上
	void HoverInProxy(
		const TSharedPtr<FBasicProxy>& ProxySPtr
		);

	void HoverInProxy(
		const FGameplayTag& ProxyType
		);

	void StopHoverInProxy();
#pragma endregion

#pragma region 在背包下点击
	void ClickedPassiveSkillProxyInAllocation(
		const TSharedPtr<FPassiveSkillProxy> ProxySPtr
		);
#pragma endregion

	TSharedPtr<FItemProxyVisitor_Hover_Base> VisitorWhenHoverSPtr;

	TSharedPtr<FItemProxyVisitor_InAllocation_Base> VisitorInAllocationSPtr;
};
