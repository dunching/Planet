// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelGradientNodes.h"
#include "VoxelPositionQueryParameter.h"
#include "VoxelGradientNodesImpl.ispc.generated.h"

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_GetGradient, Gradient)
{
	FindVoxelQueryParameter(FVoxelPositionQueryParameter, PositionQueryParameter);
	FindVoxelQueryParameter(FVoxelGradientStepQueryParameter, GradientStepQueryParameter);

	if (PositionQueryParameter->IsGradient())
	{
		const TValue<FVoxelFloatBuffer> Value = Get(ValuePin, Query);
		const FVoxelVectorBuffer QueryPositions = PositionQueryParameter->GetPositions();

		return VOXEL_ON_COMPLETE(GradientStepQueryParameter, Value, QueryPositions)
		{
			if (Value.IsConstant())
			{
				return FVector::ZeroVector;
			}

			CheckVoxelBuffersNum(Value, QueryPositions);

			FVoxelFloatBufferStorage X; X.Allocate(Value.Num());
			FVoxelFloatBufferStorage Y; Y.Allocate(Value.Num());
			FVoxelFloatBufferStorage Z; Z.Allocate(Value.Num());

			VOXEL_SCOPE_COUNTER("VoxelNode_GetGradient_Sparse");

			ForeachVoxelBufferChunk(Value.Num(), [&](const FVoxelBufferIterator& Iterator)
			{
				ensure(
					Iterator.Num() == FVoxelBufferDefinitions::NumPerChunk ||
					Iterator.Num() % 24 == 0);
				const int32 Num = 24 * FMath::DivideAndRoundDown(Iterator.Num(), 24);

				ispc::VoxelNode_GetGradient_Sparse(
					Value.GetData(Iterator),
					Num,
					GradientStepQueryParameter->Step,
					X.GetData(Iterator),
					Y.GetData(Iterator),
					Z.GetData(Iterator));
			});

			return FVoxelVectorBuffer::Make(X, Y, Z);
		};
	}

	return VOXEL_ON_COMPLETE(PositionQueryParameter, GradientStepQueryParameter)
	{
		const FVoxelVectorBuffer QueryPositions = PositionQueryParameter->GetPositions();
		if (!ensure(QueryPositions.Num() == QueryPositions.X.Num()) ||
			!ensure(QueryPositions.Num() == QueryPositions.Y.Num()) ||
			!ensure(QueryPositions.Num() == QueryPositions.Z.Num()))
		{
			return {};
		}

		// Copy sane positions to padding to ensure Bounds are valid
		QueryPositions.X.GetStorage().FixupAlignmentPaddingData();
		QueryPositions.Y.GetStorage().FixupAlignmentPaddingData();
		QueryPositions.Z.GetStorage().FixupAlignmentPaddingData();

		const int32 InputNum = QueryPositions.Num();
		const int32 AlignedInputNum = 4 * FMath::DivideAndRoundUp(InputNum, 4);
		ensure(AlignedInputNum % 4 == 0);

		const int32 GradientNumNoPadding = AlignedInputNum * 6;
		ensure(GradientNumNoPadding % 24 == 0);

		constexpr int32 GradientChunkPadding = FVoxelBufferDefinitions::NumPerChunk % 24;
		constexpr int32 GradientNumPerChunks = FVoxelBufferDefinitions::NumPerChunk - GradientChunkPadding;
		const int32 NumGradientChunks = FMath::DivideAndRoundUp(GradientNumNoPadding, GradientNumPerChunks);
		ensure(NumGradientChunks >= 1);

		// -1: last chunk has no padding
		const int32 GradientNum = GradientNumNoPadding + (NumGradientChunks - 1) * GradientChunkPadding;

		const TSharedRef<FVoxelPositionQueryParameter> NewPositionQueryParameter = MakeVoxelShared<FVoxelPositionQueryParameter>();
		{
			VOXEL_SCOPE_COUNTER("VoxelNode_GetGradient_Sparse_SplitPositions");
			const float HalfStep = GradientStepQueryParameter->Step / 2.f;

			FVoxelFloatBufferStorage X; X.Allocate(GradientNum);
			FVoxelFloatBufferStorage Y; Y.Allocate(GradientNum);
			FVoxelFloatBufferStorage Z; Z.Allocate(GradientNum);

			int32 InputIndex = 0;
			int32 GradientIndex = 0;
			while (
				InputIndex < AlignedInputNum &&
				GradientIndex < GradientNum)
			{
				FVoxelBufferIterator InputIterator;
				InputIterator.Initialize(AlignedInputNum, InputIndex);

				FVoxelBufferIterator GradientIterator;
				GradientIterator.Initialize(GradientNum, GradientIndex);

				// NumPerChunk is not a multiple of 24, pad the end with nans
				if (GradientIterator.NumToProcess < 24)
				{
					ensure(GradientIterator.NumToProcess == GradientChunkPadding);
					ensure((GradientIndex + GradientChunkPadding) % FVoxelBufferDefinitions::NumPerChunk == 0);

					for (int32 Index = 0; Index < GradientChunkPadding; Index++)
					{
						X[GradientIndex + Index] = X[Index];
						Y[GradientIndex + Index] = Y[Index];
						Z[GradientIndex + Index] = Z[Index];
					}

					GradientIndex += GradientChunkPadding;
					continue;
				}

				GradientIterator.NumToProcess = 24 * FMath::DivideAndRoundDown(GradientIterator.NumToProcess, 24);

				const int32 Step = FMath::Min(InputIterator.NumToProcess * 6, GradientIterator.NumToProcess);
				ensure(Step % 24 == 0);

				InputIterator.NumToProcess = Step / 6;
				GradientIterator.NumToProcess = Step;

				ispc::VoxelNode_GetGradient_Sparse_SplitPositions(
					QueryPositions.X.GetData(InputIterator),
					QueryPositions.Y.GetData(InputIterator),
					QueryPositions.Z.GetData(InputIterator),
					InputIterator.Num(),
					HalfStep,
					X.GetData(GradientIterator),
					Y.GetData(GradientIterator),
					Z.GetData(GradientIterator));

				InputIndex += InputIterator.NumToProcess;
				GradientIndex += GradientIterator.NumToProcess;
			}
			ensure(InputIndex == AlignedInputNum);
			ensure(GradientIndex == GradientNum);

			NewPositionQueryParameter->InitializeGradient(
				FVoxelVectorBuffer::Make(X, Y, Z),
				PositionQueryParameter->GetBounds().Extend(HalfStep));
		}

		const TSharedRef<FVoxelQueryParameters> Parameters = Query.CloneParameters();
		Parameters->Add(NewPositionQueryParameter);
		const TValue<FVoxelFloatBuffer> Value = Get(ValuePin, Query.MakeNewQuery(Parameters));

		return VOXEL_ON_COMPLETE(GradientStepQueryParameter, InputNum, AlignedInputNum, GradientChunkPadding, GradientNum, NewPositionQueryParameter, Value)
		{
			if (Value.IsConstant())
			{
				return FVector::ZeroVector;
			}
			CheckVoxelBuffersNum(Value, NewPositionQueryParameter->GetPositions());

			FVoxelFloatBufferStorage X; X.Allocate(InputNum);
			FVoxelFloatBufferStorage Y; Y.Allocate(InputNum);
			FVoxelFloatBufferStorage Z; Z.Allocate(InputNum);

			{
				VOXEL_SCOPE_COUNTER("VoxelNode_GetGradient_Sparse_Collapse");

				int32 InputIndex = 0;
				int32 GradientIndex = 0;
				while (
					InputIndex < AlignedInputNum &&
					GradientIndex < GradientNum)
				{
					FVoxelBufferIterator InputIterator;
					InputIterator.Initialize(AlignedInputNum, InputIndex);

					FVoxelBufferIterator GradientIterator;
					GradientIterator.Initialize(GradientNum, GradientIndex);

					// NumPerChunk is not a multiple of 24, end is padded with nan
					if (GradientIterator.NumToProcess < 24)
					{
						ensure(GradientIterator.NumToProcess == GradientChunkPadding);
						ensure((GradientIndex + GradientChunkPadding) % FVoxelBufferDefinitions::NumPerChunk == 0);

						for (int32 Index = 0; Index < GradientChunkPadding; Index++)
						{
							ensure(Value[GradientIndex + Index] == Value[Index]);
						}

						GradientIndex += GradientChunkPadding;
						continue;
					}

					GradientIterator.NumToProcess = 24 * FMath::DivideAndRoundDown(GradientIterator.NumToProcess, 24);

					const int32 Step = FMath::Min(InputIterator.NumToProcess * 6, GradientIterator.NumToProcess);
					ensure(Step % 24 == 0);

					InputIterator.NumToProcess = Step / 6;
					GradientIterator.NumToProcess = Step;

					ispc::VoxelNode_GetGradient_Sparse_Collapse(
						Value.GetData(GradientIterator),
						GradientIterator.Num(),
						GradientStepQueryParameter->Step,
						X.GetData(InputIterator),
						Y.GetData(InputIterator),
						Z.GetData(InputIterator));

					InputIndex += InputIterator.NumToProcess;
					GradientIndex += GradientIterator.NumToProcess;
				}
				ensure(InputIndex == AlignedInputNum);
				ensure(GradientIndex == GradientNum);
			}

			return FVoxelVectorBuffer::Make(X, Y, Z);
		};
	};
}