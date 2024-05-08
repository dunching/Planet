// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelNode.h"
#include "VoxelSurface.h"
#include "Point/VoxelPointSet.h"
#include "VoxelRaymarchDistanceFieldNode.generated.h"

struct FVoxelNode_RaymarchDistanceField;

class FVoxelRaymarchDistanceFieldProcessor : public TSharedFromThis<FVoxelRaymarchDistanceFieldProcessor>
{
public:
	const FVoxelNode_RaymarchDistanceField& Node;
	const FVoxelDummyFutureValue Dummy;
	const FVoxelQuery BaseQuery;
	const TSharedRef<const FVoxelSurface> Surface;
	const bool bUpdateNormal;
	const float KillDistance;
	const float Tolerance;
	const int32 MaxSteps;
	const float Speed;
	const float GradientStep;
	const TSharedRef<const FVoxelPointSet> Points;
	const FVoxelVectorBuffer Normals;

	FVoxelRaymarchDistanceFieldProcessor(
		const FVoxelNode_RaymarchDistanceField& Node,
		const FVoxelDummyFutureValue& Dummy,
		const FVoxelQuery& BaseQuery,
		const TSharedRef<const FVoxelSurface>& Surface,
		const bool bUpdateNormal,
		const float KillDistance,
		const float Tolerance,
		const int32 MaxSteps,
		const float Speed,
		const float GradientStep,
		const TSharedRef<const FVoxelPointSet>& Points,
		const FVoxelVectorBuffer& Positions,
		const FVoxelVectorBuffer& Normals)
		: Node(Node)
		, Dummy(Dummy)
		, BaseQuery(BaseQuery)
		, Surface(Surface)
		, bUpdateNormal(bUpdateNormal)
		, KillDistance(KillDistance)
		, Tolerance(Tolerance)
		, MaxSteps(FMath::Min(MaxSteps, 32))
		, Speed(Speed)
		, GradientStep(GradientStep)
		, Points(Points)
		, Normals(Normals)
		, PositionX(Positions.X.GetStorage().Clone())
		, PositionY(Positions.Y.GetStorage().Clone())
		, PositionZ(Positions.Z.GetStorage().Clone())
	{
		ensure(MaxSteps > 0);
	}

	void Compute();
	TSharedRef<FVoxelPointSet> GetNewPoints() const;

private:
	const TSharedRef<FVoxelFloatBufferStorage> PositionX;
	const TSharedRef<FVoxelFloatBufferStorage> PositionY;
	const TSharedRef<FVoxelFloatBufferStorage> PositionZ;

	int32 Step = 0;
	TVoxelArray<int32> IndicesToProcess;
	TVoxelArray<float> Distances;
	TOptional<FVoxelVectorBuffer> NewPointNormals;
	TVoxelArray<int32> PointsToRemove;

	void ProcessDistances(const FVoxelFloatBuffer& NewDistances);
	void ProcessGradient(const FVoxelVectorBuffer& Gradients);
	void Finalize();
};

USTRUCT(Category = "Point")
struct VOXELGRAPHNODES_API FVoxelNode_RaymarchDistanceField : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelPointSet, In, nullptr);
	// Surface to raymarch
	VOXEL_INPUT_PIN(FVoxelSurface, Surface, nullptr);
	// If true, will set the new point normal to be the distance field gradient
	// at the refined point positions
	VOXEL_INPUT_PIN(bool, UpdateNormal, false);
	// If a point is further away than this distance from the surface
	// at the end of the raymarching, it will be removed
	VOXEL_INPUT_PIN(float, KillDistance, 1000.f);
	// If a point is within Tolerance distance from the surface,
	// it will be marked as done and won't be raymarched any further
	VOXEL_INPUT_PIN(float, Tolerance, 5.f, AdvancedDisplay);
	// Max number of steps to do
	// Keep low for performance
	VOXEL_INPUT_PIN(int32, MaxSteps, 5, AdvancedDisplay);
	// How "fast" to converge to the surface, between 0 and 1
	// NewPoint = OldPoint + DistanceToSurface * Direction * Speed
	// Decrease if the raymarching is imprecise
	VOXEL_INPUT_PIN(float, Speed, 0.8f, AdvancedDisplay);
	// Distance between points when sampling gradients
	VOXEL_INPUT_PIN(float, GradientStep, 100.f, AdvancedDisplay);
	VOXEL_OUTPUT_PIN(FVoxelPointSet, Out);
};