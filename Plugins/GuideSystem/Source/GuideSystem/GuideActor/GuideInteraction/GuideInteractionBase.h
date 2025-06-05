// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "BrainComponent.h"
#include "GameplayTagContainer.h"
#include "Components/StateTreeComponentSchema.h"

#include "GuideActorBase.h"
#include "GuideSystemStateTreeComponent.h"

#include "GuideInteractionBase.generated.h"


class UGameplayTasksComponent;

class UGuideSystemStateTreeComponent;
class UPAD_TaskNode_Guide;
class ACharacter;

using FOnGuideInteractionEnd = TMulticastDelegate<void()>;

#pragma region Base
UCLASS(BlueprintType, Blueprintable)
class GUIDESYSTEM_API AGuideInteractionBase : public AGuideActorBase
{
	GENERATED_BODY()

public:
	AGuideInteractionBase(const FObjectInitializer& ObjectInitializer);

	// 交互节点名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString InteractionNodeName;

	/**
	 * 提前终止交互
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bWantToStop = false;
	
};

UCLASS()
class GUIDESYSTEM_API UStateTreeGuideInteractionComponentSchemaBase : public UStateTreeComponentSchema
{
	GENERATED_BODY()
public:
	using FOwnerType = AGuideInteractionBase;

	UStateTreeGuideInteractionComponentSchemaBase();
	
	virtual void PostLoad() override;
	
	virtual bool IsStructAllowed(const UScriptStruct* InScriptStruct) const override;

	static bool SetContextRequirements(UBrainComponent& BrainComponent, FStateTreeExecutionContext& Context, bool bLogErrors = false);

	/**
	 * 交互的本体
	 * 如玩家跟门交互，此类型则为玩家
	 */
	UPROPERTY(EditAnywhere, Category = "Defaults", NoClear)
	TSubclassOf<ACharacter> ACharacterClass = nullptr;
	
	/**
	 * 交互的目标的类型
	 * 如玩家跟门交互，此类型则为门
	 */
	UPROPERTY(EditAnywhere, Category = "Defaults", NoClear)
	TSubclassOf<AActor> InteractionTargetClass = nullptr;
	
};

/**
 *
 */
UCLASS()
class GUIDESYSTEM_API UGuideInteractionSystemStateTreeComponentBase : public UGuideSystemStateTreeComponent
{
	GENERATED_BODY()

public:
	using FSchemaType = UStateTreeGuideInteractionComponentSchemaBase;
	
	virtual TSubclassOf<UStateTreeSchema>GetSchema() const override;
	
	virtual bool SetContextRequirements(FStateTreeExecutionContext& Context, bool bLogErrors = false) override;
};
#pragma endregion
