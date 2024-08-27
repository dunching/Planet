
#pragma once

#include "CoreMinimal.h"
#include "AbilityTask_TimerHelper.h"
#include "ProjectileBase.h"

#include "Skill_Active_Base.h"
#include "Components/CapsuleComponent.h"

#include "Skill_Active_IceGun.generated.h"

class UAnimMontage;
class ACharacterBase;

struct FGameplayAbilityTargetData_IceGun : public FGameplayAbilityTargetData
{
	ACharacterBase* TargetCharacterPtr = nullptr;
};

UCLASS()
class PLANET_API USkill_Active_IceGun : public USkill_Active_Base
{
	GENERATED_BODY()

public:

	virtual void PreActivate(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
		const FGameplayEventData* TriggerEventData = nullptr
	) override;

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr,
		const FGameplayTagContainer* TargetTags = nullptr,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr
	) const override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	) override;

protected:

	virtual void PerformAction(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	);

	void FindTarget();

	void PlayMontage();
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	UAnimMontage* HumanMontage = nullptr;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	float Duration = 1.5f;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	float Distance = 800.f;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	float ToCharacterOffset = 100.f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	float Offset = 20.f;

	FVector StartPt = FVector::ZeroVector;

	FVector EndPt = FVector::ZeroVector;

	ASkill_IceGun_Projectile* IceGunPtr = nullptr;

	TArray<UMaterialInstance> CharacterMat{};

	UFUNCTION()
	void OnNotifyBeginReceived(FName NotifyName);
	
	void OnProjectileBounce(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                        UPrimitiveComponent* OtherComp,
	                        int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void OnTimerHelperTick(UAbilityTask_TimerHelper* TaskPtr, float CurrentInterval, float Interval);
	void ExcuteTasks();
	void OnOverlap(AActor* OtherActor);
	
	UFUNCTION()
	bool ResetIceGun(UAbilityTask_TimerHelper* TaskPtr);
	// 创建一个TimerHelper_Finished_Delegate类型的实例
	TimerHelper_Finished_Delegate finishedDelegate;
};


UCLASS()
class PLANET_API ASkill_IceGun_Projectile : public AProjectileBase
{
	GENERATED_BODY()

	ASkill_IceGun_Projectile(const FObjectInitializer& ObjectInitializer);
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UCapsuleComponent> CapsuleComponentPtr = nullptr;

	void Reset()
	{
		// 禁用碰撞
		CapsuleComponentPtr->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CapsuleComponentPtr->SetCollisionResponseToAllChannels(ECR_Ignore);
		// 隐藏 Actor
		SetActorHiddenInGame(true);
		SetActorTickEnabled(false); // 禁用Actor的Tick
	}

	void Activate()
	{
		CapsuleComponentPtr->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		CapsuleComponentPtr->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 仅示例，根据需要调整
		SetActorHiddenInGame(false);
		SetActorTickEnabled(true);
	}
};
