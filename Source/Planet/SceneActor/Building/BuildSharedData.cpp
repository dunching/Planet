// Fill out your copyright notice in the Description page of Project Settings.

#include "BuildSharedData.h"

#include "Components/BoxComponent.h"
#include "Components/ChildActorComponent.h"

#include "BuildingBase.h"
#include "CollisionDataStruct.h"

ABuildingCaptureData::ABuildingCaptureData(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	TerrainCheckComponnetPtr = CreateDefaultSubobject<UBoxComponent>(TEXT("TerrainCheckBoxComponnet"));
	TerrainCheckComponnetPtr->SetupAttachment(RootComponent);
}

void ABuildingCaptureData::BeginPlay()
{
	Super::BeginPlay();

	for (auto Iter : CaptureMap)
	{
		if (Iter.Value)
		{
			for (auto SecondIter : Iter.Value->CapturesInfoAry)
			{
				SecondIter->CapturePtPtr->SetActorHiddenInGame(true);
			}
		}
	}
	TerrainCheckComponnetPtr->SetVisibility(false);

	StopOverlapCheck();
}

void ABuildingCaptureData::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	auto Childrens = RootComponent->GetAttachChildren();

	for (auto Iter : Childrens)
	{
		if (Iter->GetOwner()->IsA(ABuildingBase::StaticClass()))
		{
			Iter->GetOwner()->Destroy();
		}
	}

	Super::EndPlay(EndPlayReason);
}

void ABuildingCaptureData::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	CaptureMap.Empty();

	TArray<UCustomChildActorComponent*>ChildActorAry;
	GetComponents<UCustomChildActorComponent>(ChildActorAry);

	ChildActorAry.Sort([](const UCustomChildActorComponent&Left, const UCustomChildActorComponent& Right) {
		return Left.CaptureOrder < Right.CaptureOrder;
		});

	for (auto MeshIter : ChildActorAry)
	{
		auto Ary = CaptureMap.Find(MeshIter->BuildingType);
		if (Ary)
		{
			auto CaptureInfo = MakeShared<FCapturesInfo::FCaptureInfo>();
			CaptureInfo->CapturePtPtr = Cast<ABuildingBase>(MeshIter->GetChildActor());

			(*Ary)->CapturesInfoAry.Add(CaptureInfo);
		}
		else
		{
			auto CaptureArraySPtr = MakeShared<FCapturesInfo>();

			auto CaptureInfo = MakeShared<FCapturesInfo::FCaptureInfo>();
			CaptureInfo->CapturePtPtr = Cast<ABuildingBase>(MeshIter->GetChildActor());

			CaptureArraySPtr->CapturesInfoAry.Add(CaptureInfo);

			CaptureMap.Add(MeshIter->BuildingType, CaptureArraySPtr);
		}
	}
}

void ABuildingCaptureData::StartOverlapCheck(EBuildingsType ItemsType)
{
	auto MeshComponentArySPtr = GetCaptureAry(ItemsType);
	if (MeshComponentArySPtr)
	{
		for (auto MeshIter : MeshComponentArySPtr->CapturesInfoAry)
		{
			MeshIter->CapturePtPtr->GetStateController()->SwitchToNewState(EBuildingState::kStartCapture);
		}
	}
	TerrainCheckComponnetPtr->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABuildingCaptureData::StopOverlapCheck()
{
	for (auto Iter : CaptureMap)
	{
		if (Iter.Value)
		{
			for (auto SecondIter : Iter.Value->CapturesInfoAry)
			{
				if (SecondIter->CapturePtPtr)
				{
					SecondIter->CapturePtPtr->GetStateController()->SwitchToNewState(EBuildingState::kStopCapture);
				}
			}
		}
	}

	TerrainCheckComponnetPtr->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

TSharedPtr<FCapturesInfo> ABuildingCaptureData::GetCaptureAry(EBuildingsType ItemsType) const
{
	if (CaptureMap.Contains(ItemsType))
	{
		return CaptureMap[ItemsType];
	}
	return nullptr;
}

#if WITH_EDITOR
void UCustomChildActorComponent::PostLoad()
{
	Super::PostLoad();

	auto ChildActorPtr = GetChildActor();
	if (ChildActorPtr)
	{
	}
}
#endif