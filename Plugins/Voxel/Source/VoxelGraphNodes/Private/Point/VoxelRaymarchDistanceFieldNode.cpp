// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Point/VoxelRaymarchDistanceFieldNode.h"
#include "VoxelPositionQueryParameter.h"
#include "VoxelGradientNodes.h"

void FVoxelRaymarchDistanceFieldProcessor::Compute()
{
	VOXEL_FUNCTION_COUNTER();
	FVoxelNodeStatScope StatScope(Node, 0);

	FVoxelVectorBuffer Positions;
	Positions.X.SetStorage(PositionX);
	Positions.Y.SetStorage(PositionY);
	Positions.Z.SetStorage(PositionZ);

	const TSharedRef<FVoxelQueryParameters> Parameters = BaseQuery.CloneParameters();
	Parameters->Add<FVoxelGradientStepQueryParameter>().Step = GradientStep;
	Parameters->Add<FVoxelPositionQueryParameter>().Initialize(Positions);

	const TVoxelFutureValue<FVoxelFloatBuffer> NewDistances = Surface->GetDistance(BaseQuery.MakeNewQuery(Parameters));

	MakeVoxelTask()
	.Dependency(NewDistances)
	.Execute(MakeWeakPtrLambda(this, [=]
	{
		ProcessDistances(NewDistances.Get_CheckCompleted());
	}));
}

TSharedRef<FVoxelPointSet> FVoxelRaymarchDistanceFieldProcessor::GetNewPoints() const
{
	VOXEL_FUNCTION_COUNTER();
	FVoxelNodeStatScope StatScope(Node, 0);

	FVoxelVectorBuffer Positions;
	Positions.X.SetStorage(PositionX);
	Positions.Y.SetStorage(PositionY);
	Positions.Z.SetStorage(PositionZ);

	const TSharedRef<FVoxelPointSet> NewPoints = MakeSharedCopy(*Points);
	NewPoints->Add(FVoxelPointAttributes::Position, Positions);

	if (NewPointNormals)
	{
		NewPoints->Add(FVoxelPointAttributes::Normal, *NewPointNormals);
	}

	if (PointsToRemove.Num() == 0)
	{
		return NewPoints;
	}

	VOXEL_SCOPE_COUNTER("PointsToRemove");
	checkVoxelSlow(Algo::IsSorted(PointsToRemove));

	int32 PointsToRemoveIndex = 0;
	FVoxelInt32BufferStorage Indices;
	for (int32 Index = 0; Index < Points->Num(); Index++)
	{
		if (PointsToRemoveIndex < PointsToRemove.Num() &&
			PointsToRemove[PointsToRemoveIndex] == Index)
		{
			PointsToRemoveIndex++;
			continue;
		}

		Indices.Add(Index);
	}

	return NewPoints->Gather(FVoxelInt32Buffer::Make(Indices));
}

void FVoxelRaymarchDistanceFieldProcessor::ProcessDistances(const FVoxelFloatBuffer& NewDistances)
{
	VOXEL_FUNCTION_COUNTER();
	FVoxelNodeStatScope StatScope(Node, 0);

	if (Step == 0)
	{
		const int32 Num = Points->Num();
		IndicesToProcess.Reserve(Num);
		Distances.Reserve(Num);

		if (!ensure(NewDistances.IsConstant() || NewDistances.Num() == Num))
		{
			Finalize();
			return;
		}

		for (int32 Index = 0; Index < Num; Index++)
		{
			const float Distance = NewDistances[Index];
			if (FMath::Abs(Distance) <= Tolerance)
			{
				continue;
			}

			IndicesToProcess.Add_NoGrow(Index);
			Distances.Add_NoGrow(Distance);
		}
	}
	else
	{
		const TVoxelArray<int32> OldIndicesToProcess = MoveTemp(IndicesToProcess);

		const int32 Num = OldIndicesToProcess.Num();
		IndicesToProcess.Reset(Num);
		Distances.Reset(Num);

		if (!ensure(NewDistances.IsConstant() || NewDistances.Num() == Num))
		{
			Finalize();
			return;
		}

		for (int32 Index = 0; Index < Num; Index++)
		{
			const float Distance = NewDistances[Index];
			if (FMath::Abs(Distance) <= Tolerance)
			{
				continue;
			}

			IndicesToProcess.Add_NoGrow(OldIndicesToProcess[Index]);
			Distances.Add_NoGrow(Distance);
		}
	}

	if (IndicesToProcess.Num() == 0)
	{
		Finalize();
		return;
	}

	FVoxelFloatBufferStorage NewPositionX;
	FVoxelFloatBufferStorage NewPositionY;
	FVoxelFloatBufferStorage NewPositionZ;
	NewPositionX.Allocate(IndicesToProcess.Num());
	NewPositionY.Allocate(IndicesToProcess.Num());
	NewPositionZ.Allocate(IndicesToProcess.Num());
	for (int32 Index = 0; Index < IndicesToProcess.Num(); Index++)
	{
		const int32 IndexToProcess = IndicesToProcess[Index];
		NewPositionX[Index] = (*PositionX)[IndexToProcess];
		NewPositionY[Index] = (*PositionY)[IndexToProcess];
		NewPositionZ[Index] = (*PositionZ)[IndexToProcess];
	}

	const TSharedRef<FVoxelQueryParameters> Parameters = BaseQuery.CloneParameters();
	Parameters->Add<FVoxelGradientStepQueryParameter>().Step = GradientStep;
	Parameters->Add<FVoxelPositionQueryParameter>().Initialize(FVoxelVectorBuffer::Make(
		NewPositionX,
		NewPositionY,
		NewPositionZ));

	const TSharedRef<const FVoxelSurface> LocalSurface = Surface;

	const TVoxelFutureValue<FVoxelVectorBuffer> Gradient = VOXEL_CALL_NODE(FVoxelNode_GetGradient, GradientPin, BaseQuery.MakeNewQuery(Parameters))
	{
		VOXEL_CALL_NODE_BIND(ValuePin, LocalSurface)
		{
			return LocalSurface->GetDistance(Query);
		};
	};

	MakeVoxelTask()
	.Dependency(Gradient)
	.Execute(MakeWeakPtrLambda(this, [=]
	{
		ProcessGradient(Gradient.Get_CheckCompleted());
	}));
}

void FVoxelRaymarchDistanceFieldProcessor::ProcessGradient(const FVoxelVectorBuffer& Gradients)
{
	VOXEL_FUNCTION_COUNTER();
	ensure(IndicesToProcess.Num() > 0);
	FVoxelNodeStatScope StatScope(Node, 0);

	if (!ensure(Gradients.IsConstant() || Gradients.Num() == IndicesToProcess.Num()))
	{
		Finalize();
		return;
	}

	for (int32 Index = 0; Index < IndicesToProcess.Num(); Index++)
	{
		const int32 IndexToProcess = IndicesToProcess[Index];
		const float Distance = Distances[Index];
		const FVector3f Gradient = Gradients[Index];
		const FVector3f Normal = Normals[IndexToProcess];

		const float Value = FVector3f::DotProduct(Gradient.GetSafeNormal(), Normal.GetSafeNormal()) * Distance * Speed;

		(*PositionX)[IndexToProcess] -= Normal.X * Value;
		(*PositionY)[IndexToProcess] -= Normal.Y * Value;
		(*PositionZ)[IndexToProcess] -= Normal.Z * Value;
	}

	Step++;

	if (Step == MaxSteps)
	{
		Finalize();
		return;
	}

	FVoxelFloatBufferStorage NewPositionX;
	FVoxelFloatBufferStorage NewPositionY;
	FVoxelFloatBufferStorage NewPositionZ;
	NewPositionX.Allocate(IndicesToProcess.Num());
	NewPositionY.Allocate(IndicesToProcess.Num());
	NewPositionZ.Allocate(IndicesToProcess.Num());
	for (int32 Index = 0; Index < IndicesToProcess.Num(); Index++)
	{
		const int32 IndexToProcess = IndicesToProcess[Index];
		NewPositionX[Index] = (*PositionX)[IndexToProcess];
		NewPositionY[Index] = (*PositionY)[IndexToProcess];
		NewPositionZ[Index] = (*PositionZ)[IndexToProcess];
	}

	const TSharedRef<FVoxelQueryParameters> Parameters = BaseQuery.CloneParameters();
	Parameters->Add<FVoxelGradientStepQueryParameter>().Step = GradientStep;
	Parameters->Add<FVoxelPositionQueryParameter>().Initialize(FVoxelVectorBuffer::Make(
		NewPositionX,
		NewPositionY,
		NewPositionZ));

	const TVoxelFutureValue<FVoxelFloatBuffer> NewDistances = Surface->GetDistance(BaseQuery.MakeNewQuery(Parameters));

	MakeVoxelTask()
	.Dependency(NewDistances)
	.Execute(MakeWeakPtrLambda(this, [=]
	{
		ProcessDistances(NewDistances.Get_CheckCompleted());
	}));
}

void FVoxelRaymarchDistanceFieldProcessor::Finalize()
{
	VOXEL_FUNCTION_COUNTER();
	FVoxelNodeStatScope StatScope(Node, Points->Num());

	PointsToRemove.Reserve(IndicesToProcess.Num());
	for (int32 Index = 0; Index < IndicesToProcess.Num(); Index++)
	{
		if (FMath::Abs(Distances[Index]) > KillDistance)
		{
			PointsToRemove.Add(IndicesToProcess[Index]);
		}
	}

	if (!bUpdateNormal)
	{
		Dummy.MarkDummyAsCompleted();
		return;
	}

	FVoxelVectorBuffer Position;
	Position.X.SetStorage(PositionX);
	Position.Y.SetStorage(PositionY);
	Position.Z.SetStorage(PositionZ);

	const TSharedRef<FVoxelQueryParameters> Parameters = BaseQuery.CloneParameters();
	Parameters->Add<FVoxelGradientStepQueryParameter>().Step = GradientStep;
	Parameters->Add<FVoxelPositionQueryParameter>().Initialize(Position);

	const TSharedRef<const FVoxelSurface> LocalSurface = Surface;

	const TVoxelFutureValue<FVoxelVectorBuffer> Gradient = VOXEL_CALL_NODE(FVoxelNode_GetGradient, GradientPin, BaseQuery.MakeNewQuery(Parameters))
	{
		VOXEL_CALL_NODE_BIND(ValuePin, LocalSurface)
		{
			return LocalSurface->GetDistance(Query);
		};
	};

	MakeVoxelTask()
	.Dependency(Gradient)
	.Execute(MakeWeakPtrLambda(this, [=]
	{
		NewPointNormals = Gradient.Get_CheckCompleted();
		Dummy.MarkDummyAsCompleted();
	}));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_RaymarchDistanceField, Out)
{
	const TValue<FVoxelPointSet> Points = Get(InPin, Query);
	const TValue<FVoxelSurface> Surface = Get(SurfacePin, Query);
	const TValue<bool> UpdateNormal = Get(UpdateNormalPin, Query);
	const TValue<float> KillDistance = Get(KillDistancePin, Query);
	const TValue<float> Tolerance = Get(TolerancePin, Query);
	const TValue<int32> MaxSteps = Get(MaxStepsPin, Query);
	const TValue<float> Speed = Get(SpeedPin, Query);
	const TValue<float> GradientStep = Get(GradientStepPin, Query);

	return VOXEL_ON_COMPLETE(Points, Surface, UpdateNormal, KillDistance, Tolerance, MaxSteps, Speed, GradientStep)
	{
		if (Points->Num() == 0)
		{
			return {};
		}
		if (MaxSteps <= 0)
		{
			return Points;
		}

		FindVoxelPointSetAttribute(*Points, FVoxelPointAttributes::Position, FVoxelVectorBuffer, PositionBuffer);
		FindVoxelPointSetAttribute(*Points, FVoxelPointAttributes::Normal, FVoxelVectorBuffer, NormalBuffer);

		const FVoxelDummyFutureValue Dummy = FVoxelFutureValue::MakeDummy();

		const TSharedRef<FVoxelRaymarchDistanceFieldProcessor> Processor = MakeVoxelShared<FVoxelRaymarchDistanceFieldProcessor>(
			*this,
			Dummy,
			Query,
			Surface,
			UpdateNormal,
			KillDistance,
			Tolerance,
			MaxSteps,
			Speed,
			GradientStep,
			Points,
			PositionBuffer,
			NormalBuffer);

		Processor->Compute();

		return VOXEL_ON_COMPLETE(Processor, Dummy)
		{
			return Processor->GetNewPoints();
		};
	};
}