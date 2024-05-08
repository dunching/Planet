// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "Rendering/VoxelMesh.h"
#include "Components/PrimitiveComponent.h"
#include "VoxelMeshComponent.generated.h"

UCLASS()
class VOXELCORE_API UVoxelMeshComponent final : public UPrimitiveComponent
{
	GENERATED_BODY()

public:
	bool bOnlyDrawIfSelected = false;
	FVector BoundsExtension = FVector::ZeroVector;

	UVoxelMeshComponent();

	const TSharedPtr<const FVoxelMesh>& GetMesh() const
	{
		return Mesh.Get();
	}
	void SetMesh(const TSharedPtr<const FVoxelMesh>& NewMesh);

	void ReturnToPool();

public:
	//~ Begin UPrimitiveComponent Interface
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual bool ShouldCreatePhysicsState() const override { return false; }

	virtual int32 GetNumMaterials() const override;
	virtual UMaterialInterface* GetMaterial(int32 ElementIndex) const override;
	virtual void GetUsedMaterials(TArray<UMaterialInterface*>& OutMaterials, bool bGetDebugMaterials) const override;

	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;
	//~ End UPrimitiveComponent Interface

	FMaterialRelevance GetMaterialRelevance(ERHIFeatureLevel::Type InFeatureLevel) const;

private:
	FVoxelMeshComponentRef Mesh{ this };

	friend class FVoxelMeshSceneProxy;
};