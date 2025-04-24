#pragma once

#include "CoreMinimal.h"

#include "PlanetAbilitySystemComponent.h"
#include "TemplateHelper.h"
#include "Components/ActorComponent.h"

#include "EventSubjectComponent.generated.h"

struct FOnEffectedTawrgetCallback;

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
	
	using FMakedDamageDelegate = TCallbackHandleContainer<void(const FOnEffectedTawrgetCallback&)>;

	static FName ComponentName;

	UEventSubjectComponent(const FObjectInitializer& ObjectInitializer);

	// 对“其他”角色造成的影响（伤害、控制）
	UFUNCTION(NetMulticast, Reliable)
	void OnEffectOhterCharacter(
		const FOnEffectedTawrgetCallback& ReceivedEventModifyDataCallback
		);
	
	// 对“其他”角色造成的影响（伤害、控制）
	FMakedDamageDelegate MakedDamageDelegate;

};
