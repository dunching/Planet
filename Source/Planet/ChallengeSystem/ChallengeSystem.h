// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"

#include "ChallengeSystem.generated.h"

class ATargetPoint;
class ATeleport;
class APlanetPlayerController;

UENUM(BlueprintType)
enum class EChallengeLevelType : uint8
{
	kReturnOpenWorld,
	kTest1,
	kTest2,
};

USTRUCT(BlueprintType)
struct PLANET_API FTableRow_Challenge : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EChallengeLevelType ChallengeLevelType = EChallengeLevelType::kTest1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString ExtendText;

	UPROPERTY(EditAnywhere, Category = "DataLayer")
	TObjectPtr<const UDataLayerAsset> DLS;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UnitExtendInfoMap")
	TSoftObjectPtr<ATeleport> TeleportRef;
};

/*
 * 挑战/爬塔关卡系统
 * 如传送副本之类
 * 我们会将一些挑战关卡放置在世界分区关卡，使用数据层的方式去加载或隐藏
 * 类似云顶的效果
 */
UCLASS()
class PLANET_API UChallengeSubSystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	static UChallengeSubSystem* GetInstance();

private:

	FTransform OpenWorldTransform = FTransform::Identity;
	
};
