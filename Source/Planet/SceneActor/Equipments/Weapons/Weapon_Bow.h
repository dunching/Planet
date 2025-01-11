// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Weapon_Base.h"
#include "PlanetGameplayAbility.h"
#include "Skill_Base.h"
#include "ProjectileBase.h"

#include "Weapon_Bow.generated.h"

class AStaticMeshActor;
class ASkeletalMeshActor;
class UStaticMeshComponent;
class UPrimitiveComponent;
class UNiagaraComponent;
class AHumanCharacter;
class ATool_PickAxe;

UCLASS()
class PLANET_API ASkill_WeaponActive_Bow_Projectile : public AProjectileBase
{
	GENERATED_BODY()

public:

	ASkill_WeaponActive_Bow_Projectile(const FObjectInitializer& ObjectInitializer);
	
protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Niagara")
	UNiagaraComponent* NiagaraComponent = nullptr;

};

UCLASS()
class PLANET_API AWeapon_Bow : public AWeapon_Base
{
	GENERATED_BODY()

public:

	AWeapon_Bow(const FObjectInitializer& ObjectInitializer);

	virtual void AttachToCharacter(ACharacterBase* CharacterPtr)override;

	USkeletalMeshComponent* GetMesh()const;
	
	FTransform GetEmitTransform()const;

protected:

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason)override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FTransform BowTransform = FTransform::Identity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName Bow_Socket;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<ASkeletalMeshActor> Bow_Class= nullptr;
	
	UPROPERTY(Replicated)
	ASkeletalMeshActor* BowActorPtr = nullptr;
	

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName Quiver_Socket;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<AStaticMeshActor> Quiver_Class= nullptr;
	
	UPROPERTY(Replicated)
	AStaticMeshActor* QuiverActorPtr = nullptr;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FTransform ArrowTransform = FTransform::Identity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName Arrow_Socket;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<AStaticMeshActor> Arrow_Class = nullptr;
	
	UPROPERTY(Replicated)
	AStaticMeshActor* ArrowActorPtr = nullptr;
};
