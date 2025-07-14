// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"

#include "GenerateTypes.h"
#include "GroupManaggerInterface.h"
#include "TeamMates_GenericType.h"

#include "AIComponent.generated.h"

class USceneComponent;
class ACharacterBase;
class UPAD_TaskNode;
class UPAD_TaskNode_Preset;
class UDA_NPCAllocation;
class UTaskNode_Temporary;
class AHumanCharacter_AI;
class AGeneratorNPCs_Patrol;
class ABuildingArea;
class UTaskPromt;

/**
 *
 */
UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class PLANET_API UAIComponent :
	public UActorComponent,
	public IGroupManaggerInterface
{
	GENERATED_BODY()

public:
	using FOwnerType = AHumanCharacter_AI;

	static FName ComponentName;

	UAIComponent(
		const FObjectInitializer& ObjectInitializer
		);

	virtual void BeginPlay() override;

	virtual void OnSelfGroupManaggerReady(
		AGroupManagger* NewGroupSharedInfoPtr
		) override;

	virtual void OnPlayerGroupManaggerReady(
		AGroupManagger* NewGroupSharedInfoPtr
		) override;

	void AddTemporaryTaskNode(
		UTaskNode_Temporary* TaskNodePtr
		);

	void InitialAllocationsRowName();

	void InitialAllocationsByProxy();

	void DisplayTaskPromy(
		TSubclassOf<UTaskPromt> TaskPromtClass
		);

	void StopDisplayTaskPromy();

	TMap<FGameplayTag, FProductsForSale> GetSaleItemsInfo() const;

	void SetIsCheerOn(
		bool IsCheerOn
		);

	bool GetCheerOn() const;

	void SetCustomCustomTeammateOption(ETeammateOption InCustomTeammateOption );
	
	ETeammateOption GetCustomCustomTeammateOption()const;
	
#pragma region RPC
#pragma endregion

	/**
	 * 跟随前进的点
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> PathFollowComponentPtr = nullptr;

	/**
	 * 是否是单个的NPC，如地图上的商家
	 */
	bool bIsSingle = true;

	/**
	 * 是否是玩家的队友
	 */
	bool bIsTeammate = false;

protected:
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

	/**
	 * 这个AI的行为模式
	 * 若为kNone则使用UTeamMatesHelperComponent上的类型
	 */
	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pawn")
	ETeammateOption CustomTeammateOption = ETeammateOption::kNone;

	/**
	 * 是否处于掠阵状态
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsCheerOn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FGameplayTag, FProductsForSale> ProxyMap;

	UPROPERTY(Transient)
	UTaskPromt* TaskPromtPtr = nullptr;
};
