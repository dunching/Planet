// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "UObject/ObjectMacros.h"
#include "UObject/Interface.h"

#include "GetModifyItemProxyStrategiesInterface.generated.h"

struct FModifyItemProxyStrategyIterface;

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UGetModifyItemProxyStrategies : public UInterface
{
	GENERATED_BODY()
};

class ITEMPROXY_API IGetModifyItemProxyStrategies
{
	GENERATED_BODY()

public:
	virtual void InitialModifyItemProxyStrategies() = 0;

	const TMap<FGameplayTag, TSharedPtr<FModifyItemProxyStrategyIterface>>& GetModifyItemProxyStrategies() const;

protected:

	// 通用的
	TMap<FGameplayTag, TSharedPtr<FModifyItemProxyStrategyIterface>> ModifyItemProxyStrategiesMap;
};
