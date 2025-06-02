// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PlanetAbilitySystemComponent.h"

#include "GroupManaggerBase.generated.h"

class APlanetPlayerController;
class AHumanCharacter;
class ACharacterBase;
class IPlanetControllerInterface;
class AGroupManagger;
struct FSkillProxy;
struct FActiveSkillProxy;
struct FConsumableProxy;
struct FCharacterProxy;
class UPlanetAbilitySystemComponent;
class UGSAbilitySystemComponent;
class UProxySycHelperComponent;
class UTeamMatesHelperComponentBase;
class UInventoryComponentBase;
struct FSceneProxyContainer;

/*
 *	对局内每个组群共享的资源
 *	
 *	例如每个Player控制的Pawn和AI队友、群落AI
 *	一个组的持有的物品的信息
 *
 *	每个PlayerController会拥有一个，并于AI队友共享数据
 *	
 *	每组NPC会共用一个，由AGeneratorBase生成
 *
 *	单独的NPC会自己生成（SpawnDefaultController）一个，如功能性的NPC
 */
UCLASS()
class GROUPMANAGGER_API AGroupManaggerBase:
	public AInfo,
	public IPlanetAbilitySystemInterface
{
	GENERATED_BODY()

public:

	AGroupManaggerBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;
	
	virtual UPlanetAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UTeamMatesHelperComponentBase* GetTeamMatesHelperComponentBase();

	UInventoryComponentBase* GetInventoryComponentBase();

	FGuid GroupID;
	
protected:
	
	UPROPERTY()
	TObjectPtr<UTeamMatesHelperComponentBase> TeamMatesHelperComponentPtr = nullptr;
	
	UPROPERTY()
	TObjectPtr<UInventoryComponentBase> InventoryComponentPtr = nullptr;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TObjectPtr<UGSAbilitySystemComponent> AbilitySystemComponentPtr = nullptr;
	
};
