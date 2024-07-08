
#include "BuildingBase.h"

#include "Components/ArrowComponent.h"
#include "DrawDebugHelpers.h"
#include <Kismet/GameplayStatics.h>
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/BoxComponent.h"

#include "LogWriter.h"

#include <Subsystems/SubsystemBlueprintLibrary.h>

#include "AssetRefMap_SceneObjects.h"
#include "CollisionDataStruct.h"

UStateSwitchComponent::UStateSwitchComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{

}

void UStateSwitchComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UStateSwitchComponent::CacheOriginalMaterials()
{
	auto OnwerPtr = Cast<ABuildingBase>(GetOwner());

	TArray<UStaticMeshComponent*> StaticMeshAry;
	OnwerPtr->GetComponents<UStaticMeshComponent>(StaticMeshAry);

	MaterialsMap.Empty();

	for (int32 Index = 0; Index < StaticMeshAry.Num(); Index++)
	{
		FMaterialAry MaterialAry;

		const auto Num = StaticMeshAry[Index]->GetNumMaterials();
		for (int32 MatIndex = 0; MatIndex < Num; MatIndex++)
		{
			auto OriginalMatPtr = StaticMeshAry[Index]->GetMaterial(MatIndex);
			MaterialAry.MaterialsAry.Add(OriginalMatPtr);
		}

		MaterialsMap.Add(StaticMeshAry[Index], MaterialAry);
	}
}

void UStateSwitchComponent::PlaceImp(EInputProcessorType HumanActionType)
{
}

void UStateSwitchComponent::ResetMaterial()
{
	auto OnwerPtr = Cast<ABuildingBase>(GetOwner());

	TArray<UStaticMeshComponent*> StaticMeshAry;
	OnwerPtr->GetComponents<UStaticMeshComponent>(StaticMeshAry);
	for (int32 Index = 0; Index < StaticMeshAry.Num(); Index++)
	{
		auto MatIter = MaterialsMap.Find(StaticMeshAry[Index]);
		if (!MatIter)
		{
			continue;
		}
		auto Num = StaticMeshAry[Index]->GetNumMaterials();
		for (int32 MatIndex = 0; MatIndex < Num; MatIndex++)
		{
			StaticMeshAry[Index]->SetMaterial(MatIndex, MatIter->MaterialsAry[MatIndex]);
		}
	}
}

void UStateSwitchComponent::SwitchCollisionToStartCapture()
{
	auto OnwerPtr = Cast<ABuildingBase>(GetOwner());

	TArray<UStaticMeshComponent*> StaticMeshAry;
	OnwerPtr->GetComponents<UStaticMeshComponent>(StaticMeshAry);
	for (auto Iter : StaticMeshAry)
	{
		Iter->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		Iter->SetCollisionObjectType(CapturePoint);
		Iter->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	}
}

void UStateSwitchComponent::SwitchCollisionToStopCapture()
{
	auto OnwerPtr = Cast<ABuildingBase>(GetOwner());

	TArray<UStaticMeshComponent*> StaticMeshAry;
	OnwerPtr->GetComponents<UStaticMeshComponent>(StaticMeshAry);
	for (auto Iter : StaticMeshAry)
	{
		Iter->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Iter->SetCollisionObjectType(CapturePoint);
		Iter->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	}
}

void UStateSwitchComponent::SwitchCollisionToPlaced()
{
	auto OnwerPtr = Cast<ABuildingBase>(GetOwner());

	TArray<UStaticMeshComponent*> StaticMeshAry;
	OnwerPtr->GetComponents<UStaticMeshComponent>(StaticMeshAry);
	for (auto Iter : StaticMeshAry)
	{
		Iter->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Iter->SetCollisionObjectType(Building);
		Iter->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		Iter->SetCollisionResponseToChannel(Building, ECollisionResponse::ECR_Block);
		Iter->SetCollisionResponseToChannel(VoxelWorld, ECollisionResponse::ECR_Block);
		Iter->SetCollisionResponseToChannel(PawnECC, ECollisionResponse::ECR_Block);
	}
}

void UStateSwitchComponent::SwitchCollisionToPlacing()
{
	auto OnwerPtr = Cast<ABuildingBase>(GetOwner());
	OnwerPtr->BuildingCaptureDataPtr->TerrainCheckComponnetPtr->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	TArray<UStaticMeshComponent*> StaticMeshAry;
	OnwerPtr->GetComponents<UStaticMeshComponent>(StaticMeshAry);
	for (auto Iter : StaticMeshAry)
	{
		Iter->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

bool UStateSwitchComponent::Place()
{
	return true;
}

void UStateSwitchComponent::SwitchToNewState(EBuildingState BuildingState_)
{
	if (BuildingState_ == BuildingState)
	{
		return;
	}

	BuildingState = BuildingState_;

	switch (BuildingState)
	{
	case EBuildingState::kRegular:
	{
		ResetMaterial();
		SwitchCollisionToPlaced();
	}
	break;
	case EBuildingState::kPreviewPlacing:
	{
		SwitchCollisionToPlacing();
	}
	break;
	case EBuildingState::kStartCapture:
	{
		SwitchCollisionToStartCapture();
	}
	break;
	case EBuildingState::kStopCapture:
	{
		SwitchCollisionToStopCapture();
	}
	break;
	case EBuildingState::kPreviewPlacing_Sucess:
	{
		const auto MaterialPtr =
			UAssetRefMap_SceneObjects::GetInstance()->MaterialInsSoftPath[EMaterialInstanceType::kPlacePreviewMatIns_Success].LoadSynchronous();

		auto OnwerPtr = Cast<ABuildingBase>(GetOwner());
		TArray<UStaticMeshComponent*> StaticMeshAry;
		OnwerPtr->GetComponents<UStaticMeshComponent>(StaticMeshAry);
		for (int32 Index = 0; Index < StaticMeshAry.Num(); Index++)
		{
			auto Num = StaticMeshAry[Index]->GetNumMaterials();
			for (int32 MatIndex = 0; MatIndex < Num; MatIndex++)
			{
				StaticMeshAry[Index]->SetMaterial(MatIndex, MaterialPtr);
			}
		}
	}
	break;
	case EBuildingState::kPreviewPlacing_Fail:
	{
		const auto MaterialPtr =
			UAssetRefMap_SceneObjects::GetInstance()->MaterialInsSoftPath[EMaterialInstanceType::kPlacePreviewMatIns_Fail].LoadSynchronous();

		auto OnwerPtr = Cast<ABuildingBase>(GetOwner());

		TArray<UStaticMeshComponent*> StaticMeshAry;
		OnwerPtr->GetComponents<UStaticMeshComponent>(StaticMeshAry);
		for (int32 Index = 0; Index < StaticMeshAry.Num(); Index++)
		{
			auto Num = StaticMeshAry[Index]->GetNumMaterials();
			for (int32 MatIndex = 0; MatIndex < Num; MatIndex++)
			{
				StaticMeshAry[Index]->SetMaterial(MatIndex, MaterialPtr);
			}
		}
	}
	break;
	case EBuildingState::kPlaced:
	{
		auto OnwerPtr = Cast<ABuildingBase>(GetOwner());
		OnwerPtr->BuildingCaptureDataPtr->StopOverlapCheck();

		ResetMaterial();
		SwitchCollisionToPlaced();
		Place();
	}
	break;
	default:
		break;
	}
}

void UStateSwitchComponent::RecordOriginalState()
{
	CacheOriginalMaterials();
}

FName UStateSwitchComponent::ComponentName = TEXT("StateSwitchComponent");

ABuildingBase::ABuildingBase(const FObjectInitializer& ObjectInitializer) :
	Super(
		ObjectInitializer.SetDefaultSubobjectClass<UBuildingPropertyComponent>(UBuildingPropertyComponent::ComponentName)
	)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	StateSwitchComponentPtr = CreateDefaultSubobject<UStateSwitchComponent>(UStateSwitchComponent::ComponentName);
}

void ABuildingBase::BeginPlay()
{
	Super::BeginPlay();
}

void ABuildingBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (StateSwitchComponentPtr)
	{
		StateSwitchComponentPtr->RecordOriginalState();
	}
}

void ABuildingBase::Destroyed()
{
	Super::Destroyed();
}

void ABuildingBase::GenerateCaptureObj()
{
	BuildingCaptureDataPtr = GetWorld()->SpawnActor<ABuildingCaptureData>(
		GetPropertyComponent<UBuildingPropertyComponent>()->BuildSharedDataClass,
		FTransform::Identity
	);

	BuildingCaptureDataPtr->AttachToActor(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
}

void ABuildingBase::TravelUp(const FTravelNode& TravelNode)
{

}

void ABuildingBase::TravelDown(const FTravelNode& TravelNode)
{
	if (TravelNode)
	{
        for (auto Iter : AttachedBuildingSet)
        {
            if (Iter)
            {
				TravelNode(Iter);
                Iter->TravelDown(TravelNode);
            }
        }
	}
}

void ABuildingBase::SetAttchedBuilding(ABuildingBase* NewAttachedBuldingPtr)
{
	if (NewAttachedBuldingPtr)
	{
		NewAttachedBuldingPtr->AttachedBuildingSet.Add(this);
	}
}

void UBuildingPropertyComponent::BeginPlay()
{
	Super::BeginPlay();
}

UBuildingPropertyComponent::UBuildingPropertyComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
}

void ABuildingBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (BuildingCaptureDataPtr)
	{
		BuildingCaptureDataPtr->Destroy();
	}

	Super::EndPlay(EndPlayReason);
}
