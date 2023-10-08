// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Collision/VoxelTriangleMeshCollider.h"
#include "Navigation/VoxelNavmesh.h"
#include "MeshBatch.h"
#include "LocalVertexFactory.h"
#include "PhysicsEngine/BodySetup.h"

namespace Chaos
{
	template <typename, typename>
	struct FTriangleMeshSweepVisitorCCD;

	template<>
	struct FTriangleMeshSweepVisitorCCD<void, void>
	{
		static int64 GetAllocatedSize(const FTriangleMeshImplicitObject& TriangleMesh)
		{
			int64 AllocatedSize = sizeof(FTriangleMeshImplicitObject);
			AllocatedSize += TriangleMesh.MParticles.GetAllocatedSize();

			if (TriangleMesh.MElements.RequiresLargeIndices())
			{
				AllocatedSize += TriangleMesh.MElements.GetLargeIndexBuffer().GetAllocatedSize();
			}
			else
			{
				AllocatedSize += TriangleMesh.MElements.GetSmallIndexBuffer().GetAllocatedSize();
			}

			AllocatedSize += TriangleMesh.MaterialIndices.GetAllocatedSize();

			if (TriangleMesh.ExternalFaceIndexMap)
			{
				AllocatedSize += TriangleMesh.ExternalFaceIndexMap->GetAllocatedSize();
			}
			if (TriangleMesh.ExternalVertexIndexMap)
			{
				AllocatedSize += TriangleMesh.ExternalVertexIndexMap->GetAllocatedSize();
			}

			AllocatedSize += TriangleMesh.FastBVH.Nodes.GetAllocatedSize();
			AllocatedSize += TriangleMesh.FastBVH.FaceBounds.GetAllocatedSize();

			return AllocatedSize;
		}
	};
}

int64 FVoxelTriangleMeshCollider::GetAllocatedSize() const
{
	int64 AllocatedSize = sizeof(*this);
	if (TriangleMesh)
	{
		AllocatedSize += Chaos::FTriangleMeshSweepVisitorCCD<void, void>::GetAllocatedSize(*TriangleMesh);
	}
	AllocatedSize += PhysicalMaterials.GetAllocatedSize();
	return AllocatedSize;
}

void FVoxelTriangleMeshCollider::AddToBodySetup(UBodySetup& BodySetup) const
{
	VOXEL_FUNCTION_COUNTER();

	if (!ensure(TriangleMesh))
	{
		return;
	}

	// Copied from UBodySetup::FinishCreatingPhysicsMeshes_Chaos
#if TRACK_CHAOS_GEOMETRY
	TriangleMesh->Track(Chaos::MakeSerializable(TriangleMesh), "Voxel Mesh");
#endif

	ensure(!TriangleMesh->GetDoCollide());

	BodySetup.ChaosTriMeshes.Add(TriangleMesh);
}

TSharedPtr<const FVoxelNavmesh> FVoxelTriangleMeshCollider::GetNavmesh() const
{
	VOXEL_FUNCTION_COUNTER();

	if (!TriangleMesh)
	{
		return nullptr;
	}

	const TSharedRef<FVoxelNavmesh> Navmesh = MakeVoxelShared<FVoxelNavmesh>();
	Navmesh->Offset = Offset;
	Navmesh->LocalBounds = LocalBounds;
	Navmesh->Vertices = TVoxelArray<FVector3f>(ReinterpretCastVoxelArrayView<FVector3f>(TriangleMesh->Particles().XArray()));

	if (TriangleMesh->Elements().RequiresLargeIndices())
	{
		const TConstVoxelArrayView<int32> Indices = ReinterpretCastVoxelArrayView<int32>(TriangleMesh->Elements().GetLargeIndexBuffer());
		Navmesh->Indices = TVoxelArray<int32>(Indices);
	}
	else
	{
		const TConstVoxelArrayView<uint16> Indices = ReinterpretCastVoxelArrayView<uint16>(TriangleMesh->Elements().GetSmallIndexBuffer());
		FVoxelUtilities::SetNumFast(Navmesh->Indices, Indices.Num());
		for (int32 Index = 0; Index < Indices.Num(); Index++)
		{
			Navmesh->Indices[Index] = Indices[Index];
		}
	}

	// Chaos triangles are reversed
	for (int32 Index = 0; Index < Navmesh->Indices.Num(); Index += 3)
	{
		Swap(
			Navmesh->Indices[Index + 0],
			Navmesh->Indices[Index + 2]);
	}

	return Navmesh;
}

TSharedPtr<IVoxelColliderRenderData> FVoxelTriangleMeshCollider::GetRenderData() const
{
	if (!ensure(TriangleMesh) ||
		!ensure(TriangleMesh->Elements().GetNumTriangles() > 0))
	{
		return nullptr;
	}

	return MakeVoxelShared<FVoxelTriangleMeshCollider_RenderData>(*this);
}

TArray<TWeakObjectPtr<UPhysicalMaterial>> FVoxelTriangleMeshCollider::GetPhysicalMaterials() const
{
	return PhysicalMaterials;
}

void FVoxelTriangleMeshCollider::BulkRaycast(
	const bool bDebug,
	const TConstVoxelArrayView<FVector3f> LocalRayPositions,
	const TConstVoxelArrayView<FVector3f> LocalRayDirections,
	TVoxelArray<FVector3f>& OutLocalHitPositions,
	TVoxelArray<FVector3f>& OutLocalHitNormals) const
{
	VOXEL_SCOPE_COUNTER_FORMAT("FVoxelTriangleMeshCollider::BulkRaycast Num=%d", LocalRayPositions.Num());

	if (!ensure(TriangleMesh))
	{
		return;
	}

	check(LocalRayPositions.Num() == LocalRayDirections.Num());

	struct FTrace
	{
		FVector Start = FVector(ForceInit);
		FVector Direction = FVector(ForceInit);
		bool bHit = false;
		FVector HitPosition = FVector(ForceInit);
		FVector HitNormal = FVector(ForceInit);
	};
	TArray<FTrace> DebugTraces;

	for (int32 Index = 0; Index < LocalRayPositions.Num(); Index++)
	{
		FVector RayPosition = FVector(LocalRayPositions[Index]);
		const FVector RayDirection = FVector(LocalRayDirections[Index]);

		for (int32 RecursiveHit = 0; ensure(RecursiveHit < 100); RecursiveHit++)
		{
			double Time = 0;
			int32 FaceIndex = 0;

			Chaos::FVec3 HitPosition;
			Chaos::FVec3 HitNormal;
			if (!TriangleMesh->Raycast(
				RayPosition,
				RayDirection,
				1.e9f,
				0.f,
				Time,
				HitPosition,
				HitNormal,
				FaceIndex))
			{
				if (bDebug)
				{
					DebugTraces.Add({ RayPosition, RayDirection, false });
				}
				break;
			}

			if (bDebug)
			{
				DebugTraces.Add({ RayPosition, RayDirection, true, HitPosition, HitNormal });
			}

			OutLocalHitPositions.Add(FVector3f(HitPosition));
			OutLocalHitNormals.Add(FVector3f(HitNormal));

			RayPosition = HitPosition + RayDirection * 0.001;
		}
	}

	if (bDebug)
	{
		FVoxelUtilities::RunOnGameThread([Offset = Offset, LocalBounds = LocalBounds, DebugTraces]
		{
			for (const FTrace& Trace : DebugTraces)
			{
				double TimeMin = 0;
				double TimeMax = 0;
				LocalBounds.RayBoxIntersection(Trace.Start, Trace.Direction, TimeMin, TimeMax);

				FVector Start = Trace.Start;
				if (!LocalBounds.Contains(Start))
				{
					Start = Trace.Start + Trace.Direction * TimeMin;
				}

				if (!Trace.bHit)
				{
					DrawDebugLine(
						GWorld,
						Offset + Start,
						Offset + Trace.Start + Trace.Direction * TimeMax,
						FColor::Red,
						false,
						10.f,
						0,
						2.f);

					continue;
				}

				DrawDebugLine(
					GWorld,
					Offset + Start,
					Offset + Trace.HitPosition,
					FColor::Green,
					false,
					10.f,
					0,
					2.f);

				DrawDebugDirectionalArrow(
					GWorld,
					Offset + Trace.HitPosition,
					Offset + Trace.HitPosition + Trace.HitNormal * 100.f,
					10.f,
					FColor::Blue,
					false,
					10.f,
					0,
					2.f);
			}
		});
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelTriangleMeshCollider_RenderData::FVoxelTriangleMeshCollider_RenderData(const FVoxelTriangleMeshCollider& Collider)
{
	VOXEL_FUNCTION_COUNTER();

	if (!ensure(Collider.TriangleMesh))
	{
		return;
	}

	TVoxelArray<uint32> Indices;
	TVoxelArray<FVector3f> Vertices;
	{
		const auto Process = [&](auto Elements)
		{
			for (const auto& Element : Elements)
			{
				Indices.Add(Vertices.Num());
				Vertices.Add(Collider.TriangleMesh->Particles().X(Element[0]));

				Indices.Add(Vertices.Num());
				Vertices.Add(Collider.TriangleMesh->Particles().X(Element[1]));

				Indices.Add(Vertices.Num());
				Vertices.Add(Collider.TriangleMesh->Particles().X(Element[2]));
			}
		};

		if (Collider.TriangleMesh->Elements().RequiresLargeIndices())
		{
			Process(Collider.TriangleMesh->Elements().GetLargeIndexBuffer());
		}
		else
		{
			Process(Collider.TriangleMesh->Elements().GetSmallIndexBuffer());
		}
	}

	IndexBuffer.SetIndices(Indices.ToConstArray(), EIndexBufferStride::Force32Bit);
	PositionVertexBuffer.Init(Vertices.Num(), false);
	StaticMeshVertexBuffer.Init(Vertices.Num(), 1, false);
	ColorVertexBuffer.Init(Vertices.Num(), false);

	for (int32 Index = 0; Index < Vertices.Num(); Index++)
	{
		PositionVertexBuffer.VertexPosition(Index) = Vertices[Index];
		StaticMeshVertexBuffer.SetVertexUV(Index, 0, FVector2f::ZeroVector);
		ColorVertexBuffer.VertexColor(Index) = FColor::Black;
	}

	for (int32 Index = 0; Index < Vertices.Num(); Index += 3)
	{
		const FVector3f Normal = FVoxelUtilities::GetTriangleNormal(
			Vertices[Index + 0],
			Vertices[Index + 1],
			Vertices[Index + 2]);

		const FVector3f Tangent = FVector3f::ForwardVector;
		const FVector3f Bitangent = FVector3f::CrossProduct(Normal, Tangent);

		StaticMeshVertexBuffer.SetVertexTangents(Index + 0, Tangent, Bitangent, Normal);
		StaticMeshVertexBuffer.SetVertexTangents(Index + 1, Tangent, Bitangent, Normal);
		StaticMeshVertexBuffer.SetVertexTangents(Index + 2, Tangent, Bitangent, Normal);
	}

	FRHICommandListImmediate& RHICmdList = FRHICommandListExecutor::GetImmediateCommandList();
	IndexBuffer.InitResource(UE_503_ONLY(RHICmdList));
	PositionVertexBuffer.InitResource(UE_503_ONLY(RHICmdList));
	StaticMeshVertexBuffer.InitResource(UE_503_ONLY(RHICmdList));
	ColorVertexBuffer.InitResource(UE_503_ONLY(RHICmdList));

	VertexFactory = MakeUnique<FLocalVertexFactory>(GMaxRHIFeatureLevel, "FVoxelTriangleMeshCollider_Chaos_RenderData");

	FLocalVertexFactory::FDataType Data;
	PositionVertexBuffer.BindPositionVertexBuffer(VertexFactory.Get(), Data);
	StaticMeshVertexBuffer.BindTangentVertexBuffer(VertexFactory.Get(), Data);
	StaticMeshVertexBuffer.BindPackedTexCoordVertexBuffer(VertexFactory.Get(), Data);
	ColorVertexBuffer.BindColorVertexBuffer(VertexFactory.Get(), Data);

	VertexFactory->SetData(Data);
	VertexFactory->InitResource(UE_503_ONLY(RHICmdList));
}

FVoxelTriangleMeshCollider_RenderData::~FVoxelTriangleMeshCollider_RenderData()
{
	IndexBuffer.ReleaseResource();
	PositionVertexBuffer.ReleaseResource();
	StaticMeshVertexBuffer.ReleaseResource();
	ColorVertexBuffer.ReleaseResource();
	VertexFactory->ReleaseResource();
}

void FVoxelTriangleMeshCollider_RenderData::Draw_RenderThread(const FPrimitiveSceneProxy& Proxy, FMeshBatch& MeshBatch)
{
	MeshBatch.Type = PT_TriangleList;
	MeshBatch.VertexFactory = VertexFactory.Get();

	FMeshBatchElement& BatchElement = MeshBatch.Elements[0];
	BatchElement.IndexBuffer = &IndexBuffer;
	BatchElement.FirstIndex = 0;
	BatchElement.NumPrimitives = IndexBuffer.GetNumIndices() / 3;
	BatchElement.MinVertexIndex = 0;
	BatchElement.MaxVertexIndex = PositionVertexBuffer.GetNumVertices() - 1;
}