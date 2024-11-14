// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "SceneObj.h"
#include "GenerateType.h"
#include "ItemProxy.h"
#include "BuildSharedData.h"

#include "BuildingBase.generated.h"

class UMaterialInterface;
class UStaticMeshComponent;
class ABuildingBase;

UCLASS()
class PLANET_API UStateSwitchComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	static FName ComponentName;

	UStateSwitchComponent(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay()override;

	virtual void SwitchToNewState(EBuildingState BuildingState);

	template<typename Type = ABuildingBase>
	Type* GetBuildItem();

	virtual void RecordOriginalState();

protected:

	virtual void CacheOriginalMaterials();

	virtual void ResetMaterial();

	virtual void SwitchCollisionToStartCapture();

	virtual void SwitchCollisionToStopCapture();

	virtual void SwitchCollisionToPlacing();

	virtual void SwitchCollisionToPlaced();

	virtual bool Place();

	virtual void PlaceImp(EInputProcessorType HumanActionType);

	ACharacterBase* PlaceCharacterPtr = nullptr;

	UPROPERTY()
	TMap<UStaticMeshComponent*, FMaterialAry>MaterialsMap;

	EBuildingState BuildingState = EBuildingState::kRegular;

};

template<typename Type >
Type* UStateSwitchComponent::GetBuildItem()
{
	return Cast<Type>(GetOwner());
}

UCLASS()
class PLANET_API UBuildingPropertyComponent : public USceneObjPropertyComponent
{
	GENERATED_BODY()

	friend UStateSwitchComponent;
	friend ABuildingBase;

	using FTraverseUpCallback = std::function<void(UBuildingPropertyComponent*)>;

public:

	virtual void BeginPlay()override;

	UBuildingPropertyComponent(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EBuildingsType BuildingType = EBuildingsType::kNone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Capture Info")
	TSubclassOf<ABuildingCaptureData> BuildSharedDataClass;

protected:

};

UCLASS()
class PLANET_API ABuildingBase : public ASceneObj
{
	GENERATED_BODY()

public:

	ABuildingBase(const FObjectInitializer& ObjectInitializer);

	template<typename Type = UStateSwitchComponent>
	Type* GetStateController();

	virtual void BeginPlay()override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason)override;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void Destroyed()override;

	void GenerateCaptureObj();

	using FTravelNode = std::function<void(ThisClass*)>;

	void TravelUp(const FTravelNode& TravelNode);

	void TravelDown(const FTravelNode& TravelNode);

	ABuildingCaptureData* BuildingCaptureDataPtr = nullptr;

	void SetAttchedBuilding(ABuildingBase* NewAttachedBuldingPtr);

protected:

	TSet<ABuildingBase*>AttachedBuildingSet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Capture Point")
	UStateSwitchComponent* StateSwitchComponentPtr = nullptr;

};

template<typename Type>
Type* ABuildingBase::GetStateController()
{
	return Cast<Type>(StateSwitchComponentPtr);
}
