// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SceneObj/Projectile/PistolProjectile.h"
#include "EquipmentBase.h"
#include "EquipmentGun.generated.h"

class USkeletalMeshComponent;
class UPrimitiveComponent;
class UNiagaraComponent;
class UNiagaraSystem;
class ANiagaraActor;
class AHumanCharacter;

class AProjectileBase;

UCLASS(BlueprintType, Blueprintable)
class SHIYU_API UEquipmentGunInteractionComponent  : public UEquipmentInteractionComponent
{
	GENERATED_BODY()

public:

    UEquipmentGunInteractionComponent(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName GunMuzzSockName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName LoopDurationName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName TargetPosName;

	int32 GetAttackSpeed() const;

	int32 GetCurHP()const;

	bool Attack(int32 Val);

	UFUNCTION(BlueprintNativeEvent)
		void Attacked();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "property")
		int32 HP = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "property")
		int32 AttackSpeed = 1;

private:

};

UCLASS()
class SHIYU_API AEquipmentGun : public AEquipmentBase
{
    GENERATED_BODY()

public:

    AEquipmentGun(const FObjectInitializer& ObjectInitializer);

    virtual void EquipItemToCharacter(AHumanCharacter* CharacterPtr)override;

	virtual void StartAttack();

	virtual void EndAttack();

protected:

    UFUNCTION()
        void OnHandOverlayBegin(
            UPrimitiveComponent* HitComponent, 
            AActor* OtherActor,
            UPrimitiveComponent* OtherComp, 
            FVector NormalImpulse, 
            const FHitResult& Hit
        );

    virtual void BeginPlay() override;

    virtual void OnConstruction(const FTransform& Transform) override;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Simulation Fist")
        USceneComponent* SceneCompPtr = nullptr;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        USkeletalMeshComponent* SkeletalMeshCompPtr = nullptr;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TSubclassOf<AProjectileBase>ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Niagara", meta = (DisplayName = "Niagara System Asset"))
		UNiagaraSystem* Asset = nullptr;

	UPROPERTY(EditAnywhere, Category = "Niagara", meta = (DisplayName = "Niagara System Asset"))
		int32 DelayDestroyTime = 1;

private:

    void ProcessPistol(ECMDType CMDType, EItemSocketType ItemSocketType);

    void Fire();

    FTimerHandle FireTImer;

    ANiagaraActor* BulletNiagaraActorPtr = nullptr;

    bool IsFiring = false;

};
