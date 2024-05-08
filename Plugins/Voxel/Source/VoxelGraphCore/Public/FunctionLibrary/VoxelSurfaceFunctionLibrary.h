// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelSurface.h"
#include "VoxelFunctionLibrary.h"
#include "VoxelSurfaceFunctionLibrary.generated.h"

struct FVoxelFloatDetailTextureRef;
struct FVoxelColorDetailTextureRef;

UENUM()
enum class EVoxelTransformSpace : uint8
{
	// Transform space of the actor of this graph
	// Typically, the current brush transform
	Local,
	// World space transform in unreal units
	World,
	// Transform space of the actor making the query
	// Typically, the voxel actor which is rendering the mesh
	Query
};

UCLASS()
class VOXELGRAPHCORE_API UVoxelSurfaceFunctionLibrary : public UVoxelFunctionLibrary
{
	GENERATED_BODY()

public:
	// Get the bounds of a surface
	// @param	Smoothness	Bounds will be increased by Smoothness. Should be the same as the SmoothUnion smoothness.
	UFUNCTION(Category = "Surface")
	FVoxelBounds GetSurfaceBounds(
		const FVoxelSurface& Surface,
		float Smoothness = 0.f) const;

	UFUNCTION(Category = "Surface", meta = (ShowInShortList))
	FVoxelBounds MakeBoundsFromLocalBox(const FVoxelBox& Box) const;

	UFUNCTION(Category = "Surface")
	FVoxelBox GetBoundsBox(
		const FVoxelBounds& Bounds,
		EVoxelTransformSpace TransformSpace = EVoxelTransformSpace::Local) const;

	UFUNCTION(Category = "Surface")
	FVoxelSurfaceMaterial BlendSurfaceMaterials(
		const FVoxelSurfaceMaterial& A,
		const FVoxelSurfaceMaterial& B,
		const FVoxelFloatBuffer& Alpha) const;

public:
	// Assign a detail texture to a float attribute
	// The float attribute can then be accessed in the material graph through that detail texture
	UFUNCTION(Category = "Surface", meta = (AdvancedDisplay = bLogError))
	FVoxelSurface BindFloatAttributeDetailTexture(
		const FVoxelSurface& Surface,
		FName Name,
		const FVoxelFloatDetailTextureRef& DetailTexture,
		bool bLogError = true) const;

	// Assign a detail texture to a color attribute
	// The color attribute can then be accessed in the material graph through that detail texture
	UFUNCTION(Category = "Surface", meta = (AdvancedDisplay = bLogError))
	FVoxelSurface BindColorAttributeDetailTexture(
		const FVoxelSurface& Surface,
		FName Name,
		const FVoxelColorDetailTextureRef& DetailTexture,
		bool bLogError = true) const;

public:
	UFUNCTION(Category = "Surface", meta = (ShowInShortList))
	FVoxelSurface Invert(const FVoxelSurface& Surface) const;

	// Smoothly merge two surfaces
	// Smoothness is in local space
	// ie, if this brush is scaled up, smoothness will increase accordingly
	UFUNCTION(Category = "Surface", meta = (ShowInShortList))
	FVoxelSurface SmoothUnion(
		const FVoxelSurface& A,
		const FVoxelSurface& B,
		float Smoothness = 100.f) const;

	// Smoothly intersect two surfaces
	// Smoothness is in local space
	// ie, if this brush is scaled up, smoothness will increase accordingly
	UFUNCTION(Category = "Surface", meta = (ShowInShortList))
	FVoxelSurface SmoothIntersection(
		const FVoxelSurface& A,
		const FVoxelSurface& B,
		float Smoothness = 100.f) const;

	// Smoothly subtract a surface from another
	// Smoothness is in local space
	// ie, if this brush is scaled up, smoothness will increase accordingly
	UFUNCTION(Category = "Surface", meta = (ShowInShortList))
	FVoxelSurface SmoothSubtraction(
		const FVoxelSurface& Surface,
		const FVoxelSurface& SurfaceToSubtract,
		float Smoothness = 100.f) const;

public:
	UFUNCTION(Category = "Surface")
	FVoxelSurface MakeSphereSurface(
		const FVector& Center,
		float Radius = 500.f) const;

	UFUNCTION(Category = "Surface")
	FVoxelSurface MakeBoxSurface(
		const FVector& Center,
		const FVector& Extent = FVector(500.f),
		float Smoothness = 100.f) const;

public:
	FVoxelFloatBuffer MakeSphereSurface_Distance(
		const FVector& Center,
		float Radius) const;

	FVoxelFloatBuffer MakeBoxSurface_Distance(
		const FVector& Center,
		const FVector& Extent,
		float Smoothness) const;
};