// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"
#include "STT_CharacterBase.h"

#include "STT_GetOriginalLocation.generated.h"

class UGloabVariable_Character;

USTRUCT()
struct PLANET_API FSTID_GetOriginalLocation : public FSTID_CharacterBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	UGloabVariable_Character* GloabVariable = nullptr;
};

/*
 */
USTRUCT()
struct PLANET_API FSTT_GetOriginalLocation : public FSTT_CharacterBase //FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_GetOriginalLocation;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;

};
