// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Templates/SubclassOf.h"
#include "StateTreeEvaluatorBase.h"

#include "STE_CharacterBase.h"
#include "TeamMates_GenericType.h"
#include "TemplateHelper.h"

#include "STE_Assistance.generated.h"

class AActor;

struct FCharacterProxy;
class AGeneratorNPCs_Patrol;
class ACharacterBase;
class ABuildingArea;
class USceneComponent;
class AHumanCharacter;
class AHumanCharacter_AI;
class AHumanAIController;
class UGloabVariable_Character;

UCLASS(Blueprintable)
class PLANET_API USTE_Assistance : public USTE_CharacterBase
{
	GENERATED_BODY()

public:

	using FCharacterProxyType = FCharacterProxy;

	using FTeamOptionChangedHandle =
		TCallbackHandleContainer<void(ETeammateOption, const TSharedPtr<FCharacterProxyType>&)>::FCallbackHandleSPtr;

	using FTeammateChangedHandle =
		TCallbackHandleContainer<void()>::FCallbackHandleSPtr;

	using FKownCharacterChangedHandle =
		TCallbackHandleContainer<void(TWeakObjectPtr<ACharacterBase>, bool)>::FCallbackHandleSPtr;

protected:

	virtual void TreeStart(FStateTreeExecutionContext& Context)override;

	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime)override;

	virtual void TreeStop(FStateTreeExecutionContext& Context)override;

	void OnTeamOptionChanged(ETeammateOption TeammateOption);

	void OnTeamChanged();

private:
	virtual UGloabVariable_Character* CreateGloabVarianble() override;;

	TWeakObjectPtr<ACharacterBase> UpdateTargetCharacter();

	TWeakObjectPtr<ACharacterBase> GetTargetCharacter(
			const FVector& Location,
			const int32 &MaxDistance, 
			TSet<TWeakObjectPtr<ACharacterBase>>&KnowCharatersSet
			)const;
	
public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Param)
	int32 MaxDistanceToPatrolSpline = 1000;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Param)
	int32 MinAttackDistance = 50;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	ETeammateOption TeammateOption = ETeammateOption::kInitialize;

	/**
	 * 既定的巡逻路线
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	TObjectPtr<AGeneratorNPCs_Patrol> GeneratorNPCs_PatrolPtr = nullptr;

	/**
	 * 营寨
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	TObjectPtr<ABuildingArea> BuildingAreaPtr = nullptr;

	/**
	 * 跟随前进的点
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	TObjectPtr<USceneComponent> PathFollowComponentPtr = nullptr;
	
	/**
	 * 若为Player的群体，则为PlayerCharacter
	 * NPC群体为空
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	ACharacterBase* LeaderCharacterPtr = nullptr;

	FTeamOptionChangedHandle TeammateOptionChangedDelegate;

	FTeammateChangedHandle TeammateChangedDelegate;

};
