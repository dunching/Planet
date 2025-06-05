// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GenerateTypes.h"

#include "BuildingBase.h"
#include "TrackBase.generated.h"

class ACharacterBase;
class USplineMeshComponent;
class UMaterialInterface;
class USceneComponent;
class USplineComponent;

UCLASS()
class PLANET_API UTrackBasePropertyComponent : public USceneActorPropertyComponent
{
	GENERATED_BODY()

public:

	UTrackBasePropertyComponent(const FObjectInitializer& ObjectInitializer);

private:

};

UCLASS()
class PLANET_API ATrackBase : public ABuildingBase
{
	GENERATED_BODY()

public:

	ATrackBase(const FObjectInitializer& ObjectInitializer);

	EPickType GetPickType()const;

	int32 GetCurHP()const;

	bool Attack(int32 Val);

	virtual void HasbeenInteracted(ACharacterBase* CharacterPtr);

	void UpdateSpline(const FTransform& StartTransForm, const FTransform& StopTransForm, bool bIsUseCutomTan);

	bool GetCanLinkTrack(ETrackVehiclePosState Pos)const;

	FTransform GetLinkPos(ETrackVehiclePosState Pos);

	FBearerState CheckTrackBearerState()const;

	void RemoveTrack(ATrackBase* TartgetTrackPtr);

	TPair<FTransform, float> GetProjectPt(const FVector& TargetPt)const;

	void SetLinkTrack(ATrackBase*NewTrackPtr, ETrackVehiclePosState Pos);

	ATrackBase* GetPrevTrack() { return PreviousTrackPtr; };

	ATrackBase* GetNextTrack() { return NextTrackPtr; };

	FTransform GetSplinePtByLen(float Len)const;

	float GetTrackLen()const;

	float GetTrackTotalLen()const;

protected:

	virtual void BeginPlay() override;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneComponent* SceneCompPtr = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SplineMesh)
		USplineMeshComponent* SplineMesh1Ptr = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SplineMesh)
		USplineMeshComponent* SplineMesh2Ptr = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SplineMesh)
		USplineMeshComponent* SplineMesh3Ptr = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SplineMesh)
		USplineComponent* SplinePtr = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = TrackLen)
		float TrackLen = 1000;

private:

	void GetCircular(
		const FVector& P1,
		const FVector& P2,
		const FVector& P3,
		FVector& PCenter
	);

	virtual int32 GetDefaultHP();

	virtual EPickType SetDefaultPickType();

	ATrackBase* PreviousTrackPtr = nullptr;

	ATrackBase* NextTrackPtr = nullptr;

	int32 HP = 0;

	EPickType PickType = EPickType::kNone;

	ETrackVehicleState TrackVehicleState = ETrackVehicleState::kStop;

};
