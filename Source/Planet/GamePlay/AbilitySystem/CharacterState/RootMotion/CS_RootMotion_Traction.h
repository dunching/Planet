
#pragma once

#include "CoreMinimal.h"

#include <GameplayTagContainer.h>
#include "GameFramework/RootMotionSource.h"

#include "Skill_Base.h"
#include "CS_RootMotion.h"

#include "CS_RootMotion_Traction.generated.h"

class USphereComponent;
class UAbilityTask_TimerHelper;
class UTexture2D;
struct FConsumableProxy;
class UEffectItem;
class ASPlineActor;
class UAbilityTask_MyApplyRootMotionRadialForce;
class UNetConnection;

struct FStreamableHandle;
struct FCharacterStateInfo;

UCLASS()
class PLANET_API ATractionPoint : public AActor
{
	GENERATED_BODY()

public:

	ATractionPoint(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

// 	virtual  UNetConnection* GetNetConnection() const override;

	virtual void Tick(float DeltaSeconds)override;
// 
//   	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
//   	TObjectPtr<USphereComponent> SphereComponentPtr = nullptr;
	
	UPROPERTY(Replicated)
	int32 Radius = 100;
	
	UPROPERTY(Replicated)
	float Strength = 0;
	
	UPROPERTY(Replicated)
	bool bIsPush = false;
	
	UPROPERTY(Replicated)
	bool bIsAdditive = true;
	
	UPROPERTY(Replicated)
	bool bNoZForce = true;
	
	UPROPERTY(Replicated)
	UCurveFloat* StrengthDistanceFalloff = nullptr;
	
	UPROPERTY(Replicated)
	UCurveFloat* StrengthOverTime = nullptr;
	
	UPROPERTY(Replicated)
	bool bUseFixedWorldDirection = false;
	
	UPROPERTY(Replicated)
	FRotator FixedWorldDirection = FRotator::ZeroRotator;
	
	UPROPERTY(Replicated)
	ERootMotionFinishVelocityMode VelocityOnFinishMode = 
		ERootMotionFinishVelocityMode::ClampVelocity;
	
	UPROPERTY(Replicated)
	FVector SetVelocityOnFinish = FVector::ZeroVector;
	
	UPROPERTY(Replicated)
	float ClampVelocityOnFinished = 200;
	
	// 
	UPROPERTY(Replicated)
	FGuid TractionPoint_Guid;
};

// 参考龙王E
USTRUCT()
struct PLANET_API FGameplayAbilityTargetData_RootMotion_Traction  :
	public FGameplayAbilityTargetData_RootMotion
{
	GENERATED_USTRUCT_BODY()

	FGameplayAbilityTargetData_RootMotion_Traction();

	virtual UScriptStruct* GetScriptStruct() const override;

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)override;

	virtual FGameplayAbilityTargetData_RootMotion_Traction* Clone()const override;

	TWeakObjectPtr<ATractionPoint>TractionPointPtr = nullptr;

	FGuid TractionPoint_Guid;

private:

};

template<>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_RootMotion_Traction> :
	public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_RootMotion_Traction>
{
	enum
	{
		WithNetSerializer = true,
	};
};

UCLASS()
class PLANET_API UCS_RootMotion_Traction : public UCS_RootMotion
{
	GENERATED_BODY()

public:

	using FRootMotionParam = FGameplayAbilityTargetData_RootMotion_Traction;

	virtual void OnAvatarSet(
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilitySpec& Spec
	) override;

	virtual void PreActivate(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
		const FGameplayEventData* TriggerEventData = nullptr
	);

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	)override;

protected:

	virtual void PerformAction()override;

	virtual void UpdateRootMotionImp(const TSharedPtr<FGameplayAbilityTargetData_RootMotion>& DataSPtr)override;

	void ExcuteTasks();

	void OnTaskComplete();
	
	TSharedPtr<FRootMotionParam>GameplayAbilityTargetDataSPtr = nullptr;

	UAbilityTask_MyApplyRootMotionRadialForce* RootMotionTaskPtr = nullptr;
	
	TSharedPtr<FCharacterStateInfo> CharacterStateInfoSPtr = nullptr;

};
