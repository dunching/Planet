// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "AbilitySystemInterface.h"

#include "GameFramework/PlayerState.h"

#include "PlanetAbilitySystemComponent.h"
#include "ItemProxy_Minimal.h"

#include "GroupManagger.generated.h"

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
class UTeamMatesHelperComponent;
class UInventoryComponent;
struct FSceneProxyContainer;

/*
 *	对局内每个组群共享的资源
 *	例如每个Player控制的Pawn和AI队友、群落AI
 *	一个组的持有的物品的信息
 */
UCLASS()
class PLANET_API AGroupManagger:
	public AInfo,
	public IPlanetAbilitySystemInterface
{
	GENERATED_BODY()

public:

	AGroupManagger(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;
	
	virtual UPlanetAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UTeamMatesHelperComponent* GetTeamMatesHelperComponent();

	UInventoryComponent* GetHoldingItemsComponent();

	void InitialByPlayerController(APlanetPlayerController*PCPtr);
	
	FGuid GroupID;
	
protected:
	
	UPROPERTY()
	TObjectPtr<UTeamMatesHelperComponent> TeamMatesHelperComponentPtr = nullptr;
	
	UPROPERTY()
	TObjectPtr<UInventoryComponent> InventoryComponentPtr = nullptr;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TObjectPtr<UGSAbilitySystemComponent> AbilitySystemComponentPtr = nullptr;
	
};
