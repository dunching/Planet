// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Navigation/VoxelNavigationComponent.h"
#include "Navigation/VoxelNavmesh.h"
#include "AI/NavigationSystemBase.h"
#include "AI/NavigationSystemHelpers.h"

UVoxelNavigationComponent::UVoxelNavigationComponent()
{
	bCanEverAffectNavigation = true;
	bHasCustomNavigableGeometry = EHasCustomNavigableGeometry::EvenIfNotCollidable;
}

void UVoxelNavigationComponent::SetNavigationMesh(const TSharedPtr<const FVoxelNavmesh>& NewNavigationMesh)
{
	VOXEL_FUNCTION_COUNTER();

	NavigationMesh = NewNavigationMesh;

	if (NavigationMesh)
	{
		NavigationMesh->UpdateStats();
	}

	UpdateBounds();

	if (IsRegistered() &&
		GetWorld() &&
		GetWorld()->GetNavigationSystem() &&
		FNavigationSystem::WantsComponentChangeNotifies())
	{
		VOXEL_SCOPE_COUNTER("UpdateComponentData");

		bNavigationRelevant = IsNavigationRelevant();
		FNavigationSystem::UpdateComponentData(*this);
	}
}

void UVoxelNavigationComponent::ReturnToPool()
{
	SetNavigationMesh(nullptr);
}

bool UVoxelNavigationComponent::IsNavigationRelevant() const
{
	return NavigationMesh.IsValid();
}

bool UVoxelNavigationComponent::DoCustomNavigableGeometryExport(FNavigableGeometryExport& GeomExport) const
{
	VOXEL_FUNCTION_COUNTER();

	if (NavigationMesh)
	{
		const TArray<FVector> DoubleVertices(NavigationMesh->Vertices);

		GeomExport.ExportCustomMesh(
			DoubleVertices.GetData(),
			NavigationMesh->Vertices.Num(),
			NavigationMesh->Indices.GetData(),
			NavigationMesh->Indices.Num(),
			GetComponentTransform());
	}

	return false;
}

FBoxSphereBounds UVoxelNavigationComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	const FVoxelBox LocalBounds = NavigationMesh ? NavigationMesh->LocalBounds : FVoxelBox();
	ensure(LocalBounds.IsValid());
	return LocalBounds.TransformBy(LocalToWorld).ToFBox();
}

void UVoxelNavigationComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	VOXEL_FUNCTION_COUNTER();

	Super::OnComponentDestroyed(bDestroyingHierarchy);

	// Clear memory
	NavigationMesh.Reset();
}