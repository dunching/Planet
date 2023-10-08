// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "MarchingCube/VoxelMarchingCubeNodes.h"
#include "MarchingCube/VoxelMarchingCubeMesh.h"
#include "MarchingCube/VoxelMarchingCubeProcessor.h"
#include "VoxelGradientNodes.h"
#include "VoxelDetailTextureNodes.h"
#include "VoxelDistanceFieldWrapper.h"
#include "VoxelPositionQueryParameter.h"
#include "Collision/VoxelCollisionCooker.h"
#include "Collision/VoxelTriangleMeshCollider.h"
// UE_ENABLE_INCLUDE_ORDER_DEPRECATED_IN_5_2=1 is broken for MeshCardBuild.h
#undef UE_ENABLE_INCLUDE_ORDER_DEPRECATED_IN_5_2
#define UE_ENABLE_INCLUDE_ORDER_DEPRECATED_IN_5_2 0
#include "MeshCardBuild.h"
#include "DistanceFieldAtlas.h"

VOXEL_CONSOLE_VARIABLE(
	VOXELGRAPHNODES_API, bool, GVoxelMarchingCubeShowSkippedChunks, false,
	"voxel.marchingcube.ShowSkippedChunks",
	"");

VOXEL_CONSOLE_VARIABLE(
	VOXELGRAPHNODES_API, bool, GVoxelMarchingCubeShowComputedChunks, false,
	"voxel.marchingcube.ShowComputedChunks",
	"");

VOXEL_CONSOLE_VARIABLE(
	VOXELGRAPHNODES_API, int32, GVoxelMarchingCubeNumDistanceFieldBricksPerChunk, 4,
	"voxel.marchingcube.NumDistanceFieldBricksPerChunk",
	"");

VOXEL_CONSOLE_VARIABLE(
	VOXELGRAPHNODES_API, bool, GVoxelMarchingCubeEnableDistanceFieldPadding, false,
	"voxel.marchingcube.EnableDistanceFieldPadding",
	"Add padding to perfectly overlap chunks distance fields. "
	"This might cause invalid entries into Lumen's surface cache and glitches in Lumen at chunk borders.");

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_GenerateMarchingCubeSurface, Surface)
{
	FindVoxelQueryParameter(FVoxelLODQueryParameter, LODQueryParameter);

	const int32 LOD = LODQueryParameter->LOD;

	const TValue<float> VoxelSize = Get(VoxelSizePin, Query);
	const TValue<int32> ChunkSize = Get(ChunkSizePin, Query);
	const TValue<FVoxelBox> Bounds = Get(BoundsPin, Query);
	const TValue<bool> EnableTransitions = Get(EnableTransitionsPin, Query);
	const TValue<bool> PerfectTransitions = Get(PerfectTransitionsPin, Query);
	const TValue<bool> EnableDistanceChecks = Get(EnableDistanceChecksPin, Query);
	const TValue<float> DistanceChecksTolerance = Get(DistanceChecksTolerancePin, Query);

	return VOXEL_ON_COMPLETE(LOD, VoxelSize, ChunkSize, Bounds, EnableTransitions, PerfectTransitions, EnableDistanceChecks, DistanceChecksTolerance)
	{
		const float ScaledVoxelSize = VoxelSize * (1 << LOD);

		if (!ensure(Bounds.Size() == FVector3d(ChunkSize * ScaledVoxelSize)))
		{
			VOXEL_MESSAGE(Error, "{0}: Invalid chunk size", this);
			return {};
		}
		if (!(4 <= ChunkSize && ChunkSize <= 128))
		{
			VOXEL_MESSAGE(Error, "{0}: Invalid chunk size {1}, needs to be between 4 and 128", this, ChunkSize);
			return {};
		}

		const int32 DataSize = ChunkSize + 1;

		const TValue<bool> ShouldSkip = INLINE_LAMBDA -> TValue<bool>
		{
			if (!EnableDistanceChecks)
			{
				return false;
			}

			const float Size = Bounds.Size().GetMax();
			const float Tolerance = FMath::Max(DistanceChecksTolerance, 0.f);

			const TSharedRef<FVoxelQueryParameters> Parameters = Query.CloneParameters();
			Parameters->Add<FVoxelGradientStepQueryParameter>().Step = ScaledVoxelSize;
			Parameters->Add<FVoxelPositionQueryParameter>().InitializeGrid(FVector3f(Bounds.Min) + Size / 4.f, Size / 2.f, FIntVector(2));
			Parameters->Add<FVoxelMinExactDistanceQueryParameter>().MinExactDistance = Size / 4.f * UE_SQRT_2;

			const TValue<FVoxelFloatBuffer> Distances = Get(DistancePin, Query.MakeNewQuery(Parameters));

			return
				MakeVoxelTask(STATIC_FNAME("DistanceChecks"))
				.Dependency(Distances)
				.Execute<bool>([=]
				{
					bool bCanSkip = true;
					for (const float Distance : Distances.Get_CheckCompleted())
					{
						if (FMath::Abs(Distance) < Size / 4.f * UE_SQRT_2 * (1.f + Tolerance))
						{
							bCanSkip = false;
						}
					}
					return bCanSkip;
				});
		};

		return VOXEL_ON_COMPLETE(Bounds, LOD, ChunkSize, EnableTransitions, PerfectTransitions, DataSize, ScaledVoxelSize, ShouldSkip)
		{
			if (ShouldSkip)
			{
				if (GVoxelMarchingCubeShowSkippedChunks)
				{
					FVoxelGameUtilities::DrawBox({}, Bounds, Query.GetQueryToWorld().Get_NoDependency(), FColor::Green);
				}
				return {};
			}

			if (GVoxelMarchingCubeShowComputedChunks)
			{
				FVoxelGameUtilities::DrawBox({}, Bounds, Query.GetQueryToWorld().Get_NoDependency(), FColor::Red);
			}

			const TSharedRef<FVoxelQueryParameters> Parameters = Query.CloneParameters();
			Parameters->Add<FVoxelGradientStepQueryParameter>().Step = ScaledVoxelSize;
			Parameters->Add<FVoxelPositionQueryParameter>().InitializeGrid(FVector3f(Bounds.Min), ScaledVoxelSize, FIntVector(DataSize));

			const TValue<FVoxelFloatBuffer> Distances = Get(DistancePin, Query.MakeNewQuery(Parameters));

			return VOXEL_ON_COMPLETE(Bounds, LOD, ChunkSize, EnableTransitions, PerfectTransitions, DataSize, ScaledVoxelSize, Distances)
			{
				if (Distances.IsConstant() ||
					!ensure(Distances.Num() == DataSize * DataSize * DataSize))
				{
					return {};
				}

				const TSharedRef<FVoxelMarchingCubeSurface> Surface = MakeVoxelShared<FVoxelMarchingCubeSurface>();
				Surface->LOD = LOD;
				Surface->ChunkSize = ChunkSize;
				Surface->ScaledVoxelSize = ScaledVoxelSize;
				Surface->ChunkBounds = Bounds;

				const TSharedRef<FVoxelMarchingCubeProcessor> Processor = MakeVoxelShared<FVoxelMarchingCubeProcessor>(
					ChunkSize,
					DataSize,
					ConstCast(Distances.GetStorage()),
					*Surface);
				Processor->bPerfectTransitions = PerfectTransitions;
				Processor->Generate(EnableTransitions);

				if (Surface->Cells.Num() == 0)
				{
					return {};
				}
				ensure(Surface->Indices.Num() > 0);

				if (!PerfectTransitions)
				{
					return Surface;
				}

				using FTransitionVertexToQuery = FVoxelMarchingCubeProcessor::FTransitionVertexToQuery;

				FVoxelFloatBufferStorage QueryX;
				FVoxelFloatBufferStorage QueryY;
				FVoxelFloatBufferStorage QueryZ;

				for (const TVoxelArray<FTransitionVertexToQuery>& Array : Processor->TransitionVerticesToQuery)
				{
					for (const FTransitionVertexToQuery& VertexToQuery : Array)
					{
						QueryX.Add(float(Bounds.Min.X) + VertexToQuery.PositionA.X * ScaledVoxelSize);
						QueryY.Add(float(Bounds.Min.Y) + VertexToQuery.PositionA.Y * ScaledVoxelSize);
						QueryZ.Add(float(Bounds.Min.Z) + VertexToQuery.PositionA.Z * ScaledVoxelSize);

						QueryX.Add(float(Bounds.Min.X) + VertexToQuery.PositionB.X * ScaledVoxelSize);
						QueryY.Add(float(Bounds.Min.Y) + VertexToQuery.PositionB.Y * ScaledVoxelSize);
						QueryZ.Add(float(Bounds.Min.Z) + VertexToQuery.PositionB.Z * ScaledVoxelSize);
					}
				}

				if (QueryX.Num() == 0 ||
					QueryY.Num() == 0 ||
					QueryZ.Num() == 0)
				{
					ensure(QueryX.Num() == 0);
					ensure(QueryY.Num() == 0);
					ensure(QueryZ.Num() == 0);
					return Surface;
				}

				const TSharedRef<FVoxelQueryParameters> TransitionParameters = Query.CloneParameters();
				TransitionParameters->Add<FVoxelLODQueryParameter>().LOD = LOD + 1;
				TransitionParameters->Add<FVoxelGradientStepQueryParameter>().Step = ScaledVoxelSize * 2;
				TransitionParameters->Add<FVoxelPositionQueryParameter>().Initialize(FVoxelVectorBuffer::Make(QueryX, QueryY, QueryZ));

				const TValue<FVoxelFloatBuffer> TransitionDistances = Get(DistancePin, Query.MakeNewQuery(TransitionParameters));

				return VOXEL_ON_COMPLETE(Surface, Processor, TransitionDistances)
				{
					int32 Index = 0;
					for (int32 Direction = 0; Direction < 6; Direction++)
					{
						for (const FTransitionVertexToQuery& VertexToQuery : Processor->TransitionVerticesToQuery[Direction])
						{
							const float ValueA = TransitionDistances[Index++];
							const float ValueB = TransitionDistances[Index++];

							const float Alpha = ValueA / (ValueA - ValueB);
							const FVector3f Position = FMath::Lerp(FVector3f(VertexToQuery.PositionA), FVector3f(VertexToQuery.PositionB), Alpha);

							Surface->TransitionVertices[Direction][VertexToQuery.Index].Position = Position;
						}
					}
					ensure(TransitionDistances.IsConstant() || TransitionDistances.Num() == Index);

					return Surface;
				};
			};
		};
	};
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_CreateMarchingCubeCollider, Collider)
{
	const TValue<FVoxelMarchingCubeSurface> Surface = Get(SurfacePin, Query);

	return VOXEL_ON_COMPLETE(Surface)
	{
		if (Surface->Vertices.Num() == 0)
		{
			return {};
		}

		check(Surface->Indices.Num() % 3 == 0);
		const int32 NumTriangles = Surface->Indices.Num() / 3;

		TValue<FVoxelPhysicalMaterialBuffer> Materials;
		{
			const TSharedRef<FVoxelQueryParameters> Parameters = Query.CloneParameters();
			Parameters->Add<FVoxelPositionQueryParameter>().Initialize([Surface, NumTriangles]
			{
				FVoxelFloatBufferStorage X; X.Allocate(NumTriangles);
				FVoxelFloatBufferStorage Y; Y.Allocate(NumTriangles);
				FVoxelFloatBufferStorage Z; Z.Allocate(NumTriangles);

				for (int32 Index = 0; Index < NumTriangles; Index++)
				{
					const FVector3f A = Surface->Vertices[Surface->Indices[3 * Index + 0]] * Surface->ScaledVoxelSize;
					const FVector3f B = Surface->Vertices[Surface->Indices[3 * Index + 1]] * Surface->ScaledVoxelSize;
					const FVector3f C = Surface->Vertices[Surface->Indices[3 * Index + 2]] * Surface->ScaledVoxelSize;

					const FVector3f Center = FVector3f(Surface->ChunkBounds.Min) + (A + B + C) / 3.f;

					X[Index] = Center.X;
					Y[Index] = Center.Y;
					Z[Index] = Center.Z;
				}

				return FVoxelVectorBuffer::Make(X, Y, Z);
			});

			Materials = Get(PhysicalMaterialPin, Query.MakeNewQuery(Parameters));
		}

		return VOXEL_ON_COMPLETE(Surface, Materials)
		{
			TVoxelArray<FVector3f> Positions;
			FVoxelUtilities::SetNumFast(Positions, Surface->Vertices.Num());

			for (int32 Index = 0; Index < Positions.Num(); Index++)
			{
				Positions[Index] = Surface->Vertices[Index] * Surface->ScaledVoxelSize;
			}

			if (Materials.IsConstant())
			{
				const TSharedPtr<FVoxelTriangleMeshCollider> Collider = FVoxelCollisionCooker::CookTriangleMesh(
					Surface->Indices,
					Positions,
					{});

				if (Collider)
				{
					Collider->Offset = Surface->ChunkBounds.Min;
					Collider->PhysicalMaterials.Add(Materials.GetConstant().Material);
				}
				return Collider;
			}

			TVoxelArray<uint16> MaterialIndices;
			TVoxelAddOnlyMap<uint64, uint16> MaterialToIndex;
			TVoxelArray<TWeakObjectPtr<UPhysicalMaterial>> PhysicalMaterials;

			for (const FVoxelPhysicalMaterial& Material : Materials)
			{
				uint16* IndexPtr = MaterialToIndex.Find(ReinterpretCastRef<uint64>(Material));
				if (!IndexPtr)
				{
					IndexPtr = &MaterialToIndex.Add_CheckNew(ReinterpretCastRef<uint64>(Material));
					*IndexPtr = PhysicalMaterials.Add(Material.Material);
				}
				MaterialIndices.Add(*IndexPtr);
			}

			const TSharedPtr<FVoxelTriangleMeshCollider> Collider = FVoxelCollisionCooker::CookTriangleMesh(
				Surface->Indices,
				Positions,
				MaterialIndices);

			if (Collider)
			{
				Collider->Offset = Surface->ChunkBounds.Min;
				Collider->PhysicalMaterials = MoveTemp(PhysicalMaterials);
			}
			return Collider;
		};
	};
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_CreateMarchingCubeMesh, Mesh)
{
	const TValue<FVoxelMarchingCubeSurface> Surface = Get(SurfacePin, Query);
	return VOXEL_ON_COMPLETE(Surface)
	{
		if (Surface->Vertices.Num() == 0)
		{
			return {};
		}

		const TValue<bool> GenerateDistanceField = Get(GenerateDistanceFieldPin, Query);
		const TValue<float> DistanceFieldBias = Get(DistanceFieldBiasPin, Query);
		const TValue<FVoxelMaterial> Material = Get(MaterialPin, Query);

		return VOXEL_ON_COMPLETE(Surface, GenerateDistanceField, DistanceFieldBias, Material)
		{
			FindVoxelQueryParameter(FVoxelLODQueryParameter, LODQueryParameter);

			const TSharedRef<FVoxelDetailTextureQueryHelper> DetailTextureHelper = MakeVoxelShared<FVoxelDetailTextureQueryHelper>(Surface);

			const TValue<FVoxelComputedMaterial> ComputedMaterial = INLINE_LAMBDA
			{
				const TSharedRef<FVoxelQueryParameters> MaterialParameters = Query.CloneParameters();
				MaterialParameters->Add<FVoxelDetailTextureQueryParameter>().Helper = DetailTextureHelper;
				return Material->Compute(Query.MakeNewQuery(MaterialParameters));
			};

			const FVoxelBox Bounds = FVoxelBox::FromPositions(Surface->Vertices);
			ensure(Bounds.IsValid());

			const int32 NumVertexNormals = LODQueryParameter->LOD == 0 ? Surface->Vertices.Num() : Surface->NumEdgeVertices;

			TValue<FVoxelVectorBuffer> VertexNormals;
			if (NumVertexNormals > 0)
			{
				const float Scale = Surface->ScaledVoxelSize;
				const FVector3f Offset = FVector3f(Surface->ChunkBounds.Min);

				const TSharedRef<FVoxelQueryParameters> VertexNormalParameters = Query.CloneParameters();
				VertexNormalParameters->Add<FVoxelGradientStepQueryParameter>().Step = Surface->ScaledVoxelSize;
				VertexNormalParameters->Add<FVoxelPositionQueryParameter>().Initialize([this, Query, Surface, NumVertexNormals, Scale, Offset]
				{
					const TVoxelArray<FVector3f>& Vertices = Surface->Vertices;

					FVoxelFloatBufferStorage X; X.Allocate(NumVertexNormals);
					FVoxelFloatBufferStorage Y; Y.Allocate(NumVertexNormals);
					FVoxelFloatBufferStorage Z; Z.Allocate(NumVertexNormals);

					for (int32 Index = 0; Index < NumVertexNormals; Index++)
					{
						const FVector3f Vertex = Vertices[Index];
						X[Index] = Offset.X + Scale * Vertex.X;
						Y[Index] = Offset.Y + Scale * Vertex.Y;
						Z[Index] = Offset.Z + Scale * Vertex.Z;
					}

					return FVoxelVectorBuffer::Make(X, Y, Z);
				}, Bounds.Scale(Scale).ShiftBy(FVector(Offset)));

				VertexNormals = VOXEL_CALL_NODE(FVoxelNode_GetGradient, GradientPin, Query.MakeNewQuery(VertexNormalParameters))
				{
					VOXEL_CALL_NODE_BIND(ValuePin)
					{
						return GetNodeRuntime().Get(DistancePin, Query);
					};
				};
			}
			else
			{
				VertexNormals = FVoxelVectorBuffer();
			}

			checkStatic(DistanceField::MeshDistanceFieldObjectBorder == 1);

			const float ChunkWorldSize = Surface->ChunkSize * Surface->ScaledVoxelSize;
			const int32 NumBricksPerChunk = GVoxelMarchingCubeNumDistanceFieldBricksPerChunk;
			const int32 NumQueriesPerChunk = NumBricksPerChunk * DistanceField::UniqueDataBrickSize;
			// 2 * MeshDistanceFieldObjectBorder, additional padding at the end to overlap chunks
			const int32 NumQueriesInChunk = NumQueriesPerChunk - 2 - (GVoxelMarchingCubeEnableDistanceFieldPadding ? 1 : 0);
			const float TexelSize = ChunkWorldSize / float(NumQueriesInChunk);

			// +1 so we can query the last brick element
			// +1 to be a multiple of 2
			const int32 DenseQuerySize = NumQueriesPerChunk + 2;
			const FVoxelBox QueryBounds(0, ChunkWorldSize + (GVoxelMarchingCubeEnableDistanceFieldPadding ? TexelSize : 0));

			TValue<FVoxelFloatBuffer> Distances;
			if (GenerateDistanceField)
			{
				const TSharedRef<FVoxelQueryParameters> Parameters = Query.CloneParameters();
				Parameters->Add<FVoxelGradientStepQueryParameter>().Step = TexelSize;

				Parameters->Add<FVoxelPositionQueryParameter>().InitializeGrid(
					FVector3f(Surface->ChunkBounds.Min + QueryBounds.Min - TexelSize),
					TexelSize,
					FIntVector(DenseQuerySize));

				Distances = Get(DistancePin, Query.MakeNewQuery(Parameters));
			}
			else
			{
				Distances = FVoxelFloatBuffer();
			}

			return VOXEL_ON_COMPLETE(
				Surface,
				GenerateDistanceField,
				DistanceFieldBias,
				LODQueryParameter,
				DetailTextureHelper,
				ComputedMaterial,
				Bounds,
				NumVertexNormals,
				VertexNormals,
				QueryBounds,
				NumBricksPerChunk,
				DenseQuerySize,
				Distances)
			{
				const TSharedRef<FVoxelMarchingCubeMesh> Mesh = MakeVoxelMesh<FVoxelMarchingCubeMesh>();
				Mesh->LOD = LODQueryParameter->LOD;
				Mesh->Bounds = Bounds.Scale(Surface->ScaledVoxelSize);
				Mesh->VoxelSize = Surface->ScaledVoxelSize;
				Mesh->ChunkSize = Surface->ChunkSize;
				Mesh->NumCells = Surface->Cells.Num();
				Mesh->NumEdgeVertices = Surface->NumEdgeVertices;
				Mesh->ComputedMaterial = ComputedMaterial;

				Mesh->Indices = Surface->Indices;
				Mesh->Vertices = Surface->Vertices;
				Mesh->CellIndices = Surface->CellIndices;

				Mesh->TransitionIndices = Surface->TransitionIndices;
				Mesh->TransitionVertices = Surface->TransitionVertices;
				Mesh->TransitionCellIndices = Surface->TransitionCellIndices;

				Mesh->CellIndexToDirection = DetailTextureHelper->BuildCellIndexToDirection();
				Mesh->CellTextureCoordinates = DetailTextureHelper->BuildCellCoordinates();

				if (VertexNormals.Num() > 0 &&
					ensure(VertexNormals.IsConstant() || VertexNormals.Num() == NumVertexNormals))
				{
					Mesh->bHasVertexNormals = NumVertexNormals == Surface->Vertices.Num();

					FVoxelUtilities::SetNumFast(Mesh->VertexNormals, NumVertexNormals);

					for (int32 Index = 0; Index < NumVertexNormals; Index++)
					{
						Mesh->VertexNormals[Index] = VertexNormals[Index].GetSafeNormal();
					}
				}

				if (GenerateDistanceField)
				{
					{
						VOXEL_SCOPE_COUNTER("Cards");

						const FVoxelBox CardBounds = Bounds.Scale(Surface->ScaledVoxelSize);

						Mesh->CardRepresentationData = MakeVoxelShared<FCardRepresentationData>();
						Mesh->CardRepresentationData->MeshCardsBuildData.Bounds = CardBounds.ToFBox();

						for (int32 Index = 0; Index < 6; Index++)
						{
							FLumenCardBuildData& CardBuildData = Mesh->CardRepresentationData->MeshCardsBuildData.CardBuildData.Emplace_GetRef();

							const uint32 AxisIndex = Index / 2;
							FVector3f Direction(0.0f, 0.0f, 0.0f);
							Direction[AxisIndex] = Index & 1 ? 1.0f : -1.0f;

							CardBuildData.OBB.AxisZ = Direction;
							CardBuildData.OBB.AxisZ.FindBestAxisVectors(CardBuildData.OBB.AxisX, CardBuildData.OBB.AxisY);
							CardBuildData.OBB.AxisX = FVector3f::CrossProduct(CardBuildData.OBB.AxisZ, CardBuildData.OBB.AxisY);
							CardBuildData.OBB.AxisX.Normalize();

							CardBuildData.OBB.Origin = FVector3f(CardBounds.GetCenter());
							CardBuildData.OBB.Extent = CardBuildData.OBB.RotateLocalToCard(FVector3f(CardBounds.GetExtent()) + FVector3f(1.0f)).GetAbs();

							CardBuildData.AxisAlignedDirectionIndex = Index;
						}
					}

					VOXEL_SCOPE_COUNTER("Distance Field");

					FVoxelDistanceFieldWrapper Wrapper(QueryBounds.ToFBox());
					Wrapper.SetSize(FIntVector(NumBricksPerChunk));

					const FVoxelIntBox BrickBoundsA(0, NumBricksPerChunk);
					const FVoxelIntBox BrickBoundsB(0, DistanceField::BrickSize);

					BrickBoundsA.Iterate([&](const FIntVector& BrickPositionA)
					{
						FVoxelDistanceFieldWrapper::FMip& Mip = Wrapper.Mips[0];
						FVoxelDistanceFieldWrapper::FBrick& Brick = Mip.FindOrAddBrick(BrickPositionA);
						BrickBoundsB.Iterate([&](const FIntVector& BrickPositionB)
						{
							// 7 unique voxel, and 1 voxel shared with the next brick
							// Shader samples between [0.5, 7.5] for good interpolation
							const FIntVector DistancePosition = BrickPositionA * DistanceField::UniqueDataBrickSize + BrickPositionB;
							const int32 DistanceIndex = FVoxelUtilities::Get3DIndex<int32>(DenseQuerySize, DistancePosition);
							const float Distance = Distances[DistanceIndex] + DistanceFieldBias;

							const int32 BrickIndex = FVoxelUtilities::Get3DIndex<int32>(DistanceField::BrickSize, BrickPositionB);
							Brick[BrickIndex] = Mip.QuantizeDistance(Distance);
						});
					});

					Wrapper.Mips[1] = Wrapper.Mips[0];
					Wrapper.Mips[2] = Wrapper.Mips[0];

					Mesh->DistanceFieldVolumeData = Wrapper.Build();
					//Mesh->DistanceFieldVolumeData->LocalSpaceMeshBounds = Mesh->DistanceFieldVolumeData->LocalSpaceMeshBounds.ShiftBy(-FVector(Surface->ScaledVoxelSize / 2.f));
				}

				return Mesh;
			};
		};
	};
}