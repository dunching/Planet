// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ItemProxyVisitorBase.h"

#include "ItemProxyVisitor.generated.h"

class UItemDecription;
class UItemInteractionList;

USTRUCT()
struct PLANET_API FItemProxyVisitor_Hover : public FItemProxyVisitor_Hover_Base
{
	GENERATED_USTRUCT_BODY()

public:
	virtual void Visit_PassiveSkillProxy(
		const TSharedPtr<FPassiveSkillProxy>& ProxySPtr
		) override;

private:
	virtual void Visit_Generic(
		const TSharedPtr<FBasicProxy>& ProxySPtr
		) override;

	virtual void Visit_Generic(
		const FGameplayTag& ProxyType
		) override;

	virtual void StopHoverInProxy(
		) override;

	UPROPERTY(transient)
	UItemDecription* ItemDecriptionPtr = nullptr;
};

USTRUCT()
struct PLANET_API FItemProxyVisitor_InAllocation : public FItemProxyVisitor_InAllocation_Base
{
	GENERATED_USTRUCT_BODY()

public:
	virtual void Visit_PassiveSkillProxy(
		const TSharedPtr<FPassiveSkillProxy>& ProxySPtr
		) override;

	virtual void Visit_Stop(
		) override;

private:

	void Upgrade();
	
	TSharedPtr<FPassiveSkillProxy> ProxySPtr = nullptr;
};
