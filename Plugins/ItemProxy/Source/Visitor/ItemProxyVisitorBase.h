// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ItemProxyVisitorBase.generated.h"

struct FGameplayTag;

struct FBasicProxy;
struct FMaterialProxy;
struct FPassiveSkillProxy;

USTRUCT()
struct ITEMPROXY_API FItemProxyVisitorBase
{
	GENERATED_USTRUCT_BODY()

public:
	virtual ~FItemProxyVisitorBase();
};

/**
 * 以悬浮的方式访问
 */
USTRUCT()
struct ITEMPROXY_API FItemProxyVisitor_Hover_Base : public FItemProxyVisitorBase
{
	GENERATED_USTRUCT_BODY()

public:
	virtual void Visit_PassiveSkillProxy(
		const TSharedPtr<FPassiveSkillProxy>& ProxySPtr
		);

	virtual void Visit_Generic(
		const TSharedPtr<FBasicProxy>& ProxySPtr
		);

	virtual void Visit_Generic(
		const FGameplayTag& ProxyType
		);

	virtual void StopHoverInProxy(
		);

private:
};

/**
 * 以在背包界面，点击的方式访问
 */
USTRUCT()
struct ITEMPROXY_API FItemProxyVisitor_InAllocation_Base : public FItemProxyVisitorBase
{
	GENERATED_USTRUCT_BODY()

public:
	virtual void Visit_PassiveSkillProxy(
		const TSharedPtr<FPassiveSkillProxy>& ProxySPtr
		);

	virtual void Visit_Stop(
		);
};
