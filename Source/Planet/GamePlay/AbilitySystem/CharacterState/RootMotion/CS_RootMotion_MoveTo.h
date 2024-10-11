
#pragma once

#include "CoreMinimal.h"

#include <GameplayTagContainer.h>

#include "Skill_Base.h"
#include "CS_RootMotion.h"

#include "CS_RootMotion_MoveTo.generated.h"

class UAbilityTask_TimerHelper;
class UTexture2D;
struct FConsumableProxy;
class UEffectItem;
class ASPlineActor;
class ATornado;

struct FStreamableHandle;

USTRUCT()
struct PLANET_API FGameplayAbilityTargetData_RootMotion_MoveTo :
	public FGameplayAbilityTargetData_RootMotion
{
	GENERATED_USTRUCT_BODY()

	FGameplayAbilityTargetData_RootMotion_MoveTo();

	int32 MoveSpeed = 100;

	// true:将目标按速度移动至TargetPt（参考机器人Q）
	// false:将目标按速度移动至与TargetPt垂直的点（参考铁男E）
	bool bIsMoveToTargetPt = false;

	FVector MoveDirection = FVector::ZeroVector;
	
	FVector TargetPt = FVector::ZeroVector;

private:

};
