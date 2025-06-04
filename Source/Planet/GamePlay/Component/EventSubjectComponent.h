#pragma once

#include "CoreMinimal.h"

#include "PlanetAbilitySystemComponent.h"
#include "TemplateHelper.h"
#include "Components/ActorComponent.h"

#include "EventSubjectComponent.generated.h"

struct FOnEffectedTargetCallback;

/*
 * 事务由转发
 * 仅玩家具备
 * 如角色击败了某些目标
 */
UCLASS()
class UEventSubjectComponent : 
	public UActorComponent
{
	GENERATED_BODY()

public:
	
	using FMakedDamageDelegate = TCallbackHandleContainer<void(const FOnEffectedTargetCallback&)>;

	static FName ComponentName;

	UEventSubjectComponent(const FObjectInitializer& ObjectInitializer);

};
