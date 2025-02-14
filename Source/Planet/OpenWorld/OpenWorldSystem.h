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
 * 挑战/爬塔关卡系统
 * 如传送副本之类
 * 我们会将一些挑战关卡放置在世界分区关卡，使用数据层的方式去加载或隐藏
 * 类似云顶的效果
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
