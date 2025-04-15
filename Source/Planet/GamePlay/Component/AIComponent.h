// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"
#include "GenerateType.h"
#include "GroupManaggerInterface.h"
#include "Components/ActorComponent.h"

#include "AIComponent.generated.h"

class USceneComponent;
class ACharacterBase;
class UPAD_TaskNode;
class UPAD_TaskNode_Preset;
class UDA_NPCAllocation;
class UTaskNode_Temporary;
class AHumanCharacter_AI;

/**
 *
 */
UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class PLANET_API UAIComponent :
	public UActorComponent,
	public IGroupManaggerInterface
{
	GENERATED_BODY()

public:
	using FOwnerType = AHumanCharacter_AI;

	static FName ComponentName;

	UAIComponent(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void OnGroupManaggerReady(AGroupManagger* NewGroupSharedInfoPtr)override;

	void AddTemporaryTaskNode(UTaskNode_Temporary*TaskNodePtr);

	void InitialAllocationsRowName();

	void InitialAllocationsByProxy();

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pawn")
	ETeammateOption DefaultTeammateOption = ETeammateOption::kEnemy;
#endif

protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> PathFollowComponentPtr = nullptr;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Pawn")
	FGameplayTag AI_CharacterType = FGameplayTag::EmptyTag;

#pragma region Allocation
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Allocation")
	FGameplayTag FirstWeaponSocketInfo;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Allocation")
	FGameplayTag SecondWeaponSocketInfo;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Allocation")
	FGameplayTag ActiveSkillSet_1;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Allocation")
	FGameplayTag ActiveSkillSet_2;
#pragma endregion
};
