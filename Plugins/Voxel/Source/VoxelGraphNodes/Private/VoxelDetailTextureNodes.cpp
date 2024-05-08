// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelDetailTextureNodes.h"
#include "VoxelTriangleTracer.h"
#include "VoxelPositionQueryParameter.h"
#include "MarchingCube/VoxelMarchingCubeNodes.h"
#include "VoxelDetailTextureNodesImpl.ispc.generated.h"
#include "Engine/Texture2D.h"

struct FVoxelCreateDetailTextureHelper
{
	template<EVoxelAxis Axis>
	static void Compute(
		const FVector3f& CellPosition,
		const float VoxelSize,
		const int32 TextureSize,
		const FVector3f BoundsMin,
		const TConstVoxelArrayView<FVoxelTriangleTracer> Triangles,
		int32& QueryIndex,
		FVoxelFloatBufferStorage& QueryX,
		FVoxelFloatBufferStorage& QueryY,
		FVoxelFloatBufferStorage& QueryZ)
	{
		const float InvPixelSize = TextureSize - 2;
		const float PixelSize = 1 / double(InvPixelSize);

		for (int32 Y = 0; Y < TextureSize; Y++)
		{
			for (int32 X = 0; X < TextureSize; X++)
			{
				FVector3f TextureOffset;
				switch (Axis)
				{
				default: VOXEL_ASSUME(false);
				case EVoxelAxis::X: TextureOffset = FVector3f(0, X, Y); break;
				case EVoxelAxis::Y: TextureOffset = FVector3f(X, 0, Y); break;
				case EVoxelAxis::Z: TextureOffset = FVector3f(X, Y, 0); break;
				}

				// TextureOffset is between [0, TextureSize - 1]
				// First pixel will be -0.5, last one will be -0.5 + TextureSize - 1
				FVector3f Offset = (FVector3f(-0.5f) + TextureOffset) * PixelSize;

				Offset[int32(Axis)] = 0;

				const FVector3f RayOrigin = CellPosition + Offset;

				FVector3f HitPosition = RayOrigin;

				for (const FVoxelTriangleTracer& Triangle : Triangles)
				{
					float Time;
					if (!Triangle.TraceAxis<Axis>(RayOrigin, true, Time))
					{
						continue;
					}

					HitPosition[int32(Axis)] += Time;
					break;
				}

				// Reduce precision errors
				HitPosition *= InvPixelSize * 10.f;
				HitPosition = FVoxelUtilities::RoundToFloat(HitPosition);
				HitPosition *= PixelSize * 0.1f;

				HitPosition *= VoxelSize;
				HitPosition += BoundsMin;

				QueryX[QueryIndex] = HitPosition.X;
				QueryY[QueryIndex] = HitPosition.Y;
				QueryZ[QueryIndex] = HitPosition.Z;
				QueryIndex++;
			}
		}
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelDetailTextureQueryHelperImpl::SetupQueryParameters(FVoxelQueryParameters& Parameters) const
{
	Parameters.Add<FVoxelPositionQueryParameter>().Initialize(
		QueryPositions,
		// We trace against triangles of size 10
		Surface->ChunkBounds.Extend(10 * /*UE_SQRT_3*/ 2.f * Surface->ScaledVoxelSize));
	Parameters.Add<FVoxelGradientStepQueryParameter>().Step = Surface->ScaledVoxelSize / TextureSize;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int32 FVoxelDetailTextureQueryHelper::NumCells() const
{
	return Surface->Cells.Num();
}

TVoxelFutureValue<FVoxelDetailTextureQueryHelperImpl> FVoxelDetailTextureQueryHelper::GetImpl(const int32 TextureSize)
{
	VOXEL_SCOPE_LOCK(CriticalSection);

	TVoxelFutureValue<FVoxelDetailTextureQueryHelperImpl>& Impl = TextureSizeToImpl.FindOrAdd(TextureSize);
	if (!Impl.IsValid())
	{
		Impl =
			MakeVoxelTask(STATIC_FNAME("ComputeQueryPositions"))
			.Execute<FVoxelDetailTextureQueryHelperImpl>([=]
			{
				FVoxelDetailTextureQueryHelperImpl Result;
				Result.TextureSize = TextureSize;
				Result.QueryPositions = ComputeQueryPositions(TextureSize);
				Result.Surface = Surface;
				return Result;
			});
	}
	return Impl;
}

int32 FVoxelDetailTextureQueryHelper::AddCellCoordinates(TVoxelArray<FVoxelDetailTextureCoordinate>&& CellCoordinates)
{
	VOXEL_SCOPE_LOCK(CriticalSection);
	return AllCellCoordinates.Add(MoveTemp(CellCoordinates));
}

TVoxelArray<uint8> FVoxelDetailTextureQueryHelper::BuildCellIndexToDirection() const
{
	VOXEL_FUNCTION_COUNTER();

	TVoxelArray<uint8> CellIndexToDirection;
	CellIndexToDirection.Reserve(NumCells());
	for (const FVoxelMarchingCubeCell& Cell : Surface->Cells)
	{
		FVector3f Normal = FVector3f(ForceInit);
		for (int32 TriangleIndex = 0; TriangleIndex < Cell.NumTriangles; TriangleIndex++)
		{
			const FVector3f PositionA = Surface->Vertices[Surface->Indices[3 * (Cell.FirstTriangle + TriangleIndex) + 0]];
			const FVector3f PositionB = Surface->Vertices[Surface->Indices[3 * (Cell.FirstTriangle + TriangleIndex) + 1]];
			const FVector3f PositionC = Surface->Vertices[Surface->Indices[3 * (Cell.FirstTriangle + TriangleIndex) + 2]];

			Normal += FVoxelUtilities::GetTriangleNormal(PositionA, PositionB, PositionC);
		}
		Normal.Normalize();

		CellIndexToDirection.Add(FVoxelUtilities::GetLargestAxis(Normal.GetAbs()));
	}
	return CellIndexToDirection;
}

TVoxelArray<FVoxelDetailTextureCoordinate> FVoxelDetailTextureQueryHelper::BuildCellCoordinates() const
{
	VOXEL_FUNCTION_COUNTER();

	// We should be done computing detail textures already
	checkVoxelSlow(!CriticalSection.IsLocked());

	VOXEL_SCOPE_LOCK(CriticalSection);

	TVoxelArray<FVoxelDetailTextureCoordinate> Result;
	FVoxelUtilities::SetNumFast(Result, NumCells() * AllCellCoordinates.Num());

	for (int32 Index = 0; Index < AllCellCoordinates.Num(); Index++)
	{
		FVoxelUtilities::Memcpy(
			MakeVoxelArrayView(Result).Slice(Index * NumCells(), NumCells()),
			AllCellCoordinates[Index]);
	}

	return Result;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelVectorBuffer FVoxelDetailTextureQueryHelper::ComputeQueryPositions(const int32 TextureSize) const
{
	VOXEL_FUNCTION_COUNTER();
	ensure(4 <= TextureSize && TextureSize <= 128);

	const float VoxelSize = Surface->ScaledVoxelSize;
	const int32 NumCells = Surface->Cells.Num();

	FVoxelFloatBufferStorage QueryX; QueryX.Allocate(TextureSize * TextureSize * NumCells);
	FVoxelFloatBufferStorage QueryY; QueryY.Allocate(TextureSize * TextureSize * NumCells);
	FVoxelFloatBufferStorage QueryZ; QueryZ.Allocate(TextureSize * TextureSize * NumCells);
	int32 QueryIndex = 0;

	for (const FVoxelMarchingCubeCell& Cell : Surface->Cells)
	{
		const FIntVector CellPosition = FIntVector(Cell.X, Cell.Y, Cell.Z);

		constexpr int32 MaxTrianglesPerCell = 5;
		checkVoxelSlow(Cell.NumTriangles <= MaxTrianglesPerCell);

		TVoxelArray<FVoxelTriangleTracer, TFixedAllocator<MaxTrianglesPerCell * 2>> Triangles;

		FVector3f Normal = FVector3f::ZeroVector;
		for (int32 TriangleIndex = 0; TriangleIndex < Cell.NumTriangles; TriangleIndex++)
		{
			const FVector3f PositionA = Surface->Vertices[Surface->Indices[3 * (Cell.FirstTriangle + TriangleIndex) + 0]];
			const FVector3f PositionB = Surface->Vertices[Surface->Indices[3 * (Cell.FirstTriangle + TriangleIndex) + 1]];
			const FVector3f PositionC = Surface->Vertices[Surface->Indices[3 * (Cell.FirstTriangle + TriangleIndex) + 2]];

			Normal += FVoxelUtilities::GetTriangleNormal(PositionA, PositionB, PositionC);

			Triangles.Add(FVoxelTriangleTracer(PositionA, PositionB, PositionC));

			// Add bigger triangles to ensure all rays hit
			const FVector Centroid = FVector(PositionA + PositionB + PositionC) / 3.f;
			Triangles.Add(FVoxelTriangleTracer(
				FVector3f(Centroid + 10 * (FVector(PositionA) - Centroid).GetSafeNormal()),
				FVector3f(Centroid + 10 * (FVector(PositionB) - Centroid).GetSafeNormal()),
				FVector3f(Centroid + 10 * (FVector(PositionC) - Centroid).GetSafeNormal())));
		}
		Normal.Normalize();

		switch (FVoxelUtilities::GetLargestAxis(Normal.GetAbs()))
		{
		default: check(false);
		case 0:
		{
			FVoxelCreateDetailTextureHelper::Compute<EVoxelAxis::X>(
				FVector3f(CellPosition),
				VoxelSize,
				TextureSize,
				FVector3f(Surface->ChunkBounds.Min),
				Triangles,
				QueryIndex,
				QueryX,
				QueryY,
				QueryZ);
		}
		break;
		case 1:
		{
			FVoxelCreateDetailTextureHelper::Compute<EVoxelAxis::Y>(
				FVector3f(CellPosition),
				VoxelSize,
				TextureSize,
				FVector3f(Surface->ChunkBounds.Min),
				Triangles,
				QueryIndex,
				QueryX,
				QueryY,
				QueryZ);
		}
		break;
		case 2:
		{
			FVoxelCreateDetailTextureHelper::Compute<EVoxelAxis::Z>(
				FVector3f(CellPosition),
				VoxelSize,
				TextureSize,
				FVector3f(Surface->ChunkBounds.Min),
				Triangles,
				QueryIndex,
				QueryX,
				QueryY,
				QueryZ);
		}
		break;
		}
	}
	ensure(QueryIndex == TextureSize * TextureSize * NumCells);

	return FVoxelVectorBuffer::Make(QueryX, QueryY, QueryZ);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TVoxelFutureValue<FVoxelComputedMaterialParameter> FVoxelDetailTextureParameter::Compute(const FVoxelQuery& Query) const
{
	FindVoxelQueryParameter(FVoxelLODQueryParameter, LODQueryParameter);
	FindVoxelQueryParameter(FVoxelDetailTextureQueryParameter, DetailTextureQueryParameter);

	if (!ensure(DetailTextureQueryParameter->Helper))
	{
		return {};
	}

	if (!Pool.IsValid())
	{
		VOXEL_MESSAGE(Error, "{0}: Invalid texture", this);
		return {};
	}

	const int32 LOD = LODQueryParameter->LOD;
	const int32 TextureSize = Pool->GetTextureSize_AnyThread(LOD, Query);
	const TSharedRef<FVoxelDetailTextureQueryHelper> Helper = DetailTextureQueryParameter->Helper.ToSharedRef();

	const TVoxelFutureValue<FVoxelDetailTextureQueryHelperImpl> HelperImpl = Helper->GetImpl(TextureSize);

	return VOXEL_ON_COMPLETE(TextureSize, Helper, HelperImpl)
	{
		const TSharedRef<FVoxelQueryParameters> Parameters = Query.CloneParameters();
		HelperImpl->SetupQueryParameters(*Parameters);

		const TVoxelFutureValue<FVoxelBuffer> Buffer = (*GetBuffer)(Query.MakeNewQuery(Parameters));

		return VOXEL_ON_COMPLETE(TextureSize, Helper, Buffer)
		{
			const TSharedRef<FVoxelDetailTextureAllocation> Allocation = Pool->Allocate_AnyThread(TextureSize, Helper->NumCells(), Query);

			TVoxelArray<TSharedRef<FVoxelDetailTextureUpload>> Uploads;
			for (int32 TextureIndex = 0; TextureIndex < Allocation->NumTextures; TextureIndex++)
			{
				Uploads.Add(MakeVoxelShared<FVoxelDetailTextureUpload>(*Allocation, TextureIndex));
			}

			TVoxelArray<FVoxelDetailTextureCoordinate> CellCoordinates0;
			for (int32 TextureIndex = 0; TextureIndex < Allocation->NumTextures; TextureIndex++)
			{
				TVoxelArray<FVoxelDetailTextureCoordinate> CellCoordinates;
				CellCoordinates.Reserve(Helper->NumCells());

				int32 Index = 0;

				FVoxelDetailTextureCoordinate Coordinate;
				TVoxelArrayView<uint8> Data;
				int32 Pitch = 0;
				while (Uploads[TextureIndex]->GetUploadInfo(Coordinate, Data, Pitch))
				{
					ComputeCell(
						Index,
						Pitch,
						TextureSize,
						Allocation->PixelFormat,
						TextureIndex,
						*Buffer,
						Data);

					CellCoordinates.Add(Coordinate);
					Index += TextureSize * TextureSize;
				}
				ensure(Buffer->IsConstant() || Buffer->Num() == Index);
				ensure(CellCoordinates.Num() == Helper->NumCells());

				if (TextureIndex == 0)
				{
					CellCoordinates0 = MoveTemp(CellCoordinates);
				}
				else
				{
					checkVoxelSlow(ReinterpretCastVoxelArray<uint32>(CellCoordinates0) == ReinterpretCastVoxelArray<uint32>(CellCoordinates));
				}
			}

			const int32 CellCoordinatesTextureIndex = Helper->AddCellCoordinates(MoveTemp(CellCoordinates0));

			TVoxelArray<FVoxelDummyFutureValue> UploadDummies;
			for (const TSharedRef<FVoxelDetailTextureUpload>& Upload : Uploads)
			{
				UploadDummies.Add(Upload->Upload());
			}

			return VOXEL_ON_COMPLETE(TextureSize, Allocation, CellCoordinatesTextureIndex, UploadDummies)
			{
				FVoxelComputedMaterialParameter MaterialParameters;
				MaterialParameters.ScalarParameters.Add(Pool->Guid + "_TextureIndex", CellCoordinatesTextureIndex);
				MaterialParameters.ScalarParameters.Add(Pool->Guid + "_TextureSize", TextureSize);
				MaterialParameters.DynamicParameters.Add(Pool->Guid, Allocation->GetTexture());

				if (NameOverride.IsSet())
				{
					MaterialParameters.ScalarParameters.Add(NameOverride.GetValue() + "_TextureIndex", CellCoordinatesTextureIndex);
					MaterialParameters.ScalarParameters.Add(NameOverride.GetValue() + "_TextureSize", TextureSize);
					MaterialParameters.DynamicParameters.Add(NameOverride.GetValue(), Allocation->GetTexture());
				}

				MaterialParameters.Resources.Add(Allocation);
				return MaterialParameters;
			};
		};
	};
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelNormalDetailTextureParameter::ComputeCell(
	int32 Index,
	const int32 Pitch,
	const int32 TextureSize,
	const EPixelFormat PixelFormat,
	const int32 TextureIndex,
	const FVoxelBuffer& Buffer,
	const TVoxelArrayView<uint8> Data) const
{
	const FVoxelVectorBuffer& Normals = CastChecked<FVoxelVectorBuffer>(Buffer);

	for (int32 Y = 0; Y < TextureSize; Y++)
	{
		for (int32 X = 0; X < TextureSize; X++)
		{
			const FVector3f Normal = Normals[Index++].GetSafeNormal(UE_SMALL_NUMBER, FVector3f::UpVector);
			const FVoxelOctahedron Octahedron(Normal);

			Data[Y * Pitch + X * 2 + 0] = Octahedron.X;
			Data[Y * Pitch + X * 2 + 1] = Octahedron.Y;
		}
	}
}

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_MakeNormalDetailTextureParameter, Parameter)
{
	const TValue<FVoxelNormalDetailTextureRef> Texture = Get(TexturePin, Query);

	return VOXEL_ON_COMPLETE(Texture)
	{
		FVoxelNormalDetailTextureParameter Parameter;
		if (bIsMain)
		{
			Parameter.NameOverride = STATIC_FNAME("Normal");
		}
		Parameter.Node = GetNodeRef();
		Parameter.Pool = Texture->WeakPool.Pin();
		Parameter.SetGetBuffer(GetCompute(NormalPin, Query.GetSharedContext()));
		return Parameter;
	};
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelFloatDetailTextureParameter::ComputeCell(
	int32 Index,
	const int32 Pitch,
	const int32 TextureSize,
	const EPixelFormat PixelFormat,
	const int32 TextureIndex,
	const FVoxelBuffer& Buffer,
	const TVoxelArrayView<uint8> Data) const
{
	const FVoxelFloatBuffer& Floats = CastChecked<FVoxelFloatBuffer>(Buffer);

	const int32 BlockBytes = GPixelFormats[PixelFormat].BlockBytes;
	if (BlockBytes == 1)
	{
		for (int32 Y = 0; Y < TextureSize; Y++)
		{
			for (int32 X = 0; X < TextureSize; X++)
			{
				const float Float = Floats[Index++];

				Data[Y * Pitch + X] = FVoxelUtilities::FloatToUINT8(Float);
			}
		}
	}
	else if (BlockBytes == 2)
	{
		for (int32 Y = 0; Y < TextureSize; Y++)
		{
			for (int32 X = 0; X < TextureSize; X++)
			{
				const float Float = Floats[Index++];
				const uint16 Value = FVoxelUtilities::FloatToUINT16(Float);

				Data[Y * Pitch + X * 2 + 0] = reinterpret_cast<const uint8*>(&Value)[0];
				Data[Y * Pitch + X * 2 + 1] = reinterpret_cast<const uint8*>(&Value)[1];
			}
		}
	}
	else
	{
		ensure(BlockBytes == 4);
		for (int32 Y = 0; Y < TextureSize; Y++)
		{
			for (int32 X = 0; X < TextureSize; X++)
			{
				const float Float = Floats[Index++];

				Data[Y * Pitch + X * 4 + 0] = reinterpret_cast<const uint8*>(&Float)[0];
				Data[Y * Pitch + X * 4 + 1] = reinterpret_cast<const uint8*>(&Float)[1];
				Data[Y * Pitch + X * 4 + 2] = reinterpret_cast<const uint8*>(&Float)[2];
				Data[Y * Pitch + X * 4 + 3] = reinterpret_cast<const uint8*>(&Float)[3];
			}
		}
	}
}

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_MakeFloatDetailTextureParameter, Parameter)
{
	const TValue<FVoxelFloatDetailTextureRef> Texture = Get(TexturePin, Query);

	return VOXEL_ON_COMPLETE(Texture)
	{
		FVoxelFloatDetailTextureParameter Parameter;
		Parameter.Node = GetNodeRef();
		Parameter.Pool = Texture->WeakPool.Pin();
		Parameter.SetGetBuffer(GetCompute(FloatPin, Query.GetSharedContext()));
		return Parameter;
	};
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelColorDetailTextureParameter::ComputeCell(
	int32 Index,
	const int32 Pitch,
	const int32 TextureSize,
	const EPixelFormat PixelFormat,
	const int32 TextureIndex,
	const FVoxelBuffer& Buffer,
	const TVoxelArrayView<uint8> Data) const
{
	const FVoxelLinearColorBuffer& Colors = CastChecked<FVoxelLinearColorBuffer>(Buffer);

	for (int32 Y = 0; Y < TextureSize; Y++)
	{
		for (int32 X = 0; X < TextureSize; X++)
		{
			const FColor Color = Colors[Index++].ToFColor(false);

			Data[Y * Pitch + X * 4 + 0] = Color.R;
			Data[Y * Pitch + X * 4 + 1] = Color.G;
			Data[Y * Pitch + X * 4 + 2] = Color.B;
			Data[Y * Pitch + X * 4 + 3] = Color.A;
		}
	}
}

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_MakeColorDetailTextureParameter, Parameter)
{
	const TValue<FVoxelColorDetailTextureRef> Texture = Get(TexturePin, Query);

	return VOXEL_ON_COMPLETE(Texture)
	{
		FVoxelColorDetailTextureParameter Parameter;
		Parameter.Node = GetNodeRef();
		Parameter.Pool = Texture->WeakPool.Pin();
		Parameter.SetGetBuffer(GetCompute(ColorPin, Query.GetSharedContext()));
		return Parameter;
	};
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelMaterialIdDetailTextureParameter::ComputeCell(
	int32 Index,
	const int32 Pitch,
	const int32 TextureSize,
	const EPixelFormat PixelFormat,
	const int32 TextureIndex,
	const FVoxelBuffer& Buffer,
	const TVoxelArrayView<uint8> Data) const
{
	const FVoxelInt64Buffer& PackedMaterials = CastChecked<FVoxelInt64Buffer>(Buffer);

	for (int32 Y = 0; Y < TextureSize; Y++)
	{
		for (int32 X = 0; X < TextureSize; X++)
		{
			const int64 PackedMaterial = PackedMaterials[Index++];
			checkVoxelSlow(TextureIndex == 0 || TextureIndex == 1);

			Data[Y * Pitch + X * 4 + 0] = reinterpret_cast<const uint8*>(&PackedMaterial)[4 * TextureIndex + 0];
			Data[Y * Pitch + X * 4 + 1] = reinterpret_cast<const uint8*>(&PackedMaterial)[4 * TextureIndex + 1];
			Data[Y * Pitch + X * 4 + 2] = reinterpret_cast<const uint8*>(&PackedMaterial)[4 * TextureIndex + 2];
			Data[Y * Pitch + X * 4 + 3] = reinterpret_cast<const uint8*>(&PackedMaterial)[4 * TextureIndex + 3];
		}
	}
}

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_MakeMaterialIdDetailTextureParameter, Parameter)
{
	const TValue<FVoxelMaterialIdDetailTextureRef> Texture = Get(TexturePin, Query);

	return VOXEL_ON_COMPLETE(Texture)
	{
		const TSharedRef<const TVoxelComputeValue<FVoxelSurface>> ComputeSurface = GetCompute(SurfacePin, Query.GetSharedContext());

		TVoxelComputeValue<FVoxelBuffer> Compute = [ComputeSurface](const FVoxelQuery& InQuery) -> TValue<FVoxelBuffer>
		{
			const TValue<FVoxelSurface> FutureSurface = (*ComputeSurface)(InQuery);

			return
				MakeVoxelTask()
				.Dependency(FutureSurface)
				.Execute<FVoxelInt64Buffer>([=]
				{
					const TValue<FVoxelSurfaceMaterial> FutureSurfaceMaterial = FutureSurface.Get_CheckCompleted().GetMaterial(InQuery);

					return
						MakeVoxelTask(STATIC_FNAME("SurfaceToMaterialId"))
						.Dependency(FutureSurfaceMaterial)
						.Execute<FVoxelInt64Buffer>([FutureSurfaceMaterial]() -> FVoxelInt64Buffer
						{
							const TSharedRef<const FVoxelSurfaceMaterial> SurfaceMaterial = FutureSurfaceMaterial.GetShared_CheckCompleted();
							if (SurfaceMaterial->Layers.Num() == 0 ||
								!ensure(SurfaceMaterial->IsValid_Slow()))
							{
								return {};
							}

							const int32 Num = SurfaceMaterial->Num();

							FVoxelInt64BufferStorage Result;
							Result.Allocate(Num);

							ForeachVoxelBufferChunk(Num, [&](const FVoxelBufferIterator& Iterator)
							{
								VOXEL_SCOPE_COUNTER_FORMAT("MakePackedMaterial Num=%d", Iterator.Num());

								TVoxelArray<ispc::FVoxelSurfaceLayer> Layers;
								SurfaceMaterial->GetLayers(Iterator, Layers);

								ispc::VoxelNode_MakeMaterialIdDetailTextureParameter_MakePackedMaterial(
									Layers.GetData(),
									Layers.Num(),
									Iterator.Num(),
									Result.GetData(Iterator));
							});

							return FVoxelInt64Buffer::Make(Result);
						});
				});
		};

		FVoxelMaterialIdDetailTextureParameter Parameter;
		if (bIsMain)
		{
			Parameter.NameOverride = STATIC_FNAME("MaterialId");
		}
		Parameter.Node = GetNodeRef();
		Parameter.Pool = Texture->WeakPool.Pin();
		Parameter.GetBuffer = MakeSharedCopy(MoveTemp(Compute));
		return Parameter;
	};
}