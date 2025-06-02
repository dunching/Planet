// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "BrainComponent.h"
#include "GameplayTagContainer.h"
#include "GuideInteractionBase.h"
#include "GuideThread.h"

#include "GuideInteraction.generated.h"

class UGameplayTasksComponent;

class UGuideSystemStateTreeComponent;
class UPAD_TaskNode_Guide;
class ACharacterBase;
class AChallengeEntry;
class AHumanCharacter;
class AHumanCharacter_Player;
class AHumanCharacter_AI;

using FOnGuideInteractionEnd = TMulticastDelegate<void()>;

#pragma region Base
UCLASS(BlueprintType, Blueprintable)
class PLANET_GUIDESYSTEM_API AGuideInteraction : public AGuideInteractionBase
{
	GENERATED_BODY()

public:
	AGuideInteraction(
		const FObjectInitializer& ObjectInitializer
		);
};

UCLASS()
class UStateTreeGuideInteractionComponentSchema : public UStateTreeGuideInteractionComponentSchemaBase
{
	GENERATED_BODY()

public:
	using FOwnerType = AGuideInteractionBase;

	UStateTreeGuideInteractionComponentSchema();

	virtual void PostLoad() override;

	virtual bool IsStructAllowed(
		const UScriptStruct* InScriptStruct
		) const override;

	static bool SetContextRequirements(
		UBrainComponent& BrainComponent,
		FStateTreeExecutionContext& Context,
		bool bLogErrors = false
		);

	UPROPERTY(EditAnywhere, Category = "Defaults", NoClear)
	TSubclassOf<AHumanCharacter_Player> HumanCharacter_PlayerClass = nullptr;
};

/**
 *
 */
UCLASS()
class PLANET_GUIDESYSTEM_API UGuideInteractionSystemStateTreeComponent : public UGuideInteractionSystemStateTreeComponentBase
{
	GENERATED_BODY()

public:
	using FSchemaType = UStateTreeGuideInteractionComponentSchema;

	virtual TSubclassOf<UStateTreeSchema> GetSchema() const override;

	virtual bool SetContextRequirements(
		FStateTreeExecutionContext& Context,
		bool bLogErrors = false
		) override;
};
#pragma endregion

#pragma region NPC交互
/**
 *	与NPC对话时的系列任务
 *
 *	例如：主线或者支线的某一段要求角色前往某处与NPC对话时，新增的对话选项
 */
UCLASS(BlueprintType, Blueprintable)
class PLANET_GUIDESYSTEM_API AGuideInteraction_HumanCharacter_AI : public AGuideInteractionBase
{
	GENERATED_BODY()

public:
	AGuideInteraction_HumanCharacter_AI(
		const FObjectInitializer& ObjectInitializer
		);

	// 这个引导所属的Character
	AHumanCharacter_AI* Character_NPC = nullptr;
};

UCLASS()
class UStateTreeGuideInteraction_HumanCharacter_AI_ComponentSchema : public UStateTreeGuideInteractionComponentSchema
{
	GENERATED_BODY()

public:
	using FOwnerType = AGuideInteraction_HumanCharacter_AI;

	UStateTreeGuideInteraction_HumanCharacter_AI_ComponentSchema();

	virtual void PostLoad() override;

	static bool SetContextRequirements(
		UBrainComponent& BrainComponent,
		FStateTreeExecutionContext& Context,
		bool bLogErrors = false
		);

	UPROPERTY(EditAnywhere, Category = "Defaults", NoClear)
	TSubclassOf<AHumanCharacter_AI> HumanCharacter_AIClass = nullptr;
};

/**
 *
 */
UCLASS()
class PLANET_GUIDESYSTEM_API UGuideInteraction_HumanCharacter_AI_SystemStateTreeComponent : public
	UGuideInteractionSystemStateTreeComponent
{
	GENERATED_BODY()

public:
	using FSchemaType = UStateTreeGuideInteraction_HumanCharacter_AI_ComponentSchema;

	virtual TSubclassOf<UStateTreeSchema> GetSchema() const override;

	virtual bool SetContextRequirements(
		FStateTreeExecutionContext& Context,
		bool bLogErrors = false
		) override;
};
#pragma endregion

#pragma region 挑战关卡入口
/**
 *	在挑战入口对话时的系列任务
 */
UCLASS(BlueprintType, Blueprintable)
class PLANET_GUIDESYSTEM_API AGuideInteraction_ChallengeEntry : public AGuideInteractionBase
{
	GENERATED_BODY()

public:
	AGuideInteraction_ChallengeEntry(
		const FObjectInitializer& ObjectInitializer
		);

	// 
	AChallengeEntry* ChallengeEntryPtr = nullptr;
};

UCLASS()
class UStateTreeGuideInteraction_ChallengeEntry_ComponentSchema : public UStateTreeGuideInteractionComponentSchema
{
	GENERATED_BODY()

public:
	using FOwnerType = AGuideInteraction_ChallengeEntry;

	UStateTreeGuideInteraction_ChallengeEntry_ComponentSchema();

	virtual void PostLoad() override;

	static bool SetContextRequirements(
		UBrainComponent& BrainComponent,
		FStateTreeExecutionContext& Context,
		bool bLogErrors = false
		);

	UPROPERTY(EditAnywhere, Category = "Defaults", NoClear)
	TSubclassOf<AChallengeEntry> ChallengeEntryClass = nullptr;
};

/**
 *
 */
UCLASS()
class PLANET_GUIDESYSTEM_API UGuideInteraction_ChallengeEntry_SystemStateTreeComponent : public
	UGuideInteractionSystemStateTreeComponent
{
	GENERATED_BODY()

public:
	using FSchemaType = UStateTreeGuideInteraction_ChallengeEntry_ComponentSchema;

	virtual TSubclassOf<UStateTreeSchema> GetSchema() const override;

	virtual bool SetContextRequirements(
		FStateTreeExecutionContext& Context,
		bool bLogErrors = false
		) override;
};
#pragma endregion
