// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"
#include "Tasks/StateTreeAITask.h"
#include <EnvironmentQuery/EnvQueryTypes.h>

#include "GenerateType.h"

#include "STT_RotateToFaceEntry.generated.h"

class UEnvQuery;

class ACharacterBase;
class AHumanCharacter;
class AHumanCharacter_AI;
class AHumanAIController;
class UAITask_ReleaseSkill;
class USTE_Assistance;
class UGloabVariable_Character;

#pragma region 面向目标
USTRUCT()
struct PLANET_API FSTID_RotateToFaceTarget
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AHumanCharacter> CharacterPtr = nullptr;

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AHumanAIController> AIControllerPtr = nullptr;
	
	UPROPERTY(EditAnywhere, Category = Context)
	UGloabVariable_Character* GloabVariable = nullptr;

};

USTRUCT()
struct PLANET_API FSTT_RotateToFaceEntry : public FStateTreeAIActionTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_RotateToFaceTarget;

	using FAITaskType = UAITask_ReleaseSkill;

	virtual const UStruct* GetInstanceDataType() const override;

	virtual EStateTreeRunStatus Tick(
		FStateTreeExecutionContext& Context,
		const float DeltaTime
	) const override;

};
#pragma endregion

#pragma region 是否面向目标
USTRUCT()
struct PLANET_API FSTID_IsFaceToTarget
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AHumanCharacter> CharacterPtr = nullptr;

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AHumanAIController> AIControllerPtr = nullptr;
	
	UPROPERTY(EditAnywhere, Category = Context)
	UGloabVariable_Character* GloabVariable = nullptr;

	UPROPERTY(
		EditAnywhere,
		Category = Param
	)
	bool bRunForever = true;
	
};

USTRUCT()
struct PLANET_API FSTT_IsFaceToTarget : public FStateTreeAIActionTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_IsFaceToTarget;

	using FAITaskType = UAITask_ReleaseSkill;

	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}

	virtual EStateTreeRunStatus Tick(
		FStateTreeExecutionContext& Context,
		const float DeltaTime
	) const override;

};
#pragma endregion
