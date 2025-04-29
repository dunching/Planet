// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Tornado.generated.h"

class UCapsuleComponent;
class USphereComponent;

class ACharacterBase;
class UItemProxy_Description_ActiveSkill_Tornado;

struct FSkillProxy;

/*
*	
*/
UCLASS()
class PLANET_API ATornado :
	public AActor
{
	GENERATED_BODY()

public:
	using FOwnerType = ACharacterBase;

	ATornado(
		const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()
	);

	virtual void Tick(
		float DeltaTime
	) override;

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
	) const override;

	void SetData(
		const TObjectPtr<UItemProxy_Description_ActiveSkill_Tornado>& ItemProxy_DescriptionPtr,
		
		const TSharedPtr<FSkillProxy>& InSkillProxyPtr ,

		FVector Direction
	);

private:
	UFUNCTION()
	void OnBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TObjectPtr<UCapsuleComponent> CapsuleComponentPtr = nullptr;


	FVector Direction = FVector::ZeroVector;

	TSet<TObjectPtr<ACharacterBase>> HasBeenEffectedSet;

public:

	UPROPERTY(Replicated)
	TObjectPtr<UItemProxy_Description_ActiveSkill_Tornado> ItemProxy_DescriptionPtr = nullptr;

	TSharedPtr<FSkillProxy> SkillProxyPtr = nullptr;

	float CurrentTime = 0.f;
};
