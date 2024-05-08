// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "VoxelNavigationComponent.generated.h"

struct FVoxelNavmesh;

UCLASS()
class VOXELCORE_API UVoxelNavigationComponent final : public UPrimitiveComponent
{
	GENERATED_BODY()

public:
	UVoxelNavigationComponent();

	void SetNavigationMesh(const TSharedPtr<const FVoxelNavmesh>& NewNavigationMesh);
	void ReturnToPool();

	//~ Begin UPrimitiveComponent Interface
	virtual bool ShouldCreatePhysicsState() const override { return false; }
	virtual bool IsNavigationRelevant() const override;
	virtual bool DoCustomNavigableGeometryExport(FNavigableGeometryExport& GeomExport) const override;
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;
	//~ End UPrimitiveComponent Interface

private:
	TSharedPtr<const FVoxelNavmesh> NavigationMesh;
};