
#pragma once

#include "CoreMinimal.h"

#include <GameplayTagContainer.h>

#include "Skill_Base.h"
#include "CS_Base.h"

#include "CS_RootMotion.generated.h"

class UAbilityTask_TimerHelper;
class UTexture2D;
struct FConsumableProxy;
class UEffectItem;
class ASPlineActor;
class ATornado;

struct FStreamableHandle;

struct FGameplayAbilityTargetData_RootMotion;

USTRUCT()
struct PLANET_API FGameplayAbilityTargetData_RootMotion : 
	public FGameplayAbilityTargetData_CS_Base
{
	GENERATED_USTRUCT_BODY()

	friend UCS_RootMotion;

	FGameplayAbilityTargetData_RootMotion();

	FGameplayAbilityTargetData_RootMotion(
		const FGameplayTag& Tag
	);

	virtual UScriptStruct* GetScriptStruct() const override;

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)override;

	virtual FGameplayAbilityTargetData_RootMotion* Clone()const override;

	TSharedPtr<FGameplayAbilityTargetData_RootMotion> Clone_SmartPtr()const ;

	TWeakObjectPtr<ACharacterBase> TriggerCharacterPtr = nullptr;

	TWeakObjectPtr<ACharacterBase> TargetCharacterPtr = nullptr;

private:

};

template<>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_RootMotion> :
	public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_RootMotion>
{
	enum
	{
		WithNetSerializer = true,
	};
};

UCLASS()
class PLANET_API UCS_RootMotion : public UCS_Base
{
	GENERATED_BODY()

public:

	UCS_RootMotion();

	virtual void PreActivate(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
		const FGameplayEventData* TriggerEventData = nullptr
	);
	
	void UpdateRootMotion(
		const FGameplayEventData& GameplayEventData
	);

protected:

	// virtual void InitalDefaultTags()override;
	
	UFUNCTION(Client, Reliable)
	void UpdateRootMotion_Client(
		const FGameplayEventData& GameplayEventData
	);

	virtual void UpdateRootMotionImp(const TSharedPtr<FGameplayAbilityTargetData_RootMotion>&DataSPtr);

	virtual void PerformAction();

};
