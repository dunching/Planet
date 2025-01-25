// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "Common/GenerateType.h"
#include "Component/SceneActorInteractionComponent.h"
#include "CharacterBase.h"
#include "SceneActor.h"

#include "TrackVehicleBase.generated.h"

class UMaterialInterface;
class USceneComponent;
class UStaticMeshComponent;
class ATrackBase;
class UHoldItemComponent;

UCLASS()
class PLANET_API UTrackVehicleItemPropertyComponent : public USceneActorPropertyComponent
{
	GENERATED_BODY()

public:

	UTrackVehicleItemPropertyComponent(const FObjectInitializer& ObjectInitializer);

private:

};

UENUM()
enum class ETrackVehicleDir : uint8
{
	kFront,
	kBack,
	kStop,
};

UCLASS()
class PLANET_API ATrackVehicleBase : public APawn
{
	GENERATED_BODY()

public:

	ATrackVehicleBase(const FObjectInitializer& ObjectInitializer);

	void SetState(ETrackVehicleDir NewTrackVehicleDir);

	ETrackVehicleDir GetState()const { return TrackVehicleDir; };

	void SetAttchedTrack(ATrackBase* NewAttachedTrack);

	ATrackBase* GetAttchedTrack();

	virtual void SetPrevMaterial(UMaterialInterface* PrevMaterialPtr);

	void SetPlaced();

	void DriveToTheWay(ETrackVehicleDir NewTrackVehicleDir);

	void UpdateCurrentRunLen(float NewRunLen);

protected:

	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void Tick(float DeltaSeconds) override;

	USceneActorPropertyComponent* GetDynamicCommonproperty()const { return ItemPropertyComponentPtr; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
		USceneComponent* RootCompPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
		USceneComponent* SceneCompPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh)
		UStaticMeshComponent* WheelMeshFLPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh)
		UStaticMeshComponent* WheelMeshFRPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh)
		UStaticMeshComponent* WheelMeshBFPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh)
		UStaticMeshComponent* WheelMeshBRPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh)
		UStaticMeshComponent* MeshPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterProperty)
		UTrackVehicleItemPropertyComponent* ItemPropertyComponentPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterProperty)
		UInventoryComponent* HoldItemComponentPtr = nullptr;

private:

	void UpdatePos();

	int32 HP = 0;

	float CurSpeed = 0;

	float AbsSpeed = 50;

	float Acceleration = 10;

	FRotator WheelRot = FRotator::ZeroRotator;

	ETrackVehicleDir TrackVehicleDir = ETrackVehicleDir::kStop;

	TArray<TArray<UMaterialInterface*>>StaticMeshMatAry;

	ATrackBase* AttachedTrackTrackPtr = nullptr;

	float RunLen = 0; 

	int32 UpdateRadio = 60;

	float Speed = 10; // Len / S

	FTimerHandle FixPosTimer;

};
