// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "AbilitySystemInterface.h"
#include "GroupManagger.h"

#include "GameFramework/PlayerState.h"

#include "PlanetAbilitySystemComponent.h"
#include "ItemProxy_Minimal.h"

#include "GroupManagger_Player.generated.h"

/*
 * 挂载在PLayerController上
 */
UCLASS()
class PLANET_API AGroupManagger_Player :
	public AGroupManagger
{
	GENERATED_BODY()

public:
	
	using FOwnerType = APlanetPlayerController;

#pragma region RPC

	/**
	 * 生成一个PlayerCharacter的复制体
	 * @param Transform 
	 */
	UFUNCTION(Server, Reliable)
	void CloneCharacter_Server(
		const FGuid& ID,
		const FTransform& Transform,
		ETeammateOption TeammateOption
		);

	UFUNCTION(Server, Reliable)
	void SpawnGeneratorActor_Server(
		const TSoftObjectPtr<AGeneratorColony_ByInvoke>& GeneratorBasePtr
		);

	UFUNCTION(Server, Reliable)
	void SpawnCharacterAry_Server(
		const TArray<TSubclassOf<AHumanCharacter_AI>>& CharacterClassAry,
		const TArray<FGuid>& IDAry,
		const TArray<FTransform>& TransformAry,
		ETeammateOption TeammateOption
		);

	UFUNCTION(Server, Reliable)
	void SpawnCharacter_Server(
		TSubclassOf<AHumanCharacter_AI> CharacterClass,
		const FGuid& ID,
		const FTransform& Transform,
		ETeammateOption TeammateOption
		);

	UFUNCTION(Server, Reliable)
	void SpawnCharacterByProxyType_Server(
		const FGameplayTag& CharacterProxyType,
		const FTransform& Transform
		);

	UFUNCTION(Server, Reliable)
	void DestroyActor_Server(
		AActor* ActorPtr
		);
	
#pragma endregion
	

};