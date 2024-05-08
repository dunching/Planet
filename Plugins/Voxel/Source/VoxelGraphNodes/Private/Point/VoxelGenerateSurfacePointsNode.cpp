// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Point/VoxelGenerateSurfacePointsNode.h"
#include "VoxelRuntime.h"
#include "TransvoxelData.h"
#include "VoxelPositionQueryParameter.h"
#include "VoxelGenerateSurfacePointsNodeImpl.ispc.generated.h"

void FVoxelGenerateSurfacePointsBuilder::Compute()
{
	VOXEL_FUNCTION_COUNTER();
	FVoxelNodeStatScope StatScope(Node, 0);

	// / 4 to allow for distance checks
	//BaseCellSize = MaxResolution / 4; TODO
	BaseCellSize = TargetCellSize;

	const double Size = Bounds.Size().GetMax() / BaseCellSize;
	ensure(Size >= 0);

	const int64 NewDepth = FMath::Max<int64>(1, FMath::CeilLogTwo64(FMath::CeilToInt64(Size)));
	if (NewDepth > 10)
	{
		VOXEL_MESSAGE(Error, "{0}: bounds too big", Node);
		Finalize();
		return;
	}

	ensure(Depth == -1);
	Depth = NewDepth;

	Offset = Bounds.Min;

	ensure(Cells.Num() == 0);
	Cells.Add(FCell(FIntVector::ZeroValue));

	ComputeDistances();
}

void FVoxelGenerateSurfacePointsBuilder::ComputeDistances()
{
	VOXEL_SCOPE_COUNTER_FORMAT("FVoxelGenerateSurfacePointsBuilder::ComputeDistances Depth=%d NumCells=%d", Depth, Cells.Num());
	FVoxelNodeStatScope StatScope(Node, 0);

	const int32 CellSize = 1 << Depth;

	FVoxelFloatBufferStorage QueryX; QueryX.Allocate(Cells.Num() * 8);
	FVoxelFloatBufferStorage QueryY; QueryY.Allocate(Cells.Num() * 8);
	FVoxelFloatBufferStorage QueryZ; QueryZ.Allocate(Cells.Num() * 8);

	ensure(CellSize % 2 == 0);
	const float HalfCellSize = CellSize / 2.f;
	const float QuarterCellSize = CellSize / 4.f;

	for (int32 Index = 0; Index < Cells.Num(); Index++)
	{
		const FIntVector Cell = Cells[Index].Vector();

		QueryX[8 * Index + 0] = Offset.X + (Cell.X + 1 * QuarterCellSize) * BaseCellSize;
		QueryX[8 * Index + 1] = Offset.X + (Cell.X + 3 * QuarterCellSize) * BaseCellSize;
		QueryX[8 * Index + 2] = Offset.X + (Cell.X + 1 * QuarterCellSize) * BaseCellSize;
		QueryX[8 * Index + 3] = Offset.X + (Cell.X + 3 * QuarterCellSize) * BaseCellSize;
		QueryX[8 * Index + 4] = Offset.X + (Cell.X + 1 * QuarterCellSize) * BaseCellSize;
		QueryX[8 * Index + 5] = Offset.X + (Cell.X + 3 * QuarterCellSize) * BaseCellSize;
		QueryX[8 * Index + 6] = Offset.X + (Cell.X + 1 * QuarterCellSize) * BaseCellSize;
		QueryX[8 * Index + 7] = Offset.X + (Cell.X + 3 * QuarterCellSize) * BaseCellSize;

		QueryY[8 * Index + 0] = Offset.Y + (Cell.Y + 1 * QuarterCellSize) * BaseCellSize;
		QueryY[8 * Index + 1] = Offset.Y + (Cell.Y + 1 * QuarterCellSize) * BaseCellSize;
		QueryY[8 * Index + 2] = Offset.Y + (Cell.Y + 3 * QuarterCellSize) * BaseCellSize;
		QueryY[8 * Index + 3] = Offset.Y + (Cell.Y + 3 * QuarterCellSize) * BaseCellSize;
		QueryY[8 * Index + 4] = Offset.Y + (Cell.Y + 1 * QuarterCellSize) * BaseCellSize;
		QueryY[8 * Index + 5] = Offset.Y + (Cell.Y + 1 * QuarterCellSize) * BaseCellSize;
		QueryY[8 * Index + 6] = Offset.Y + (Cell.Y + 3 * QuarterCellSize) * BaseCellSize;
		QueryY[8 * Index + 7] = Offset.Y + (Cell.Y + 3 * QuarterCellSize) * BaseCellSize;

		QueryZ[8 * Index + 0] = Offset.Z + (Cell.Z + 1 * QuarterCellSize) * BaseCellSize;
		QueryZ[8 * Index + 1] = Offset.Z + (Cell.Z + 1 * QuarterCellSize) * BaseCellSize;
		QueryZ[8 * Index + 2] = Offset.Z + (Cell.Z + 1 * QuarterCellSize) * BaseCellSize;
		QueryZ[8 * Index + 3] = Offset.Z + (Cell.Z + 1 * QuarterCellSize) * BaseCellSize;
		QueryZ[8 * Index + 4] = Offset.Z + (Cell.Z + 3 * QuarterCellSize) * BaseCellSize;
		QueryZ[8 * Index + 5] = Offset.Z + (Cell.Z + 3 * QuarterCellSize) * BaseCellSize;
		QueryZ[8 * Index + 6] = Offset.Z + (Cell.Z + 3 * QuarterCellSize) * BaseCellSize;
		QueryZ[8 * Index + 7] = Offset.Z + (Cell.Z + 3 * QuarterCellSize) * BaseCellSize;
	}

	const TSharedRef<FVoxelQueryParameters> Parameters = BaseQuery.CloneParameters();
	Parameters->Add<FVoxelGradientStepQueryParameter>().Step = BaseCellSize * CellSize;
	Parameters->Add<FVoxelPositionQueryParameter>().Initialize(FVoxelVectorBuffer::Make(QueryX, QueryY, QueryZ));
	Parameters->Add<FVoxelMinExactDistanceQueryParameter>().MinExactDistance = BaseCellSize * HalfCellSize * UE_SQRT_2;

	const TVoxelFutureValue<FVoxelFloatBuffer> Distances = Surface->GetDistance(BaseQuery.MakeNewQuery(Parameters));

	MakeVoxelTask()
	.Dependency(Distances)
	.Execute(MakeWeakPtrLambda(this, [=]
	{
		ProcessDistances(Distances.Get_CheckCompleted().GetStorage());
	}));
}

void FVoxelGenerateSurfacePointsBuilder::ProcessDistances(const FVoxelFloatBufferStorage& Distances)
{
	VOXEL_SCOPE_COUNTER_FORMAT("FVoxelGenerateSurfacePointsBuilder::ProcessDistances Depth=%d NumCells=%d", Depth, Cells.Num());
	FVoxelNodeStatScope StatScope(Node, 0);

	if (Distances.IsConstant() ||
		!ensure(Distances.Num() == 8 * Cells.Num()))
	{
		Finalize();
		return;
	}

	const int32 CellSize = 1 << Depth;
	ensure(CellSize % 2 == 0);
	const int32 HalfCellSize = CellSize / 2;
	const float MinDistance = CellSize * BaseCellSize / 4.f * UE_SQRT_2 * (1.f + DistanceChecksTolerance);

	const FVoxelIntBox IntBounds = FVoxelIntBox::FromFloatBox_WithPadding(Bounds.ShiftBy(-Offset) / BaseCellSize);

	TVoxelChunkedArray<FCell> NewCells;
	{
		VOXEL_SCOPE_COUNTER("Find new cells");

		for (int32 Index = 0; Index < Cells.Num(); Index++)
		{
			const FIntVector Cell = Cells[Index].Vector();
			if (!IntBounds.Intersect(FVoxelIntBox(
				Cell,
				Cell + CellSize)))
			{
				continue;
			}

			if (FMath::Abs(Distances.LoadFast(8 * Index + 0)) < MinDistance)
			{
				NewCells.Add(FCell(Cell + FIntVector(0, 0, 0)));
			}
			if (FMath::Abs(Distances.LoadFast(8 * Index + 1)) < MinDistance)
			{
				NewCells.Add(FCell(Cell + FIntVector(HalfCellSize, 0, 0)));
			}
			if (FMath::Abs(Distances.LoadFast(8 * Index + 2)) < MinDistance)
			{
				NewCells.Add(FCell(Cell + FIntVector(0, HalfCellSize, 0)));
			}
			if (FMath::Abs(Distances.LoadFast(8 * Index + 3)) < MinDistance)
			{
				NewCells.Add(FCell(Cell + FIntVector(HalfCellSize, HalfCellSize, 0)));
			}
			if (FMath::Abs(Distances.LoadFast(8 * Index + 4)) < MinDistance)
			{
				NewCells.Add(FCell(Cell + FIntVector(0, 0, HalfCellSize)));
			}
			if (FMath::Abs(Distances.LoadFast(8 * Index + 5)) < MinDistance)
			{
				NewCells.Add(FCell(Cell + FIntVector(HalfCellSize, 0, HalfCellSize)));
			}
			if (FMath::Abs(Distances.LoadFast(8 * Index + 6)) < MinDistance)
			{
				NewCells.Add(FCell(Cell + FIntVector(0, HalfCellSize, HalfCellSize)));
			}
			if (FMath::Abs(Distances.LoadFast(8 * Index + 7)) < MinDistance)
			{
				NewCells.Add(FCell(Cell + FIntVector(HalfCellSize, HalfCellSize, HalfCellSize)));
			}
		}
	}

	Depth--;
	Cells = MoveTemp(NewCells);

	if (Cells.Num() == 0)
	{
		Finalize();
		return;
	}

	if (Depth > 0)
	{
		ComputeDistances();
		return;
	}

	ComputeFinalDistances();
}

void FVoxelGenerateSurfacePointsBuilder::ComputeFinalDistances()
{
	VOXEL_SCOPE_COUNTER_FORMAT("FVoxelGenerateSurfacePointsBuilder::ComputeFinalDistances Depth=%d NumCells=%d", Depth, Cells.Num());
	FVoxelNodeStatScope StatScope(Node, 0);
	ensure(Depth == 0);

	TVoxelChunkedArray<TVoxelStaticArray<int32, 8>> ValueIndices;
	ValueIndices.SetNumUninitialized(Cells.Num());

	FVoxelFloatBufferStorage QueryX; QueryX.Reserve(8 * Cells.Num());
	FVoxelFloatBufferStorage QueryY; QueryY.Reserve(8 * Cells.Num());
	FVoxelFloatBufferStorage QueryZ; QueryZ.Reserve(8 * Cells.Num());
	int32 NumQueries = 0;

	if (false)
	{
		// Naive slow version

		NumQueries = 8 * Cells.Num();

		for (int32 Index = 0; Index < Cells.Num(); Index++)
		{
			const FIntVector Cell = Cells[Index].Vector();

			for (int32 ChildIndex = 0; ChildIndex < 8; ChildIndex++)
			{
				QueryX.Add(Offset.X + (Cell.X + bool(ChildIndex & 0x1)) * BaseCellSize);
				QueryY.Add(Offset.Y + (Cell.Y + bool(ChildIndex & 0x2)) * BaseCellSize);
				QueryZ.Add(Offset.Z + (Cell.Z + bool(ChildIndex & 0x4)) * BaseCellSize);

				ValueIndices[Index][ChildIndex] = 8 * Index + ChildIndex;
			}
		}
	}
	else
	{
		constexpr int32 HashSize = GVoxelDefaultAllocationSize / sizeof(int32);
		constexpr int32 MaxNumElements = HashSize;

		TVoxelArray<int32> HashTable;
		FVoxelUtilities::SetNumFast(HashTable, HashSize);

		// Needed to allow querying [-1],
		constexpr int32 Padding = 1;

		TVoxelArray<uint32> Elements_Values_Padded;
		TVoxelArray<int32> Elements_NextIndices_Padded;
		FVoxelUtilities::SetNumFast(Elements_Values_Padded, MaxNumElements);
		FVoxelUtilities::SetNumFast(Elements_NextIndices_Padded, MaxNumElements);

		const TVoxelArrayView<uint32> Elements_Values = MakeVoxelArrayView(Elements_Values_Padded).Slice(Padding, MaxNumElements - Padding);
		const TVoxelArrayView<int32> Elements_NextIndices = MakeVoxelArrayView(Elements_NextIndices_Padded).Slice(Padding, MaxNumElements - Padding);

		for (int32 CellIndex = 0; CellIndex < Cells.Num(); CellIndex += ValueIndices.NumPerChunk)
		{
			FVoxelUtilities::LargeMemset(HashTable, 0xFF);

			checkStatic(ValueIndices.NumPerChunk < TVoxelChunkedArray<FCell>::NumPerChunk);
			const int32 NumToProcess = FMath::Min(ValueIndices.NumPerChunk, Cells.Num() - CellIndex);
			ensure(NumToProcess * 8 <= MaxNumElements);

			checkVoxelSlow(&Cells[CellIndex + NumToProcess - 1] - &Cells[CellIndex] == NumToProcess - 1);
			checkVoxelSlow(ValueIndices[CellIndex + NumToProcess - 1].GetData() - ValueIndices[CellIndex].GetData() == 8 * (NumToProcess - 1));

			const int32 NumElements = ispc::GenerateSurfacePointsBuilder_ComputeFinalDistances_ComputeIndices(
				ReinterpretCastPtr<uint32>(&Cells[CellIndex]),
				ValueIndices[CellIndex].GetData(),
				HashSize,
				HashTable.GetData(),
				Elements_Values.GetData(),
				Elements_NextIndices.GetData(),
				NumQueries,
				NumToProcess);
			ensure(NumElements <= Elements_Values.Num());
			ensure(NumElements <= Elements_NextIndices.Num());

			QueryX.AddUninitialized(NumElements);
			QueryY.AddUninitialized(NumElements);
			QueryZ.AddUninitialized(NumElements);

			FVoxelBufferIterator Iterator;
			Iterator.Initialize(NumQueries + NumElements, NumQueries);
			for (; Iterator; ++Iterator)
			{
				checkVoxelSlow(Elements_Values.IsValidIndex(Iterator.GetIndex() - NumQueries + Iterator.Num() - 1));

				ispc::GenerateSurfacePointsBuilder_ComputeFinalDistances_WriteIndices(
					&Elements_Values[Iterator.GetIndex() - NumQueries],
					QueryX.GetData(Iterator),
					QueryY.GetData(Iterator),
					QueryZ.GetData(Iterator),
					Offset.X,
					Offset.Y,
					Offset.Z,
					BaseCellSize,
					Iterator.Num());
			}

			NumQueries += NumElements;
		}
	}

	const TSharedRef<FVoxelQueryParameters> Parameters = BaseQuery.CloneParameters();
	Parameters->Add<FVoxelGradientStepQueryParameter>().Step = BaseCellSize;
	Parameters->Add<FVoxelPositionQueryParameter>().Initialize(FVoxelVectorBuffer::Make(QueryX, QueryY, QueryZ));

	const TVoxelFutureValue<FVoxelFloatBuffer> SparseDistances = Surface->GetDistance(BaseQuery.MakeNewQuery(Parameters));

	MakeVoxelTask()
	.Dependency(SparseDistances)
	.Execute(MakeWeakPtrLambda(this, [=, ValueIndices = MoveTemp(ValueIndices)]
	{
		ProcessFinalDistances(
			NumQueries,
			ValueIndices,
			SparseDistances.Get_CheckCompleted().GetStorage());
	}));
}

void FVoxelGenerateSurfacePointsBuilder::ProcessFinalDistances(
	const int32 NumQueries,
	const TVoxelChunkedArray<TVoxelStaticArray<int32, 8>>& ValueIndices,
	const FVoxelFloatBufferStorage& SparseDistances)
{
	VOXEL_SCOPE_COUNTER_FORMAT("FVoxelGenerateSurfacePointsBuilder::ProcessFinalDistances Depth=%d NumCells=%d", Depth, Cells.Num());
	FVoxelNodeStatScope StatScope(Node, 0);

	ensure(Depth == 0);

	if (SparseDistances.IsConstant() ||
		!ensure(SparseDistances.Num() == NumQueries))
	{
		Finalize();
		return;
	}

	for (int32 CellIndex = 0; CellIndex < Cells.Num(); CellIndex++)
	{
		const TVoxelStaticArray<int32, 8>& CellIndices = ValueIndices[CellIndex];
		const float Distance0 = SparseDistances.LoadFast(CellIndices[0]);
		const float Distance1 = SparseDistances.LoadFast(CellIndices[1]);
		const float Distance2 = SparseDistances.LoadFast(CellIndices[2]);
		const float Distance3 = SparseDistances.LoadFast(CellIndices[3]);
		const float Distance4 = SparseDistances.LoadFast(CellIndices[4]);
		const float Distance5 = SparseDistances.LoadFast(CellIndices[5]);
		const float Distance6 = SparseDistances.LoadFast(CellIndices[6]);
		const float Distance7 = SparseDistances.LoadFast(CellIndices[7]);

		if (Distance0 >= 0)
		{
			if (Distance1 >= 0 &&
				Distance2 >= 0 &&
				Distance3 >= 0 &&
				Distance4 >= 0 &&
				Distance5 >= 0 &&
				Distance6 >= 0 &&
				Distance7 >= 0)
			{
				continue;
			}
		}
		else
		{
			if (Distance1 < 0 &&
				Distance2 < 0 &&
				Distance3 < 0 &&
				Distance4 < 0 &&
				Distance5 < 0 &&
				Distance6 < 0 &&
				Distance7 < 0)
			{
				continue;
			}
		}

		int32 CellCode =
			((Distance0 >= 0) << 0) |
			((Distance1 >= 0) << 1) |
			((Distance2 >= 0) << 2) |
			((Distance3 >= 0) << 3) |
			((Distance4 >= 0) << 4) |
			((Distance5 >= 0) << 5) |
			((Distance6 >= 0) << 6) |
			((Distance7 >= 0) << 7);

		CellCode = ~CellCode & 0xFF;

		if (CellCode == 0 ||
			CellCode == 255)
		{
			continue;
		}

		const FCell Cell = Cells[CellIndex];
		const FVector CellRelativeOffset = Offset + FVector(Cell.Vector()) * BaseCellSize;

		const Voxel::Transvoxel::FCellVertices CellVertices = Voxel::Transvoxel::CellCodeToCellVertices[CellCode];
		checkVoxelSlow(CellVertices.NumVertices() > 0);

		FVector RelativePositionSum = FVector(ForceInit);
		for (int32 CellVertexIndex = 0; CellVertexIndex < CellVertices.NumVertices(); CellVertexIndex++)
		{
			const Voxel::Transvoxel::FVertexData VertexData = CellVertices.GetVertexData(CellVertexIndex);

			// A: low point / B: high point
			const int32 IndexA = VertexData.IndexA;
			const int32 IndexB = VertexData.IndexB;
			checkVoxelSlow(0 <= IndexA && IndexA < 8);
			checkVoxelSlow(0 <= IndexB && IndexB < 8);

			const float DistanceA = SparseDistances.LoadFast(CellIndices[IndexA]);
			const float DistanceB = SparseDistances.LoadFast(CellIndices[IndexB]);
			ensureVoxelSlow((DistanceA >= 0) != (DistanceB >= 0));

			const int32 EdgeIndex = VertexData.EdgeIndex;
			checkVoxelSlow(0 <= EdgeIndex && EdgeIndex < 3);

			FVector Position = FVector(
				bool(IndexA & 1),
				bool(IndexA & 2),
				bool(IndexA & 4));
			Position[EdgeIndex] += DistanceA / (DistanceA - DistanceB);
			RelativePositionSum += Position;
		}

		RelativePositionSum /= CellVertices.NumVertices();
		const FVector Position = CellRelativeOffset + RelativePositionSum * BaseCellSize;

		// Extend otherwise plane at Z=0 is broken
		if (!Bounds.Extend(KINDA_SMALL_NUMBER).Contains(Position))
		{
			continue;
		}

		const FVector Alpha = RelativePositionSum;
		ensureVoxelSlow(-KINDA_SMALL_NUMBER < Alpha.X && Alpha.X < 1.f + KINDA_SMALL_NUMBER);
		ensureVoxelSlow(-KINDA_SMALL_NUMBER < Alpha.Y && Alpha.Y < 1.f + KINDA_SMALL_NUMBER);
		ensureVoxelSlow(-KINDA_SMALL_NUMBER < Alpha.Z && Alpha.Z < 1.f + KINDA_SMALL_NUMBER);

		const float MinX = FVoxelUtilities::BilinearInterpolation(
			SparseDistances.LoadFast(CellIndices[0b000]),
			SparseDistances.LoadFast(CellIndices[0b010]),
			SparseDistances.LoadFast(CellIndices[0b100]),
			SparseDistances.LoadFast(CellIndices[0b110]),
			Alpha.Y,
			Alpha.Z);

		const float MaxX = FVoxelUtilities::BilinearInterpolation(
			SparseDistances.LoadFast(CellIndices[0b001]),
			SparseDistances.LoadFast(CellIndices[0b011]),
			SparseDistances.LoadFast(CellIndices[0b101]),
			SparseDistances.LoadFast(CellIndices[0b111]),
			Alpha.Y,
			Alpha.Z);

		const float MinY = FVoxelUtilities::BilinearInterpolation(
			SparseDistances.LoadFast(CellIndices[0b000]),
			SparseDistances.LoadFast(CellIndices[0b001]),
			SparseDistances.LoadFast(CellIndices[0b100]),
			SparseDistances.LoadFast(CellIndices[0b101]),
			Alpha.X,
			Alpha.Z);

		const float MaxY = FVoxelUtilities::BilinearInterpolation(
			SparseDistances.LoadFast(CellIndices[0b010]),
			SparseDistances.LoadFast(CellIndices[0b011]),
			SparseDistances.LoadFast(CellIndices[0b110]),
			SparseDistances.LoadFast(CellIndices[0b111]),
			Alpha.X,
			Alpha.Z);

		const float MinZ = FVoxelUtilities::BilinearInterpolation(
			SparseDistances.LoadFast(CellIndices[0b000]),
			SparseDistances.LoadFast(CellIndices[0b001]),
			SparseDistances.LoadFast(CellIndices[0b010]),
			SparseDistances.LoadFast(CellIndices[0b011]),
			Alpha.X,
			Alpha.Y);

		const float MaxZ = FVoxelUtilities::BilinearInterpolation(
			SparseDistances.LoadFast(CellIndices[0b100]),
			SparseDistances.LoadFast(CellIndices[0b101]),
			SparseDistances.LoadFast(CellIndices[0b110]),
			SparseDistances.LoadFast(CellIndices[0b111]),
			Alpha.X,
			Alpha.Y);

		const FVector3f Normal = FVector3f(
			MaxX - MinX,
			MaxY - MinY,
			MaxZ - MinZ).GetSafeNormal();

		Id.Add(FVoxelUtilities::MurmurHashMulti(
			Cell.X,
			Cell.Y,
			Cell.Z,
			FVoxelUtilities::RoundToInt32(Offset.X),
			FVoxelUtilities::RoundToInt32(Offset.Y),
			FVoxelUtilities::RoundToInt32(Offset.Z)));

		PositionX.Add(Position.X);
		PositionY.Add(Position.Y);
		PositionZ.Add(Position.Z);

		NormalX.Add(Normal.X);
		NormalY.Add(Normal.Y);
		NormalZ.Add(Normal.Z);
	}

	Finalize();
}

void FVoxelGenerateSurfacePointsBuilder::Finalize()
{
	FVoxelNodeStatScope StatScope(Node, Id.Num());
	Dummy.MarkDummyAsCompleted();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_GenerateSurfacePoints, Out)
{
	const TValue<FVoxelBox> Bounds = Get(BoundsPin, Query);
	const TValue<FVoxelSurface> Surface = Get(SurfacePin, Query);
	const TValue<float> CellSize = Get(CellSizePin, Query);
	const TValue<float> MaxResolution = Get(MaxResolutionPin, Query);
	const TValue<float> DistanceChecksTolerance = Get(DistanceChecksTolerancePin, Query);

	return VOXEL_ON_COMPLETE(Bounds, Surface, CellSize, MaxResolution, DistanceChecksTolerance)
	{
		if (!Bounds.IsValid() ||
			Bounds == FVoxelBox())
		{
			VOXEL_MESSAGE(Error, "{0}: Invalid bounds", this);
			return {};
		}
		if (Bounds.IsInfinite())
		{
			VOXEL_MESSAGE(Error, "{0}: Infinite bounds", this);
			return {};
		}

		const FVoxelDummyFutureValue Dummy = FVoxelFutureValue::MakeDummy();
		const TSharedRef<FVoxelGenerateSurfacePointsBuilder> Builder = MakeVoxelShared<FVoxelGenerateSurfacePointsBuilder>(
			*this,
			Dummy,
			Query,
			Surface,
			FMath::Max(CellSize, 1.f),
			FMath::Max(MaxResolution, 1.f),
			FMath::Max(DistanceChecksTolerance, 0.f),
			Bounds);

		Builder->Compute();

		return VOXEL_ON_COMPLETE(Builder, Dummy)
		{
			const TSharedRef<FVoxelPointSet> PointSet = MakeVoxelShared<FVoxelPointSet>();
			if (Builder->Id.Num() == 0)
			{
				return {};
			}

			PointSet->SetNum(Builder->Id.Num());

			PointSet->Add(FVoxelPointAttributes::Id,
				FVoxelPointIdBuffer::Make(Builder->Id));

			PointSet->Add(FVoxelPointAttributes::Position,
				FVoxelVectorBuffer::Make(
					Builder->PositionX,
					Builder->PositionY,
					Builder->PositionZ));

			PointSet->Add(FVoxelPointAttributes::Normal,
				FVoxelVectorBuffer::Make(
					Builder->NormalX,
					Builder->NormalY,
					Builder->NormalZ));
			return PointSet;
		};
	};
}