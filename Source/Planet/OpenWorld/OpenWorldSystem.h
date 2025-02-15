// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"

#include "OpenWorldSystem.generated.h"

class ATargetPoint;
class ATeleport;
class APlanetPlayerController;

UENUM(BlueprintType)
enum class ETeleport : uint8
{
	kReturnOpenWorld,
	kTeleport_1,
	kTest1,
	kTest2,
};

USTRUCT(BlueprintType)
struct PLANET_API FTableRow_Teleport : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ETeleport ChallengeLevelType = ETeleport::kTest1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString ExtendText;

	UPROPERTY(EditAnywhere, Category = "DataLayer")
	TObjectPtr<const UDataLayerAsset> DLS;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSoftObjectPtr<ATeleport> TeleportRef;
};

/*
 * 传送
 */
UCLASS()
class PLANET_API UOpenWorldSubSystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	static UOpenWorldSubSystem* GetInstance();

	void EntryLevel(ETeleport ChallengeLevelType, APlanetPlayerController* PCPtr);

private:

	void ForeachTeleportRow(
		const FName& Key,
		const FTableRow_Teleport& Value,
		ETeleport ChallengeLevelType,
		APlanetPlayerController* PCPtr
		);
	
	FTransform OpenWorldTransform = FTransform::Identity;
	
};
